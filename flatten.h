#ifndef TROB_FLATTEN_H
#define TROB_FLATTEN_H
#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Instructions.h>
#include "util.h"

using namespace llvm;
namespace trob {
    class Flatten : public FunctionPass {
        public:
            static char ID;
            Flatten() : FunctionPass(ID), flag(true) {}
            Flatten(bool flag) : FunctionPass(ID), flag(flag) {}
            bool runOnFunction(Function&);
            bool flatten(Function&);
        private:
            bool flag;
    };
}
#endif