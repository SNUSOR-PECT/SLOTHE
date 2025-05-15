#include "Merge.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

#include <tuple>
#include <vector>
#include <stack>

using namespace llvm;

#define DEBUG_TYPE "merge-path"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
static cl::opt<bool> MergeDirection(
    "merge-direction",
    cl::desc("Direction to merge"),
    cl::init(false)
);

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses Merge::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
  
  for (auto &BB : llvm::reverse(Func)) {
    auto term = BB.getTerminator(); // check BB which contains conditional branch
    // errs() << *term << "\n";
    BranchInst *brInst = dyn_cast<BranchInst>(term);
    if ( brInst && (brInst->isConditional())) {
        // Merge to one path
        if (MergeDirection) { // True
            brInst->setSuccessor(0, brInst->getSuccessor(1));
        } else { // False
            brInst->setSuccessor(1, brInst->getSuccessor(0));
        }
        // brInst->setSuccessor(1, brInst->getSuccessor(0));
        break;
    }
  }

  return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMergePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "merge-path", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "merge-path") {
                    FPM.addPass(Merge(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMergePluginInfo();
}