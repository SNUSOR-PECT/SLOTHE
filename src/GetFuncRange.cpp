#include "GetFuncRange.h"

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

#define DEBUG_TYPE "get-func-range"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
static cl::opt<std::string> TargetFunc(
    "target-func",
    cl::desc("Target function name"),
    cl::value_desc("function name"),
    cl::init("")
);

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses GetFuncRange::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &)  {
    bool Changed = false;
    Value *arg = nullptr;

    // 1. detect function call
    for (auto &BB : Func) {
        for (auto &I : BB) {
          if (auto *call = dyn_cast<CallInst>(&I)) {
              Function *calledFunc = call->getCalledFunction();
              if (calledFunc && calledFunc->getName() == TargetFunc) {
                  arg = call->getArgOperand(0);
                  break; // iterator is invalidated, safe break
              }
          }
        }
    }

    // Safety check
    if (!arg) {
      // errs() << "No " << TargetFunc << " found\n";
      return PreservedAnalyses::all();
    }

    for (auto &BB : Func) {
      Instruction *term = BB.getTerminator();
        if (ReturnInst *retInst = dyn_cast<ReturnInst>(term)) {
            IRBuilder<> builder(retInst);
            builder.CreateRet(arg);
            retInst->eraseFromParent();
            Changed = true;
        }
    }

    return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getGetFuncRangePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "get-func-range", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "get-func-range") {
                    FPM.addPass(GetFuncRange(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getGetFuncRangePluginInfo();
}