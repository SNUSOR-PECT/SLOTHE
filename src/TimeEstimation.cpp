#include "TimeEstimation.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
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

// Wrapper: call this version!
bool isDerivedFrom(Value *V, Value *target) {
    std::unordered_set<Value*> visited;
    return isDerivedFromImpl(V, target, visited);
}

static cl::opt<std::string> TargetFunc(
    "target-func",
    cl::desc("Target function name"),
    cl::value_desc("function name"),
    cl::init("")
);

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------
void TimeEstimation::traceFunction(llvm::Function *Func, std::set<std::string> &visited, int* requiredLvl, llvm::Value* input) {
    if (!Func || visited.count(Func->getName().str())) return;
    visited.insert(Func->getName().str());

    llvm::errs() << "Tracing function: " << Func->getName() << "\n";
    for (auto &BB : *Func) {
        for (auto &I : BB) {
            if (auto *call = llvm::dyn_cast<llvm::CallInst>(&I)) {
                if (llvm::Function *callee = call->getCalledFunction()) {
                    if (!callee->isDeclaration()) {
                        // if a polynomial -> get degree and estimate time
                        // if a inverse -> get a d and estimate time
                        traceFunction(callee, visited, requiredLvl, input);
                    }
                }
            } 
            else {
                if ((I.getOpcode() == Instruction::FMul) && (isDerivedFrom(&I, input))) {
                    *requiredLvl += 1;
                }
            }
        }
    }
}
PreservedAnalyses TimeEstimation::run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
    std::set<std::string> visited;
    int requiredLvl = 0;
    if (Function *root = M.getFunction(TargetFunc)) {
        llvm::Value* input = root->getArg(0);
        traceFunction(root, visited, &requiredLvl, input);
    }

    errs() << "Total required lvl = " << requiredLvl << "\n";
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
