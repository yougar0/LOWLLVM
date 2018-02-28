#include "flatten.h"
#include "split.h"

static llvm::RegisterPass<trob::Flatten> FlattenPass("flatten", "call control flow flatten");
static llvm::RegisterPass<trob::SplitBasicBlock> SplitBasicBlockPass("split_bb", "split basic block");