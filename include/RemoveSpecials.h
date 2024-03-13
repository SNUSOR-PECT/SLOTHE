//==============================================================================
// FILE:
//    RemoveSpecials.h
//
// DESCRIPTION:
//    Declares the RemoveSpecials Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_REMOVESPECIALS_H
#define LLVM_TUTOR_REMOVESPECIALS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------

struct RemoveSpecials : public llvm::PassInfoMixin<RemoveSpecials> {
    public:
        explicit RemoveSpecials(llvm::raw_ostream &OutS) : OS(OutS) {}
        
        llvm::Value* getOperandFromCondition(llvm::Value* Cond);
        bool isSpecialBranch(llvm::BasicBlock* BB);

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif