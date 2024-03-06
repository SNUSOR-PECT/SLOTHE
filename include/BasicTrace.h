//==============================================================================
// FILE:
//    BasicTrace.h
//
// DESCRIPTION:
//    Declares the BasicTrace Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_BASICTRACE_H
#define LLVM_TUTOR_BASICTRACE_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <vector>
#include <tuple>

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------

struct BasicTrace : public llvm::PassInfoMixin<BasicTrace> {
    public:
        explicit BasicTrace(llvm::raw_ostream &OutS) : OS(OutS) {}

        // utils
        std::string getLabel(llvm::BasicBlock* BB);
        std::vector<std::string> getPredecessors(llvm::BasicBlock* BB);
        void printPredecessors(llvm::BasicBlock* BB);
        void printConds(std::vector<std::tuple<llvm::BasicBlock*, llvm::Value*, bool>>& conds);
        void traversePath(llvm::BasicBlock* BB, std::vector<llvm::BasicBlock*>& pathBB);
        void splitPathBB(std::vector<llvm::BasicBlock*>& pathBB, std::vector<std::vector<llvm::BasicBlock*>>& _pathBB);

        // crucial functions
        void tracePathBB(llvm::BasicBlock* BB, std::vector<llvm::BasicBlock*>& stack);
        void getConds(std::vector<llvm::BasicBlock*>& pathBB, std::vector<std::tuple<llvm::BasicBlock*, llvm::Value*, bool>>& conds);
        
        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif