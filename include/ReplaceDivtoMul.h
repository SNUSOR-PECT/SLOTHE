//==============================================================================
// FILE:
//    ReplaceDivtoMul.h
//
// DESCRIPTION:
//    Declares the ReplaceDivtoMul Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_ReplaceDivtoMul_H
#define LLVM_TUTOR_ReplaceDivtoMul_H

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

struct ReplaceDivtoMul : public llvm::PassInfoMixin<ReplaceDivtoMul> {
    public:
        explicit ReplaceDivtoMul(llvm::raw_ostream &OutS) : OS(OutS) {}
        
        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif