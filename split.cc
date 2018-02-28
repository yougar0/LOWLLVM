#include "split.h"

char trob::SplitBasicBlock::ID = 0;
static cl::opt<int> SplitNum("split_num", cl::init(2), 
                                cl::desc("Split <split_num> time each BB"));

bool trob::SplitBasicBlock::runOnFunction(Function& F) {
    // Check if the number of applications is correct
    if(!((SplitNum > 1) && (SplitNum <= 10))) {
        errs() << "Split application basic block percentage -split_num=x must be 1 < x <= 10";
        return false;
    }

    // Do us obfucate
    if(canObfuscate(flag, F, "split")) {
        return split(F);
    }

    return false;
}

bool trob::SplitBasicBlock::split(Function& F) {
    std::vector<BasicBlock*> origBB;
    int SplitN = SplitNum;
    bool Splited = false;
    // Save all basic blocks
    for(Function::iterator I = F.begin(), IE = F.end(); I != IE; I++) {
        origBB.push_back(&*I);
    }

    for(std::vector<BasicBlock*>::iterator BB = origBB.begin(),
        BBE = origBB.end(); BB != BBE; BB++) {
        // No needs to split a 1 inst bb or ones containing a PHI node
        if((*BB)->size() < 2 || containsPHI(**BB)) {
            continue;
        }

        // Check splitN and current BB size
        if((size_t)SplitN > (*BB)->size()) {
            SplitN = (*BB)->size() - 1;
        }

        // Generate splits point
        std::vector<int> SplitPoint;
        for(unsigned int i = 1; i < (*BB)->size(); i++) {
            SplitPoint.push_back(i);
        }

        // Shuffle
        if(SplitPoint.size() != 1) {
            shuffle(SplitPoint);
            std::sort(SplitPoint.begin(), SplitPoint.begin() + SplitN);
        }

        // Split
        BasicBlock::iterator bbIt = (*BB)->begin();
        BasicBlock* toSplit = (*BB);
        int last = 0;
        for(int i = 0; i < SplitN; i++) {
            for(int j = 0; j < SplitPoint[i] - last; j++) {
                bbIt++;
            }
            last = SplitPoint[i];
            if(toSplit->size() < 2) {
                continue;
            }
            toSplit = toSplit->splitBasicBlock(bbIt, toSplit->getName() + ".split");
            Splited = true;
        }
    }

    return Splited;
}

bool trob::SplitBasicBlock::containsPHI(BasicBlock& bb) {
    for (BasicBlock::iterator I = bb.begin(), IE = bb.end(); I != IE; ++I) {
        if (isa<PHINode>(I)) {
            return true;
        }
    }
    return false;
}

void trob::SplitBasicBlock::shuffle(std::vector<int>& vec) {
    int n = vec.size();
    for (int i = n - 1; i > 0; --i) {
        std::swap(vec[i], vec[cryptoutils->get_uint32_t() % (i + 1)]);
    }
}