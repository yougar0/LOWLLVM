#ifndef TROB_SPLIT_H
#define TROB_SPLIT_H
// LLVM include
#include <llvm/Pass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Transforms/Utils/Local.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include "util.h"
#include "crypto_util.h"

using namespace llvm;

namespace trob {
    class SplitBasicBlock : public FunctionPass {
        public:
            static char ID;
            SplitBasicBlock() : FunctionPass(ID), flag(true) {}
            SplitBasicBlock(bool flag) : FunctionPass(ID), flag(flag) {}
            bool runOnFunction(Function&);
        private:
            bool flag;
            bool split(Function&);
            bool containsPHI(BasicBlock&);
            void shuffle(std::vector<int>&);
    };
}
#endif