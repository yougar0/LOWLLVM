#include "util.h"
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <llvm/IR/Module.h>

// Shamefully borrowed from ../Scalar/RegToMem.cpp :(
bool valueEscapes(Instruction* inst) {
    BasicBlock* bb = inst->getParent();
    Instruction* i;
    for(Value::use_iterator ui = inst->use_begin(), e = inst->use_end();
        ui != e; ui++) {
        i = cast<Instruction>(*ui);
        if(i->getParent() != bb || isa<PHINode>(i)) {
            return true;
        }
    }

    return false;
}


void trob::fixStack(Function& f) {
    // Try to remove phi node and demote reg to stack
    std::vector<PHINode*> phiVector;
    std::vector<Instruction*> regVector;
    BasicBlock* bbEntry = &*f.begin();

    do {
        phiVector.clear();
        regVector.clear();

        for(Function::iterator i = f.begin(); i != f.end(); i++) {
            for(BasicBlock::iterator j = i->begin(); j != i->end(); j++) {
                if(isa<PHINode>(j)) {
                    PHINode* phi = cast<PHINode>(j);
                    phiVector.push_back(phi);
                    continue;
                }
                if(!(isa<AllocaInst>(j) && j->getParent() == bbEntry) && 
                    (j->isUsedOutsideOfBlock(&*i) || valueEscapes(&*j))) {
                    regVector.push_back(&*j);
                    continue;
                }
            }
        }

        for(unsigned int i = 0; i != regVector.size(); i++) {
            DemoteRegToStack(*regVector.at(i), f.begin()->getTerminator());
        }

        for(unsigned int i = 0; i != phiVector.size(); i++) {
            DemotePHIToStack(phiVector.at(i), f.begin()->getTerminator());
        }
    } while(regVector.size() != 0 || phiVector.size() != 0);
}

std::string trob::readAnnotate(Function& function) {
    std::string annotation = "";
    // get annotation variable
    GlobalVariable* globalVariable = function.getParent()->
        getGlobalVariable("llvm.global.annotations");
    if(globalVariable != NULL) {
        // Get the array
        if(ConstantArray* constantArray = dyn_cast<ConstantArray>
            (globalVariable->getInitializer())) {
            for(unsigned int i = 0; i < constantArray->getNumOperands(); i++) {
                // Get the struct
                if(ConstantStruct* annotationStruct = 
                    dyn_cast<ConstantStruct>(constantArray->getOperand(i))) {
                   if(ConstantExpr* expr = 
                        dyn_cast<ConstantExpr>(annotationStruct->getOperand(0))) {
                        // If it's a bitcast we can check if the annotation is concerning
                        // the current function.
                        if(expr->getOpcode() == Instruction::BitCast && 
                            expr->getOperand(0) == &function) {
                            ConstantExpr* note = 
                                cast<ConstantExpr>(annotationStruct->getOperand(1));
                            // If it's a GetElementPtr, that means we found the variable
                            // containing the annotations
                            if(note->getOpcode() == Instruction::GetElementPtr) {
                                if(GlobalVariable* annotationStr = 
                                    dyn_cast<GlobalVariable>(note->getOperand(0))) {
                                    if(ConstantDataSequential* data = 
                                        dyn_cast<ConstantDataSequential>(annotationStr->getInitializer())) {
                                        if(data->isString()) {
                                            annotation += data->getAsString().lower() + " ";
                                        }
                                    }
                                }
                            }
                        }
                    }     
                }
            }
        }
    }

    return annotation;
}

bool trob::canObfuscate(bool flag, Function& f, std::string attribute) {
    std::string attrNo = "no" + attribute;

    // check if declaration
    if(f.isDeclaration()) {
        return false;
    }

    // check external linkage
    if(f.hasAvailableExternallyLinkage()) {
        return false;
    }

    // we have to check nofla flag first
    // Because .find("fla") is true for a string like "fla" or "nofla"
    if(trob::readAnnotate(f).find(attrNo) != std::string::npos) {
        return false;
    }

    // if fla annotations
    if(trob::readAnnotate(f).find(attribute) != std::string::npos) {
        return true;
    }

    // if fla flag is set
    if(flag) {
        return true;
    }
    return false;
}