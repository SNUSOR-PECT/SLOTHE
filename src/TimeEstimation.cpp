#include "TimeEstimation.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

using namespace llvm;

#define DEBUG_TYPE "estimate-time"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
bool isDerivedFromImpl(Value *V, Value *target, std::unordered_set<Value*> &visited) {
    if (V == target) return true;
    if (!V) return false;

    // Stop recursion if we've already checked this node
    if (visited.count(V)) return false;
    visited.insert(V);

    if (auto *I = dyn_cast<Instruction>(V)) {
        for (unsigned i = 0; i < I->getNumOperands(); ++i) {
            if (isDerivedFromImpl(I->getOperand(i), target, visited))
                return true;
        }
    }
    return false;
}

// Wrapper
bool isDerivedFrom(Value *V, Value *target) {
    std::unordered_set<Value*> visited;
    return isDerivedFromImpl(V, target, visited);
}

int getDepthToInput(Value *V, Value *target, std::unordered_set<Value*> &visited) {
    if (!V) return 0;
    if (V == target) return 0; // Don't count the input
    if (visited.count(V)) return 0;
    visited.insert(V);

    int count = 0;

    if (auto *I = llvm::dyn_cast<Instruction>(V)) {
        if ((I->getOpcode() == Instruction::FMul) && (isDerivedFrom(I, target))) {
            count += 1;
        }
        // Check all operands recursively
        for (unsigned i = 0; i < I->getNumOperands(); ++i) {
            count += getDepthToInput(I->getOperand(i), target, visited);
        }
    }
    return count;
}

// Wrapper
int getDepth(Value *retVal, Value *target) {
    std::unordered_set<Value*> visited;
    return getDepthToInput(retVal, target, visited);
}

static cl::opt<std::string> TargetFunc(
    "target-func",
    cl::desc("Target function name"),
    cl::value_desc("function name"),
    cl::init("")
);

static llvm::cl::opt<std::string> paramFile(
    "param-path",
    llvm::cl::desc("Specify FHE parameter file"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("default.txt")
);
//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------
void calInv(int lvl, int d, std::vector<std::pair<std::string, int>>& opLvl) {
    int curL = lvl;
    opLvl.push_back({"PAdd", curL}); // a = 2-x
    opLvl.push_back({"PAdd", curL}); // b = 1-x
    for (int i=0; i<d-1; i++) {
        // b = b*b
        opLvl.push_back({"CMult", curL++}); 
        // a = a*(1+b)
        opLvl.push_back({"PAdd", curL}); 
        opLvl.push_back({"CMult", curL++});
    }
}

// for odd-only/even-only polynomial, it can be optimized more,
// this assumes all coefficients are valid
void calPoly(int lvl, int deg, std::vector<std::pair<std::string, int>>& opLvl) {
    int curL = lvl;
    
    // compute_all_powers
    std::vector<int> levels(deg+1, 0);
    std::vector<int> powers(deg+1);
    powers[1] = curL;

    for (int i=2; i<=deg; i++) {
        // compute x^i
        int minlv = i;
        int cand = -1;
        for (int j=1; j<=i/2; j++) {
            int k = i-j;
            int newlv = std::max(levels[j], levels[k]) + 1;
            if (newlv < minlv) {
                cand = j;
                minlv = newlv;
            }
        }
        levels[i] = minlv;
        if (cand<0) errs() << "Out of range";

        int tmp = powers[cand];
        int opL = tmp > powers[i-cand] ? tmp : powers[i-cand];
        powers[i] = opL+1;
        opLvl.push_back({"CMult", opL});
    }

    for (int i=1; i<=deg; i++) { // deg = 18
        opLvl.push_back({"PMult", powers[i]});
        opLvl.push_back({"CAdd", powers[i]+1}); // a = 2-x
    }
}

void getDeg(int* deg, llvm::Function *Func) {
    int cnt = -1;
    for (auto &BB : *Func) {
        for ([[maybe_unused]] auto &I : BB) {
            cnt++;
        }
    }
    *deg = cnt;
}

void TimeEstimation::traceFunction(llvm::Function *Func, std::set<std::string> &visited, llvm::Value* input, std::vector<std::pair<std::string, int>>& opLvl) {
    if (!Func || visited.count(Func->getName().str())) return;
    visited.insert(Func->getName().str());

    int lvl4Funcs = 0;
    for (auto &BB : *Func) {
        for (auto &I : BB) {
            if (I.getOpcode() == Instruction::Ret) break;
            else if (auto *call = llvm::dyn_cast<llvm::CallInst>(&I)) {
                if (llvm::Function *callee = call->getCalledFunction()) {
                    if (callee->getName() == "_inverse") {
                        // (1) get d
                        llvm::Value *op1 = call->getArgOperand(1);
                        int d = llvm::dyn_cast<llvm::ConstantInt>(op1)->getSExtValue();

                        // (2) get starting level
                        Value *op0 = call->getArgOperand(0);
                        int opL = getDepth(op0, input)+lvl4Funcs;

                        // (3) get required level for evaluating inverse
                        int reqLvl = 2 * (d-1);
                        lvl4Funcs += reqLvl;

                        // (4) push operations
                        calInv(opL, d, opLvl);                        
                    } else { // polynomial
                        // (1) get deg
                        int deg = 0;
                        getDeg(&deg, callee);

                        // (2) get starting level
                        llvm::Value *arg = call->getArgOperand(0);
                        int opL = getDepth(arg, input)+lvl4Funcs;

                        // (3) get required level for evaluating inverse
                        int reqLvl = ceil(log2(deg))+1;
                        lvl4Funcs += reqLvl;

                        // (4) push operations
                        calPoly(opL, deg, opLvl);
                    }
                }
            } 
            else {
                Value *op0 = I.getOperand(0);
                Value *op1 = I.getOperand(1);
                
                if (isDerivedFrom(&I, input)) {
                    if (llvm::isa<llvm::Constant>(op1)) {
                        // PMult or PAdd
                        int opL = getDepth(op0, input)+lvl4Funcs;
                        if (I.getOpcode() == Instruction::FMul) { opLvl.push_back({"PMult", opL}); }
                        else if (I.getOpcode() == Instruction::FAdd) { opLvl.push_back({"PAdd", opL}); }
                    } else {
                        int Lvl0 = getDepth(op0, input);
                        int Lvl1 = getDepth(op1, input);
                        int opL = Lvl0 > Lvl1 ? Lvl0+lvl4Funcs : Lvl1+lvl4Funcs; 
                        // CMult or CAdd
                        if (I.getOpcode() == Instruction::FMul) { opLvl.push_back({"CMult", opL}); }
                        else if (I.getOpcode() == Instruction::FAdd) { opLvl.push_back({"CAdd", opL}); }
                    }
                }
            }
        }
    }
}

PreservedAnalyses TimeEstimation::run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
    std::set<std::string> visited;
    std::vector<std::pair<std::string, int>> opLvl;

    if (Function *root = M.getFunction(TargetFunc)) {
        llvm::Value* input = root->getArg(0);
        traceFunction(root, visited, input, opLvl);
    }

    int totalLvl = opLvl[opLvl.size()-1].second;

    // for (std::size_t i=0; i<opLvl.size(); i++) {
    //     errs() << "op = " << opLvl[i].first << " : " << opLvl[i].second << "\n";
    // }

    return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTimeEstimationPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "estimate-time", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "estimate-time") {
                    MPM.addPass(TimeEstimation(llvm::errs()));
                    return true;
                  }
                  return false;
                });
        }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getTimeEstimationPluginInfo();
}
