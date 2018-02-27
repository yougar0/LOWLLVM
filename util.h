#ifndef TROB_UTIL_H
#define TROB_UTIL_H
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Transforms/Utils/Local.h> // For DemoteRegToStack And DemotePHIToStack

using namespace llvm;
namespace trob {
    void fixStack(Function&);
    std::string readAnnotate(Function&);
    bool canObfuscate(bool, Function&, std::string);
}
#endif