#include "flatten.h"

static llvm::RegisterPass<trob::Flatten> FlattenPass("flatten", "call control flow flatten");