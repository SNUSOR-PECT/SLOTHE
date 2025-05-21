#include "ReplaceDiv.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

#define DEBUG_TYPE "replace-div"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
static cl::opt<double> expectedMax(
    "div-max",
    cl::desc("Expected maximum value of divisor"),
    cl::value_desc("Divisor"),
    cl::init(1e-9)
);

static cl::opt<int> iterD(
    "iter-d",
    cl::desc("The number of iteration for inv"),
    cl::value_desc("interation"),
    cl::init(3)
);


//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses ReplaceDiv::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &)  {
    LLVMContext &Ctx = Func.getContext();
    Module *M = Func.getParent();
    IRBuilder<> Builder(Ctx);

    // Get reference to _inverse function or declare it
    FunctionCallee DivFunc = M->getOrInsertFunction(
        "_inverse", FunctionType::get(Type::getDoubleTy(Ctx), {Type::getDoubleTy(Ctx), Type::getInt32Ty(Ctx)}, false));

    double divVal = 1 / ((expectedMax / 2.0) * 1.1);

    std::vector<Instruction*> ToErase;

    for (auto &BB : Func) {
      for (auto &I : BB) {
        if (I.getOpcode() == Instruction::FDiv) {
            Value *dividend = I.getOperand(0);
            Value *divisor = I.getOperand(1);

            Builder.SetInsertPoint(&I);

            // Step 1: y / t
            Value *tConst = ConstantFP::get(Type::getDoubleTy(Ctx), divVal);
            Value *Divided = Builder.CreateFMul(divisor, tConst);

            // Step 2: _inverse(y / t)
            llvm::Value *iterD_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), iterD);
            llvm::Value *DivCall = Builder.CreateCall(DivFunc, {Divided, iterD_const});

            // Step 3: _inverse_result * t
            Value *MulT = Builder.CreateFMul(DivCall, tConst);

            // Step 4: dividend * result
            Value *NewVal = Builder.CreateFMul(dividend, MulT);

            I.replaceAllUsesWith(NewVal);
            ToErase.push_back(&I);
        }
      }
    }

    // Cleanup
    for (auto *I : ToErase)
      I->eraseFromParent();

    return PreservedAnalyses::none();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getReplaceDivPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "replace-div", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "replace-div") {
                    FPM.addPass(ReplaceDiv(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getReplaceDivPluginInfo();
}