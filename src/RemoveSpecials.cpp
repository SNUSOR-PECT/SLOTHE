#include "RemoveSpecials.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
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
// Util functions
//------------------------------------------------------------------------------
uint32_t RemoveSpecials::getConstantVal(Value* val) {
  ConstantInt *CI = dyn_cast<ConstantInt>(val);
  return CI->getSExtValue();
}

llvm::Value* RemoveSpecials::getOperandFromCondition(Value* Cond, size_t pos) {
  ICmpInst *ICmp = dyn_cast<ICmpInst>(Cond);
  return ICmp->getOperand(pos);
}

bool RemoveSpecials::isConditionHighwordofInput(Value* Cond) {
  std::stack<llvm::Instruction*> insts;

  Value* op = getOperandFromCondition(Cond, 0);
  insts.push(dyn_cast<Instruction>(Cond)); // push branch condition

  // trace the instructions consist of target operands
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

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

bool RemoveSpecials::isSpecialBranch(llvm::BasicBlock* BB, llvm::BranchInst *brInst) {
  Value* Cond = brInst->getCondition();

  // First, check if the intermediate is specials(INF/NaN)
  uint32_t opI = getConstantVal(getOperandFromCondition(Cond, 1));
  uint32_t opT = 0x7ff00000 - 1; // since the condition holds 'equal'
  if (!(opI == opT))
    return false;

  // Then check if the other operand is high word of the input 
  bool isCondHighWord = isConditionHighwordofInput(Cond);

  return isCondHighWord;
}

bool RemoveSpecials::isSubNormalBranch(llvm::BasicBlock* BB, llvm::BranchInst *brInst) {
  Value* Cond = brInst->getCondition();

  // First, check if the intermediate is subNormal
  uint32_t opI = getConstantVal(getOperandFromCondition(Cond, 1));
  uint32_t opT1 = 0x3c800000; // subnormal value in ieee-754 representation. 2**-55
  uint32_t opT2 = 0x3c900000; // subnormal value in ieee-754 representation. 2**-54

  if (!((opI == opT1) || (opI == opT2)))
    return false;

  // Then check if the other operand is high word of the input 
  bool isCondHighWord = isConditionHighwordofInput(Cond);

  return isCondHighWord;
}

bool RemoveSpecials::isExactZero(llvm::BasicBlock* BB, llvm::BranchInst *brInst) {
  // the first op : C <- or A B
  // the second op : eq C 0
  // then this block means that if x==+-0 -> remove
  Value* Cond = brInst->getCondition();

  // First, check if the intermediate is 0
  uint32_t opI = getConstantVal(getOperandFromCondition(Cond, 1));
  if (opI == 0)
    errs() << "Const is zero!\n";
  else
    return false;
}

PreservedAnalyses RemoveSpecials::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
  bool specialFound = false;
  bool subnormalFound = false;
  bool exactZeroFound = false;
  
  for (auto &BB : Func) {
    auto term = BB.getTerminator(); // check BB which contains conditional branch
    // errs() << *term << "\n";
    BranchInst *brInst = dyn_cast<BranchInst>(term);

    if ( brInst && (brInst->isConditional())) {
      if (!specialFound) {
        bool isSpecials = isSpecialBranch(&BB, brInst); // check the branch's semantic
          if (isSpecials) {
              // remove branch by setting the same successor on both True/False branch
              // To remove other successors, apply simplify-cfg pass would be good choice :) 
              errs() << "[*] BB    - isSpecials = True detected!\n";
              brInst->setSuccessor(0, brInst->getSuccessor(1));
              specialFound = true;
          }
      }
      if (!subnormalFound) {
        bool isSubNormal = isSubNormalBranch(&BB, brInst); // check the branch's semantic
        if (isSubNormal) {
          errs() << "[*] BB    - isSubNormal = True detected!\n\n";
          brInst->setSuccessor(0, brInst->getSuccessor(1));
          subnormalFound = true;
        }
      }
      if (!exactZeroFound) {
        bool isSubNormal = isExactZero(&BB, brInst); // check the branch's semantic
        if (isSubNormal) {
          errs() << "[*] BB    - isExactZero = True detected!\n\n";
          // brInst->setSuccessor(0, brInst->getSuccessor(1));
          exactZeroFound = true;
        }
      }
    }

    // We currently assume that only one BB holds INF/NaN or subNormal branch among BBs.
    // Thus, since each case occurs only once throughout BBs, we break the loop
    if (specialFound & subnormalFound) break;
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