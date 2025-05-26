#include "OpAnalyzer.h"

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
#include <set>

using namespace llvm;

#define DEBUG_TYPE "analyze-op"

//------------------------------------------------------------------------------
// Util functions
//------------------------------------------------------------------------------
using opcode_t = unsigned;
void initInst(std::set<opcode_t>& inst) {
  inst = {
    Instruction::Add,  Instruction::FAdd,
    Instruction::Sub,  Instruction::FSub,
    Instruction::Mul,  Instruction::FMul,
    Instruction::UDiv, Instruction::SDiv, Instruction::FDiv,
    // Instruction::URem, Instruction::SRem, Instruction::FRem
    Instruction::Ret
  };
}

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

PreservedAnalyses OpAnalyzer::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {

  std::set<opcode_t> inst;
  initInst(inst);
  
  for (auto &BB : Func) {
    for (auto &I : BB) {
      if (auto *call = llvm::dyn_cast<llvm::CallBase>(&I)) {
        const llvm::Function *calledFunc = call->getCalledFunction();
        if (calledFunc) {
          continue; // computable
        }
      }
      // if I is incomputable, return -1
      if (inst.find(I.getOpcode()) == inst.end()) {
        llvm::errs() << -1 << "\n";
        return llvm::PreservedAnalyses::all();
      }
    }
  }

  for (auto &BB : Func) {
    for (auto &I : BB) {
      if (auto *call = llvm::dyn_cast<llvm::CallBase>(&I)) {
        const llvm::Function *calledFunc = call->getCalledFunction();
        if (calledFunc) {
            llvm::StringRef funcName = calledFunc->getName();
            llvm::errs() << funcName << " ";
        }
      }
    }
  }

  return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getOpAnalyzerPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "analyze-op", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "analyze-op") {
                    FPM.addPass(OpAnalyzer(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getOpAnalyzerPluginInfo();
}