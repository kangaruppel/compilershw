// 15-745 S18 Assignment 1: LocalOpts.cpp
// Group: Milijana Surbatovich (milijans) & Emily Ruppel (eruppel)
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include <iostream>
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;
using namespace std;

void algebraic_identities(Function& F)
{
  for (BasicBlock& BB : F) {
    for (Instruction& inst : BB) {
      //check for 0 + or * other var
      //or 1 * other var
      if (inst.isBinaryOp()) {
	//IN Progress
      }
    }
  }
}

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

   
    bool runOnFunction(Function &F) override {
   
      vector<Instruction*> to_delete;
      map<Value*, int> is_const;

      for (BasicBlock& BB : F) {
	//this is a bb level opt
	//vector<Value*> consts;
	//constant folding run
	for (Instruction& inst : BB) {
	  if (StoreInst* si = dyn_cast<StoreInst>(&inst)) {
	    if(ConstantInt* ci = dyn_cast<ConstantInt>(si->getValueOperand())) {
	      is_const[si->getPointerOperand()] = ci->getSExtValue();
	      for(User* use : si->getPointerOperand()->users()) {
		is_const[use] = ci->getSExtValue();
	      }
	      outs() << "inserting into map " << *ci<< " at " << *si->getPointerOperand()<<"\n";
	    } else if (is_const.find(si->getValueOperand())!=is_const.end()) {
	      APInt val = APInt(32, is_const[si->getValueOperand()], true);
	      ConstantInt* ci = ConstantInt::get(F.getParent()->getContext(), val);
		
	      StoreInst* store = new StoreInst(ci, si->getPointerOperand(), &inst);
	      
	      for(User* use : si->getPointerOperand()->users()) {
		is_const[use] = ci->getSExtValue();
		outs() << "inserting into map " << *ci<< " at " << *use<<"\n";
	      }
	      //si->setValueOperand(ci);
	      ReplaceInstWithInst(si, store);
	      si->getPointerOperand()->replaceAllUsesWith(ci);
	      //to_delete.push_back(&inst);
	    }
	  }
	  if (inst.isBinaryOp()) {
	    //outs() << "found binop " << inst <<"\n";
	    Value* operand1 = inst.getOperand(0);
	    Value* operand2 = inst.getOperand(1);
	    //outs() <<"ops: " << *operand1 <<" " << *operand2 <<"\n";
	    if ((isa<ConstantInt>(operand1)||(is_const.find(operand1)!=is_const.end()))
		&&(isa<ConstantInt>(operand2)||(is_const.find(operand2)!=is_const.end()))) {
	      outs() << "found constant binop"<<inst <<"\n";
		int new_val;
		int op1, op2;
		if(isa<ConstantInt>(operand1)) {
		  op1 = cast<ConstantInt>(operand1)->getSExtValue();
		} else {
		  op1 = is_const[operand1];
		}
		if(isa<ConstantInt>(operand2)) {
		  op2 = cast<ConstantInt>(operand2)->getSExtValue();
		} else {
		  op2 = is_const[operand2];
		} outs() << "using " << op1 <<" and " << op2<<"\n";
		switch(inst.getOpcode()) {
		case Instruction::Add :
		  new_val = op1 + op2;
		  break;
		case Instruction::Sub :
		  new_val = op1 - op2;
		  break;
		case Instruction::Mul :
		  new_val = op1 * op2;
		  break;
		case Instruction::SDiv :
		  new_val = op1 / op2;
		  break;
		default:
		  outs() << "reached default opcode:" << inst.getOpcode() << "\n";
		}//end switch
		//put the new val in our map
		ConstantInt* ci = ConstantInt::get(F.getParent()->getContext(), APInt(32, new_val, true));
		is_const[cast<Value>(&inst)] = ci->getSExtValue();
		for(User* use : inst.users()) {
		  outs() << "inserting into map " << *ci<< " at " << *use<<"\n";
		  is_const[use] = ci->getSExtValue();
		}
		//StoreInst* si = new StoreInst(cast<Value>(ci), cast<Value>(&inst), &inst);
		inst.replaceAllUsesWith(ci);
		to_delete.push_back(&inst);
		outs() << "pushing delete\n";
		
	    }//end const if
	  }//end binop if
	}//end inst iterator
	//now we can delete the redundant instructions
	for (Instruction* inst : to_delete) {
	  outs() << "erasing " << *inst << "\n";
	  inst->eraseFromParent();
	  }
      }//end BB iterator
      
      //TODO: Identities
      //TODO: Strength reduction
      return true;
    }
  };
}

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("local-opts", "Simple local opts pass", true, true);
