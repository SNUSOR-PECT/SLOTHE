#include "RemoveSpecials.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <tuple>
#include <vector>
#include <stack>

using namespace llvm;

#define DEBUG_TYPE "remove-specials"

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

llvm::Value* RemoveSpecials::getOperandFromCondition(Value* Cond) {
    ICmpInst *ICmp = dyn_cast<ICmpInst>(Cond);
    return ICmp->getOperand(0);
}

bool RemoveSpecials::isSpecialBranch(llvm::BasicBlock* BB) {
    std::stack<llvm::Instruction*> insts;

    auto term = BB->getTerminator();
    BranchInst *brInst = dyn_cast<BranchInst>(term);
    Value* cond = brInst->getCondition();
    Value* op = getOperandFromCondition(cond);

    insts.push(dyn_cast<Instruction>(cond)); // push branch condition

    // trace the instructions consists of target operands
    // this may not work -- need to be modified precisely
    while(1) {
        if (Instruction *I = dyn_cast<Instruction>(op)) {
            insts.push(I);
            op = I->getOperand(0);
            continue;
        }
        break;
    }

    // detect {bitcast - lshr - trunc} sequence
    std::vector<bool> isValidSequence(3, false);
    while(!insts.empty()) {
        Instruction* I = insts.top();
        insts.pop(); // remove top instruction
        // errs() << *I << "\n";

        if (isValidSequence[0] == false && I->getOpcode()==Instruction::BitCast) {
            isValidSequence[0] = true;
        }
        if (isValidSequence[1] == false && I->getOpcode()==Instruction::LShr) {
            isValidSequence[1] = true;
        }
        if (isValidSequence[2] == false && I->getOpcode()==Instruction::Trunc) {
            isValidSequence[2] = true;
        }
    }

    return isValidSequence[0] & isValidSequence[1] & isValidSequence[2];
}

PreservedAnalyses RemoveSpecials::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
    // errs() << "\n- Start of Function [" << Func.getName() << "]\n";

    for (auto &BB : Func) {
      // We currently assume that only one BB holds INF/NaN branch among BBs.
      // Thus, since `isMergeable=true` occurs only once throughout BBs, we break the loop
      auto term = BB.getTerminator(); // check BB which contains conditional branch
      if (BranchInst *brInst = dyn_cast<BranchInst>(term)) {
        if (brInst->isConditional()) {
            bool isMergeable = isSpecialBranch(&BB); // check the branch's semantic
            if (isMergeable) {
                // remove branch by setting the same successor on both True/False branch
                // To remove other successors, apply simplify-cfg pass would be good choice :) 
                errs() << "[*] BB    - isMergeable = True detected!\n\n";
                brInst->setSuccessor(0, brInst->getSuccessor(1));
                break;
            }
        }
      }
    }

    return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getRemoveSpecialsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "remove-specials", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "remove-specials") {
                    FPM.addPass(RemoveSpecials(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getRemoveSpecialsPluginInfo();
}