#include "BasicTrace.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>
#include <tuple>

using namespace llvm;

#define DEBUG_TYPE "basic-trace"

//------------------------------------------------------------------------------
// util functions
//------------------------------------------------------------------------------

std::string BasicTrace::getLabel(BasicBlock* BB) {
  std::string str;
  raw_string_ostream myOS(str);
  BB->printAsOperand(myOS, false);
  return myOS.str();
}

std::vector<std::string> BasicTrace::getPredecessors(BasicBlock* BB) {
  std::vector<std::string> Preds;
  for (BasicBlock *Pred : predecessors(BB)) {
    Preds.push_back(getLabel(Pred));
  }
  return Preds;
}

void BasicTrace::printPredecessors(BasicBlock* BB) {
  std::vector<std::string> Preds = getPredecessors(BB);
  for (std::string str : Preds)
    errs() << str << ", ";
  errs() << "\n";
}

void BasicTrace::printConds(std::vector<std::tuple<BasicBlock*, Value*, bool>>& conds) {
  for (auto cond : conds) {
    errs() << "        BB: " << getLabel(get<0>(cond)) << ", condition: " << *get<1>(cond) << ", ifTrue : " << get<2>(cond) << "\n";
  }
}

void BasicTrace::traversePath(BasicBlock* BB, std::vector<BasicBlock*>& pathBB) {
  errs() << "    - BasicBlock [" << getLabel(BB) << "]: ";
  for (auto bb : pathBB) {
    errs() << getLabel(bb) << " ";
  }
  errs() << "\n";
}

void BasicTrace::splitPathBB(std::vector<BasicBlock*>& pathBB, std::vector<std::vector<BasicBlock*>>& _pathBB) {
  std::vector<size_t> indices;
  BasicBlock* entry = pathBB[0]; // entry block (different paths)
  size_t sz = pathBB.size();

  for (size_t i=0; i<sz; i++) {
    if (pathBB[i] == entry)
      indices.push_back(i);
  }
  indices.push_back(pathBB.size()); // last index
  _pathBB.resize(indices.size()-1);

  for (size_t i=0; i<indices.size()-1; i++) {
    size_t begin = indices[i];
    size_t end = indices[i+1];
    std::vector<BasicBlock*> tmp(pathBB.begin() + begin, pathBB.begin() + end);
    _pathBB[i] = tmp;
  }
}

void BasicTrace::tracePathBB(BasicBlock* BB, std::vector<BasicBlock*>& pathBB) {
  for (BasicBlock *Pred : predecessors(BB)) {
    tracePathBB(Pred, pathBB);
    // errs() << getLabel(Pred) << " ";
    pathBB.push_back(Pred);
  }
}

//------------------------------------------------------------------------------
// Crucial functions
//------------------------------------------------------------------------------

void BasicTrace::getConds(std::vector<BasicBlock*>& pathBB, std::vector<std::tuple<BasicBlock*, Value*, bool>>& conds) {
  std::tuple<BasicBlock*, Value*, bool> condBB;

  for (size_t i=0; i<pathBB.size()-1; i++) {
    BasicBlock *BB = pathBB[i];
    auto term = BB->getTerminator();

    if (BranchInst *brInst = dyn_cast<BranchInst>(term)) {
      if (brInst->isConditional()) { // conditional branch
          Value* cond = brInst->getOperand(0);
          BasicBlock *ifTrueBB = brInst->getSuccessor(0); // Get the true branch successor
          bool ifTrue = (ifTrueBB == pathBB[i+1]); // is BB's condition true?
          condBB = std::make_tuple(BB, cond, ifTrue);
          conds.push_back(condBB);
        }
    }
  }
}

PreservedAnalyses BasicTrace::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
    errs() << "- Start of Function [" << Func.getName() << "]\n";

    for (auto &BB : Func) {
      // errs() << "    - BasicBlock [" << getLabel(&BB) << "]: ";
      if (BB.isEntryBlock()) {
        errs() << "    - BasicBlock [" << getLabel(&BB) << "]\n";
        continue;
      }

      std::vector<BasicBlock*> pathBB; // multiple paths can exist
      std::vector<std::tuple<BasicBlock*, Value*, bool>> conds; // BasicBlock, condition (llvm::Value), path(true/false)
      tracePathBB(&BB, pathBB);

      // check cases : multiple path
      size_t cnt = std::count(pathBB.begin(), pathBB.end(), pathBB[0]);
      if (cnt == 1) {
        pathBB.push_back(&BB);
        traversePath(&BB, pathBB);
        getConds(pathBB, conds);
        printConds(conds);
      }
      else {
        // split a vector into individual cases
        std::vector<std::vector<BasicBlock*>> _pathBB;
        splitPathBB(pathBB, _pathBB);
        for (auto bb : _pathBB) {
          bb.push_back(&BB);
          traversePath(&BB, bb);
          getConds(bb, conds);
          printConds(conds);

          conds.clear();
          errs() << "        ---------------------------------------------\n";
        }
      }
      
      // for (Instruction &I : BB) {
      //   errs() << "        - Instruction : " << I << "\n";
      // }
    }

    return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getBasicTracePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "basic-trace", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "basic-trace") {
                    FPM.addPass(BasicTrace(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getBasicTracePluginInfo();
}