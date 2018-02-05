// 15-745 S18 Assignment 1: FunctionInfo.cpp
// Group:
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

  typedef struct funcData_ {
    int bbs;
    int insts;
    int numArgs;
    int callSites;
    StringRef name;
  } funcData;

  // Print output for each function
	std::map<std::string, funcData> funcCounter;
  virtual bool runOnFunction(Function &F) {
    int bbs, insts, numArgs;
    StringRef name;
    char stuff[5] = "call";
    char temp[5];
    bbs = 0;
    insts = 0;
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
    for (auto &B : F) {
      bbs++;
      for (auto &I : B) {
        insts++;
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
	  //outs() << name << ",\t" << funcCounter[name].bbs << ",\t" << funcCounter[name].insts << ",\t" << funcCounter[name].numArgs << "\n";
  }

  bool doFinalization(Module &M) override{
    outs() << "Test!\n";
    for( std::map<std::string,funcData>::iterator it=funcCounter.begin(); it!=funcCounter.end(); ++it) {
      outs() << it->first << ",\t" << it->second.bbs << ",\t" << it->second.insts
             << ",\t" << it->second.callSites << ",\t";
      if(it->second.numArgs == -1) {
        outs() << "*";
      }
      else {
        outs() << it->second.numArgs;
      }
      outs() << "\n";
    }
  }
};
}

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("function-info", "15745: Function Information", false, false);
