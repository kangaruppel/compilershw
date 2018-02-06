// 15-745 S18 Assignment 1: FunctionInfo.cpp
// Group: Milijana Surbatovich (milijans) and Emily Ruppel (eruppel)
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"

#include <string.h>
#include <iostream>
#include <map>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
  class FunctionInfo : public FunctionPass {
  public:
    static char ID;
    FunctionInfo() : FunctionPass(ID) { }
    ~FunctionInfo() { }

    // We don't modify the program, so we preserve all analyses
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }

    // Do some initialization
    bool doInitialization(Module &M) override {
      errs() << "15745 Function Information Pass\n"; // TODO: remove this.
      // outs() << "Name,\tArgs,\tCalls,\tBlocks,\tInsns\n";

      return false;
    }
  // Struct to store counters for all functions
  typedef struct funcData_ {
    int bbs;
    int insts;
    int numArgs;
    int callSites;
    StringRef name;
  } funcData;

  // Map to function name for simplicity
	std::map<std::string, funcData> funcCounter;
  virtual bool runOnFunction(Function &F) {
    int bbs, insts, numArgs;
    StringRef name;
    char stuff[5] = "call";
    char temp[5];
    bbs = 0;
    insts = 0;
    // Initialize counters for new function
    name = F.getName();
    if(funcCounter.find(name) == funcCounter.end()) {
      funcCounter[name].callSites = 0;
    }
    funcCounter[name].bbs = 0;
    funcCounter[name].insts = 0;
    funcCounter[name].numArgs = 0;
    if(F.isVarArg()) {
      numArgs = -1;
    }
    else {
      numArgs = F.arg_size();
    }
    // Iterate over basic blocks
    for (auto &B : F) {
      bbs++;
      // Iterate over instructions
      for (auto &I : B) {
        insts++;
        // Verbose way to figure out if we're dealing with a call instruction...
        strncpy(temp, I.getOpcodeName(), 5);
        if(strcmp(stuff, temp) == 0) {
          if(funcCounter.find(cast<CallInst>(I).getCalledFunction()->getName()) == funcCounter.end()) {
            funcCounter[cast<CallInst>(I).getCalledFunction()->getName()].callSites = 1;
          }
          else {
            funcCounter[cast<CallInst>(I).getCalledFunction()->getName()].callSites += 1;
          }
        }
      }
    }
    funcCounter[name].bbs = bbs;
    funcCounter[name].insts = insts;
    funcCounter[name].numArgs = numArgs;
  return true;
  }

  // Print final results after analyzing all functions
  bool doFinalization(Module &M) override{
    outs() << "Name\t" << "#Args\t" << "#Calls\t" << "#Blocks\t" << "#Insns\n";
    for( std::map<std::string,funcData>::iterator it=funcCounter.begin(); it!=funcCounter.end(); ++it) {
      outs() << it->first << ",\t";
      if(it->second.numArgs == -1) {
        outs() << "*";
      }
      else {
        outs() << it->second.numArgs;
      }
      outs() << "\t";
      outs() << it->second.callSites << ",\t" << it->second.bbs << ",\t" << it->second.insts << ",\n";
    }
    return true;
  }
};
}

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("function-info", "15745: Function Information", false, false);
