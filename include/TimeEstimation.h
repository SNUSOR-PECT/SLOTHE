//==============================================================================
// FILE:
//    TimeEstimation.h
//
// DESCRIPTION:
//    Declares the TimeEstimation Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_TimeEstimation_H
#define LLVM_TUTOR_TimeEstimation_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <vector>
#include <tuple>
#include <cstdlib>
#include <fstream>
#include <unordered_set>

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------

struct TimeEstimation : public llvm::PassInfoMixin<TimeEstimation> {
    public:
        explicit TimeEstimation(llvm::raw_ostream &OutS) : OS(OutS) {}

        void traceFunction(llvm::Function *F, std::set<std::string> &visited, llvm::Value* input, std::vector<std::pair<std::string, int>>& opLvl);
        
        llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif