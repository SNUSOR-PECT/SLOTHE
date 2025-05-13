#include "Mergeable.h"

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

#define DEBUG_TYPE "find-mergeable"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses Mergeable::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
  
  for (auto &BB : llvm::reverse(Func)) {
    auto term = BB.getTerminator(); // check BB which contains conditional branch
    // errs() << *term << "\n";
    BranchInst *brInst = dyn_cast<BranchInst>(term);
    if ( brInst && (brInst->isConditional())) {
        errs() << "Before swap: " << *brInst << "\n";
        // Swap the successors
        BasicBlock *succ0 = brInst->getSuccessor(0);
        BasicBlock *succ1 = brInst->getSuccessor(1);

        brInst->setSuccessor(0, succ1);
        brInst->setSuccessor(1, succ0);

        // Print after swap
        errs() << "After swap: " << *brInst << "\n";

        // Currently, we only try the checking process at the last branch
        break;
    }

  }

  return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMergeablePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "find-mergeable", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "find-mergeable") {
                    FPM.addPass(Mergeable(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMergeablePluginInfo();
}