#include "CountCond.h"

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

#define DEBUG_TYPE "count-conditions"

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

PreservedAnalyses CountCond::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
  if (Func.getName() == TargetFunc) {
    int cnt = 0;
    for (auto &BB : Func) {
        auto term = BB.getTerminator(); 
        // check BB which contains conditional branch
        BranchInst *brInst = dyn_cast<BranchInst>(term);
        if ( brInst && (brInst->isConditional())) {
            cnt++;
        }
    }

    errs() << cnt << "\n";

    return llvm::PreservedAnalyses::all();
  }

  return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getCountCondPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "count-conditions", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "count-conditions") {
                    FPM.addPass(CountCond(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getCountCondPluginInfo();
}