#include "GetDivisorRange.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

#define DEBUG_TYPE "get-div-range"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------

bool isDerivedFrom(Value *V, Value *target) {
    if (V == target) return true;

    if (auto *I = dyn_cast<Instruction>(V)) {
        for (unsigned i = 0; i < I->getNumOperands(); ++i) {
            if (isDerivedFrom(I->getOperand(i), target)) {
                return true;
            }
        }
    }
    return false;
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

PreservedAnalyses GetDivisorRange::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &)  {
  bool Changed = false;
  bool flag = 0;

  // detect division
  Value* fdiv_detected = nullptr;
  for (auto &BB : Func) {
    for (auto &I : BB) {
      if (I.getOpcode() == Instruction::FDiv) {
        Value* tmp = I.getOperand(1);
        if (isDerivedFrom(&I, Func.getArg(0))) {
          // errs() << *tmp << " is derived from the input %0\n";
          fdiv_detected = tmp;
          flag = 1;
          break;
        }
      }
    }
  }

  if (Func.getName() == TargetFunc)
    errs() << flag << '\n';

  // Safety check
  if (!fdiv_detected) {
    // errs() << "No fdiv found\n";
    return PreservedAnalyses::all();
  }

  for (auto &BB : Func) {
    Instruction *term = BB.getTerminator();
    if (ReturnInst *retInst = dyn_cast<ReturnInst>(term)) {
        // Ensure fdiv_detected is valid and dominates here
        IRBuilder<> builder(retInst);
        
        // If it's not an instruction, it's already safe to use
        if (isa<Instruction>(fdiv_detected)) {
            Instruction *fdInst = cast<Instruction>(fdiv_detected);
            if (fdInst->getParent() != &BB ||
                !fdInst->comesBefore(retInst)) {
                // insert a move/copy if needed
                // errs() << "Error: fdiv_detected doesn't dominate return\n";
                return PreservedAnalyses::none();
            }
        }
        
        builder.CreateRet(fdiv_detected);
        retInst->eraseFromParent();
        break;
    }
  }

  return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getGetDivisorRangePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "get-div-range", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "get-div-range") {
                    FPM.addPass(GetDivisorRange(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getGetDivisorRangePluginInfo();
}