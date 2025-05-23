#include "CostEstimation.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

#include <tuple>
#include <vector>
#include <stack>

using namespace llvm;

#define DEBUG_TYPE "estimate-cost"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses CostEstimation::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
  
    int cntCond = 0, cntSubfunc = 0;
    for (auto &BB : Func) {
        auto term = BB.getTerminator(); 
        BranchInst *brInst = dyn_cast<BranchInst>(term);
        if ( brInst && (brInst->isConditional())) {
            cntCond++;
        }

        for (auto &I : BB) {
          if (auto *call = dyn_cast<CallInst>(&I)) {
              Function *calledFunc = call->getCalledFunction();
              if (calledFunc ) {
                    cntSubfunc++;
              }
          }
        }
    }

    int cost = cntCond*100 + cntSubfunc*30;

    errs() << cost << "\n";


  return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getCostEstimationPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "estimate-cost", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "estimate-cost") {
                    FPM.addPass(CostEstimation(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getCostEstimationPluginInfo();
}