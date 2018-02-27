#include "flatten.h"
#include "crypto_util.h"
#include <llvm/Transforms/Scalar.h>

char trob::Flatten::ID = 0;

bool trob::Flatten::runOnFunction(Function& F) {
    if(trob::canObfuscate(flag, F, "fla")) {
        return flatten(F);
    }
    return false;
}

bool trob::Flatten::flatten(Function& F) {
    std::vector<BasicBlock*> origBB;
    BasicBlock* loopEntry;
    BasicBlock* loopEnd;
    LoadInst*   load;
    SwitchInst* switchInst;
    AllocaInst* switchVar;

    // SCRAMBLER
    char scrambling_key[16];
    trob::cryptoutils->get_bytes(scrambling_key, 16);
    // END OF SCRAMBLER

    // Lower switch
    FunctionPass* lower = createLowerSwitchPass();
    lower->runOnFunction(F);

    // Save all original BB
    for(Function::iterator i = F.begin(); i != F.end(); i++) {
        origBB.push_back(&*i);
        if(isa<InvokeInst>(i->getTerminator())) {
            return false;
        }
    }

    // Noting to flatten
    if(origBB.size() <= 1) {
        return false;
    }

    // Remove first BB
    origBB.erase(origBB.begin());

    // Get a pointer on the first BB
    BasicBlock* insert = &*F.begin();

    // If main begin with an if
    BranchInst* br = NULL;
    if(isa<BranchInst>(insert->getTerminator())) {
        br = cast<BranchInst>(insert->getTerminator());
    }

    if((br != NULL && br->isConditional()) || 
        insert->getTerminator()->getNumSuccessors() > 1) {
        BasicBlock::iterator i = insert->end();
        --i;
        if(insert->size() > 1) {
            --i;
        }
        origBB.insert(origBB.begin(), insert->splitBasicBlock(i, "first"));
    }

    // Remove jump
    insert->getTerminator()->eraseFromParent();

    // Create switch variable and set as it
    switchVar = new AllocaInst(Type::getInt32Ty(F.getContext()), 
                                0, "switchVar", insert);
    new StoreInst(ConstantInt::get(Type::getInt32Ty(F.getContext()),
                                trob::cryptoutils->scramble32(0, scrambling_key)), 
                    switchVar, insert);
    
    // Create main loop
    loopEntry = BasicBlock::Create(F.getContext(), "loopEntry", &F, insert);
    loopEnd   = BasicBlock::Create(F.getContext(), "loopEnd", &F, insert);

    load = new LoadInst(switchVar, "switchVar", loopEntry);

    // Move first BB on top
    insert->moveBefore(loopEntry);
    BranchInst::Create(loopEntry, insert);

    // loopEnd jump to loopEntry
    BranchInst::Create(loopEntry, loopEnd);

    BasicBlock* swDefault = 
        BasicBlock::Create(F.getContext(), "switchDefault", &F, loopEnd);
    BranchInst::Create(loopEnd, swDefault);

    // Create switch instruction itself and set condition
    switchInst = SwitchInst::Create(&*F.begin(), swDefault, 0, loopEntry);
    switchInst->setCondition(load);

    // Remove branch jump form 1st BB and make a jump to while
    F.begin()->getTerminator()->eraseFromParent();

    BranchInst::Create(loopEntry, &*F.begin());

    // Put all BB in the switch
    for(std::vector<BasicBlock*>::iterator bb = origBB.begin(); bb != origBB.end(); bb++) {
        ConstantInt* numCase = NULL;

        // Move the BB inside the switch (only visual, no code logic)
        (*bb)->moveBefore(loopEnd);

        // Add case to switch
        numCase = cast<ConstantInt>(ConstantInt::get(switchInst->getCondition()->getType(),
            trob::cryptoutils->scramble32(switchInst->getNumCases(), scrambling_key)));
        
        switchInst->addCase(numCase, *bb);
    }

    // Recalculate switchVar
    for(std::vector<BasicBlock*>::iterator bb = origBB.begin(); bb != origBB.end(); bb++) {
        BasicBlock* i = *bb;
        ConstantInt* numCase = NULL;

        // Ret BB
        if(i->getTerminator()->getNumSuccessors() == 0) {
            continue;
        }

        // If it's a non-conditional jump
        if(i->getTerminator()->getNumSuccessors() == 1) {
            // Get successor and delete terminator
            BasicBlock* succ = i->getTerminator()->getSuccessor(0);
            i->getTerminator()->eraseFromParent();

            // Get next case
            numCase = switchInst->findCaseDest(succ);

            // If next case == default case (switchDefault)
            if(numCase == NULL) {
                numCase = cast<ConstantInt>(ConstantInt::get(
                    switchInst->getCondition()->getType(),
                    trob::cryptoutils->scramble32(switchInst->getNumCases() - 1,
                    scrambling_key)));
            }

            // Update switchVar and jump to the end of loop
            new StoreInst(numCase, load->getPointerOperand(), i);
            BranchInst::Create(loopEnd, i);
            continue;
        }

        // If it's a conditional jump
        if(i->getTerminator()->getNumSuccessors() == 2) {
            // Get next cases
            ConstantInt* numCaseTrue = 
                switchInst->findCaseDest(i->getTerminator()->getSuccessor(0));
            ConstantInt* numCaseFalse = 
                switchInst->findCaseDest(i->getTerminator()->getSuccessor(1));
            
            // Check if next case == default case (switchDefault)
            if (numCaseTrue == NULL) {
                numCaseTrue = cast<ConstantInt>(
                    ConstantInt::get(switchInst->getCondition()->getType(),
                    trob::cryptoutils->scramble32(
                        switchInst->getNumCases() - 1, scrambling_key)));
            }

            if (numCaseFalse == NULL) {
                numCaseFalse = cast<ConstantInt>(
                    ConstantInt::get(switchInst->getCondition()->getType(),
                    trob::cryptoutils->scramble32(
                        switchInst->getNumCases() - 1, scrambling_key)));
            }

            // Create a SelectInst
            BranchInst* br = cast<BranchInst>(i->getTerminator());
            SelectInst* sel = 
                SelectInst::Create(br->getCondition(), numCaseTrue, numCaseFalse, "",
                     i->getTerminator());

            // Earse terminator
            i->getTerminator()->eraseFromParent();

            // Update switchVar and jump to the end of loop
            new StoreInst(sel, load->getPointerOperand(), i);
            BranchInst::Create(loopEnd, i);
            continue;
        }
    }
    fixStack(F);
    return true;
}

