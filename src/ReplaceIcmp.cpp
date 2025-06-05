#include "ReplaceIcmp.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

#define DEBUG_TYPE "replace-icmp"

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
void replaceICmp(ICmpInst *ICmp) {
  using Pred = llvm::ICmpInst::Predicate;

  Module *M = ICmp->getModule();
  IRBuilder<> builder(ICmp);

  // Get icmp operands
  Value *lhs = ICmp->getOperand(0);
  Value *rhs = ICmp->getOperand(1);

  // Declare or get the custom _icmp function: i1 _icmp(double, double)
  // (You can change this to double if your function returns float type)
  FunctionType *funcTy = FunctionType::get(builder.getInt1Ty(),
                                           {lhs->getType(), rhs->getType()},
                                           false);
  FunctionCallee customICmp = M->getOrInsertFunction("_icmp", funcTy);

  // Replace the icmp with a call to _icmp(lhs, rhs)
  Value *call;
  Pred pred = ICmp->getPredicate();
  if (ICmpInst::isGT(pred) || ICmpInst::isGE(pred))
    call = builder.CreateCall(customICmp, {lhs, rhs});
  else
    call = builder.CreateCall(customICmp, {rhs, lhs});

  // Replace all uses and erase the original instruction
  ICmp->replaceAllUsesWith(call);
  ICmp->eraseFromParent();
}

PreservedAnalyses ReplaceIcmp::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &)  {
    bool Changed = false;

    Module *M = Func.getParent();
    Function *fFunc = M->getFunction(TargetFunc); // f
    if (!fFunc) {
      return PreservedAnalyses::all();
    } 

    for (auto &BB : Func) {
    for (auto it = BB.begin(), end = BB.end(); it != end; ) {
        Instruction *I = &*it++;
        if (auto *icmp = dyn_cast<ICmpInst>(I)) {
            errs() << "ICMP\n";
            replaceICmp(icmp);
        }
    }
    }


    return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getReplaceIcmpPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "replace-icmp", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "replace-icmp") {
                    FPM.addPass(ReplaceIcmp(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getReplaceIcmpPluginInfo();
}