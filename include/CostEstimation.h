//==============================================================================
// FILE:
//    CostEstimation.h
//
// DESCRIPTION:
//    Declares the CostEstimation Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_CostEstimation_H
#define LLVM_TUTOR_CostEstimation_H

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

struct CostEstimation : public llvm::PassInfoMixin<CostEstimation> {
    public:
        explicit CostEstimation(llvm::raw_ostream &OutS) : OS(OutS) {}
        
        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif