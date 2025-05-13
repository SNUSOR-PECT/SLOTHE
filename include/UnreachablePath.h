//==============================================================================
// FILE:
//    UnreachablePath.h
//
// DESCRIPTION:
//    Declares the UnreachablePath Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_UnreachablePath_H
#define LLVM_TUTOR_UnreachablePath_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------

struct UnreachablePath : public llvm::PassInfoMixin<UnreachablePath> {
    public:
        explicit UnreachablePath(llvm::raw_ostream &OutS) : OS(OutS) {}
        
        uint32_t getConstantVal(llvm::Value* val);
        llvm::Value* getOperandFromCondition(llvm::Value* Cond, size_t pos);
        bool isConditionHighwordofInput(llvm::Value* Cond);

        bool isSpecialBranch(llvm::BasicBlock* BB, llvm::BranchInst *brInst);
        bool isSubNormalBranch(llvm::BasicBlock* BB, llvm::BranchInst *brInst);
        bool isExactZero(llvm::BasicBlock* BB, llvm::BranchInst *brInst);

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif