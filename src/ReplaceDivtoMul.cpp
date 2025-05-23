#include "ReplaceDivtoMul.h"

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

#define DEBUG_TYPE "replace-div-mul"

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

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses ReplaceDivtoMul::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &)  {
    std::vector<Instruction*> ToErase;

    for (auto &BB : Func) {
        for (auto &I : BB) {
            if (I.getOpcode() == Instruction::FDiv) {
                // errs() << "FDiv detected\n";
                Value *dividend = I.getOperand(0);
                Value *divisor = I.getOperand(1);

                if (auto* c = llvm::dyn_cast<llvm::ConstantFP>(divisor)) {
                    // errs() << "divisor is a constant\n";
                    double div_value = c->getValueAPF().convertToDouble();
                    double recip = 1.0 / div_value;

                    // Create reciprocal constant
                    llvm::Value* recipConst = llvm::ConstantFP::get(divisor->getType(), recip);

                    // Insert multiplication before the division instruction
                    llvm::IRBuilder<> builder(&I);
                    llvm::Value* newMul = builder.CreateFMul(dividend, recipConst);

                    // Replace uses and mark old instruction for erasure
                    I.replaceAllUsesWith(newMul);
                    ToErase.push_back(&I);
                }
            }
        }
    }

    // Remove all collected instructions (safe!)
    for (auto* I : ToErase) {
        I->eraseFromParent();
    }

    return PreservedAnalyses::none();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getReplaceDivtoMulPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "replace-div-mul", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "replace-div-mul") {
                    FPM.addPass(ReplaceDivtoMul(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getReplaceDivtoMulPluginInfo();
}