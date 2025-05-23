#include "ReplaceFunc.h"

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

#define DEBUG_TYPE "replace-func"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
static cl::opt<std::string> TargetFunc(
    "target-func",
    cl::desc("Target function name"),
    cl::value_desc("function name"),
    cl::init("")
);

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

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses ReplaceFunc::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &)  {
    bool Changed = false;

    Module *M = Func.getParent();
    Function *fFunc = M->getFunction("f");
    if (!fFunc)
        return PreservedAnalyses::all(); // Skip if `f` is not linked

    // detect function call
    for (auto &BB : Func) {
        for (auto &I : BB) {
          if (auto *call = dyn_cast<CallInst>(&I)) {
              Function *calledFunc = call->getCalledFunction();
              if (calledFunc && calledFunc->getName() == TargetFunc) {
                  IRBuilder<> Builder(call);
                  Value *arg = call->getArgOperand(0);

                  auto *newCall = Builder.CreateCall(fFunc, {arg});
                  call->replaceAllUsesWith(newCall);
                  call->eraseFromParent();
                  Changed = true;
                //   errs() << TargetFunc << " is replaced\n";
                  break; // iterator is invalidated, safe break
              }
          }
        }
    }

    return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getReplaceFuncPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "replace-func", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "replace-func") {
                    FPM.addPass(ReplaceFunc(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getReplaceFuncPluginInfo();
}