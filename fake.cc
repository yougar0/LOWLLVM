#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Instructions.h>

using namespace llvm;

namespace {
class FakeVM : public FunctionPass {
    public:
    static char ID;
    FakeVM() : FunctionPass(ID) {}

    bool runOnFunction(Function& F) override {
        for(Function::iterator block = F.begin(); block != F.end(); block++) {
            for(BasicBlock::iterator inst = block->begin(); inst != block->end(); inst++) {
                if(inst->isBinaryOp()) {
                    if(inst->getOpcode() == Instruction::Add) {
                        return add(cast<BinaryOperator>(inst));
                    }
                }
            }
        }

        return false;
    }

    bool add(BinaryOperator* bo) {
        BinaryOperator* op = BinaryOperator::CreateNeg(bo->getOperand(1), "", bo);
        op = BinaryOperator::Create(Instruction::Sub, bo->getOperand(0), op, "", bo);

        bo->replaceAllUsesWith(op);

        return true;
    }
};
}

char FakeVM::ID = 0;

static RegisterPass<FakeVM> fakeVM("fakevm", "fakevm", false, false);

