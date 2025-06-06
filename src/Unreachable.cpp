#include "Unreachable.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <tuple>
#include <vector>
#include <stack>

using namespace llvm;

#define DEBUG_TYPE "remove-unreachable"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
static cl::opt<double> _min(
    "val-min",
    cl::desc("minimum value"),
    cl::value_desc("minimum"),
    cl::init(0.0)
);
static cl::opt<double> _max(
    "val-max",
    cl::desc("maximum value"),
    cl::value_desc("maximum"),
    cl::init(0.0)
);

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------
using opcode_t = unsigned;
bool isHighWord(Value* val) {

  std::vector<opcode_t> seq_rev = {Instruction::Trunc, Instruction::LShr, Instruction::BitCast}; // reversed order of inst sequence

  int idx = 0;
  bool isValid = true;
  while(1) {
      if (Instruction *I = dyn_cast<Instruction>(val)) {
          if (I->getOpcode()!=seq_rev[idx++]) isValid=false;
          val = I->getOperand(0);
          continue;
      }
      break;
  }

  return isValid;
}

bool isHighAbsX(Value* val) {
  bool res = true;

  if (auto *binOp = dyn_cast<BinaryOperator>(val)) {
    // (1) check operator
    if (binOp->getOpcode() != llvm::Instruction::And) res = false;
    // (2) check operands
    if (!isHighWord(binOp->getOperand(0))) res = false;
    if (ConstantInt *CI = dyn_cast<ConstantInt>(binOp->getOperand(1))) {
      if (CI->getSExtValue() != 2147483647) res = false;
    }
  }

  return res;
}

int isIEEESpecialValBranch(llvm::BasicBlock* BB, llvm::ICmpInst *I) {
  using Pred = llvm::ICmpInst::Predicate;

  Value *RHS = I->getOperand(1);

  if (auto *CI = llvm::dyn_cast<llvm::ConstantInt>(RHS)) {
    uint32_t uVal = CI->getZExtValue();
    int32_t  sVal = CI->getSExtValue();

    if (isHighAbsX(I->getOperand(0))) {
      // case 1) sub-normal values
      if (I->getPredicate() == Pred::ICMP_ULT) {
        if (uVal <= 1043333120) return 1; // -55
        // if (uVal == 1015021568) return 1; // -55
        // if (uVal == 1043333120) return 1; // -28
      }

      // case 2) INF or NaN
      if (I->getPredicate() == Pred::ICMP_UGT) {
        if (uVal == 2146435071) return 2;
      }
    } else {
      // case 3) +-0 or exact 0
      if ((isHighAbsX(llvm::dyn_cast<llvm::Instruction>(I->getOperand(0))->getOperand(1))) && I->getPredicate() == Pred::ICMP_EQ) {
        if (sVal == 0) return 3;
      }
    }
  }

  return 0;
}


double highXtoDouble(uint32_t val) {
  uint64_t tmp = (static_cast<uint64_t>(val) << 32);  // low word assumed to be 0
  double res;
  std::memcpy(&res, &tmp, sizeof(res));
  return res;
}

// x in range : _min < x < _max
int checkValidRange(llvm::ICmpInst *I) {
  using Pred = llvm::ICmpInst::Predicate;

  Value *RHS = I->getOperand(1);

  if (auto *CI = llvm::dyn_cast<llvm::ConstantInt>(RHS)) {
    int32_t sVal = CI->getSExtValue();
    double dVal = highXtoDouble(sVal);

    Pred pred = I->getPredicate();
    if (ICmpInst::isLT(pred) || ICmpInst::isLE(pred)) {
      // |x| < dVal
      if (dVal <= _min) return 0;     // always false
      if (dVal > _max) return 1;      // always true
    } else if (ICmpInst::isGT(pred) || ICmpInst::isGE(pred)) {
      // |x| > dVal
      if (dVal >= _max) return 0;     // always false
      if (dVal < _min) return 1;      // always true
    }
  }

  return -1; // both can be valid
}

PreservedAnalyses Unreachable::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &FM) {
  bool modified = false;

  for (auto &BB : Func) {
    if (auto *brInst = dyn_cast<BranchInst>(BB.getTerminator())) {
      if (!brInst->isConditional()) continue;

      ICmpInst *ICmp = dyn_cast<ICmpInst>(brInst->getCondition());

      // 1) check if the branch indicates IEEE Special values
      int tag = isIEEESpecialValBranch(&BB, ICmp);
      switch (tag) {
        case 1:
        case 2:
        case 3:
          brInst->setSuccessor(0, brInst->getSuccessor(1)); // always false
          modified = true;
          continue;
        default:
          break;
      }

      // 2) check if the branch indicates unreachable path
      int isValid = checkValidRange(ICmp);
      if (isValid == 0 || isValid == 1) {
        BasicBlock *BBParent = brInst->getParent();

        unsigned removeIdx = isValid == 0 ? 0 : 1; // which successor is unreachable
        unsigned keepIdx   = 1 - removeIdx;

        BasicBlock *deadSucc = brInst->getSuccessor(removeIdx);
        BasicBlock *liveSucc = brInst->getSuccessor(keepIdx);

        // Redirect branch to always go to the live successor
        brInst->setSuccessor(0, liveSucc);
        brInst->setSuccessor(1, liveSucc);

        // Update all phi nodes in the dead successor
        for (Instruction &I : *deadSucc) {
          if (auto *phi = dyn_cast<PHINode>(&I)) {
            phi->removeIncomingValue(BBParent, false); // DeletePHIIfEmpty
          }
        }

        modified = true;
      }
    }
  }

  return modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getUnreachablePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "remove-unreachable", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "remove-unreachable") {
                    FPM.addPass(Unreachable(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getUnreachablePluginInfo();
}
