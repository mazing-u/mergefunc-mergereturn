#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"

#include <unordered_map>

using namespace llvm;

namespace {
  struct MojMergeReturnPass : public FunctionPass {
    static char ID;
    std::unordered_map<BasicBlock*, Value*> BlokoviReturnMapiranje;

    MojMergeReturnPass() : FunctionPass(ID) {}

    void mapirajReturnNaredbe(Function &F) {
      if (F.getReturnType()->isVoidTy()) {
        for (BasicBlock &BB : F) {
          if (isa<ReturnInst>(BB.getTerminator())) {
            BlokoviReturnMapiranje[&BB] = nullptr;
          }
        }
      }
      else {
        for (BasicBlock &BB : F) {
          if (isa<ReturnInst>(BB.getTerminator())) {
            BlokoviReturnMapiranje[&BB] = BB.getTerminator()->getOperand(0);
          }
        }
      }
    }
    
    void preveziBlokove(Function &F) {
      BasicBlock* ZajednickiBB = BasicBlock::Create(F.getContext(), "", &F, nullptr);
      IRBuilder<> Builder(ZajednickiBB);
      std::vector<Instruction*> InstrukcijeZaBrisanje;

      PHINode* phi = nullptr;
      if (!F.getReturnType()->isVoidTy()) {
        phi = Builder.CreatePHI(F.getReturnType(), BlokoviReturnMapiranje.size(), "");
      }

      for (auto iterator : BlokoviReturnMapiranje) {
        BasicBlock* BB = iterator.first;
        Value* RetVrednost = iterator.second;
        Instruction* Return = BB->getTerminator();

        Builder.SetInsertPoint(Return);
        Builder.CreateBr(ZajednickiBB);

        if (!F.getReturnType()->isVoidTy()) {
          phi->addIncoming(RetVrednost, BB);
        }

        InstrukcijeZaBrisanje.push_back(Return);
      }

      for (Instruction* I : InstrukcijeZaBrisanje) {
        I->eraseFromParent();
      }

      Builder.SetInsertPoint(ZajednickiBB);
      
      if(phi) {
        Builder.CreateRet(phi);
      }
      else {
        Builder.CreateRetVoid();
      }
    }

    bool runOnFunction(Function &F) override {
      bool IRpromenjen = false;

      BlokoviReturnMapiranje.clear();

      mapirajReturnNaredbe(F);
      if (BlokoviReturnMapiranje.size() > 1) {
        preveziBlokove(F);
        IRpromenjen = true;
      }

      return IRpromenjen;
    }
  };
}

char MojMergeReturnPass::ID = 0;
static RegisterPass<MojMergeReturnPass> X("moj-merge-return-pass", "Moj merge return pass (PROJEKAT).");