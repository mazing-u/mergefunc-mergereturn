#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <numeric>

using namespace llvm;

namespace {
struct FunctionComparator {
  const Function *F1, *F2;

  mutable DenseMap<const Value *, unsigned> SN1, SN2;
  mutable unsigned brojac1 = 0;   
  mutable unsigned brojac2 = 0;   

  FunctionComparator(const Function *A, const Function *B) : F1(A), F2(B) {
    for (const Argument &A1 : F1->args()) SN1[&A1] = brojac1++;
    for (const Argument &A2 : F2->args()) SN2[&A2] = brojac2++;
  }

  unsigned serial1(const Value *V) const {
    auto [It, Inserted] = SN1.try_emplace(V, brojac1);
    if (Inserted) ++brojac1;
    return It->second;
  }

  unsigned serial2(const Value *V) const {
    auto [It, Inserted] = SN2.try_emplace(V, brojac2);
    if (Inserted) ++brojac2;
    return It->second;
  }

  bool valuesEqual(const Value *V1, const Value *V2) const {
    if (isa<Constant>(V1) || isa<Constant>(V2))
      return V1 == V2;
    return serial1(V1) == serial2(V2);
  }

  bool instructionsEqual(const Instruction *I1, const Instruction *I2) const {
    if (I1->getOpcode() != I2->getOpcode()) 
        return false;
    if (I1->getNumOperands() != I2->getNumOperands()) 
; RUN: opt -passes=mergefunc -S < %s | FileCheck %s
        return false;
    if (I1->getType() != I2->getType()) 
        return false;

    for (unsigned i = 0, e = I1->getNumOperands(); i != e; ++i)
      if (!valuesEqual(I1->getOperand(i), I2->getOperand(i)))
        return false;

    if (const auto *C1 = dyn_cast<CallInst>(I1))
      if (C1->getCallingConv() != cast<CallInst>(I2)->getCallingConv())
        return false;

    if (const auto *B1 = dyn_cast<BranchInst>(I1))
      if (B1->isConditional() != cast<BranchInst>(I2)->isConditional())
        return false;

    if (const auto *Cmp1 = dyn_cast<CmpInst>(I1))
      if (Cmp1->getPredicate() != cast<CmpInst>(I2)->getPredicate())
        return false;

    if (const auto *GEP1 = dyn_cast<GetElementPtrInst>(I1))
      if (GEP1->getSourceElementType() != cast<GetElementPtrInst>(I2)->getSourceElementType())
        return false;

    if (const auto *LI1 = dyn_cast<LoadInst>(I1))
      if (LI1->isVolatile() != cast<LoadInst>(I2)->isVolatile() ||
          LI1->getAlign() != cast<LoadInst>(I2)->getAlign())
        return false;

    if (const auto *SI1 = dyn_cast<StoreInst>(I1))
      if (SI1->isVolatile() != cast<StoreInst>(I2)->isVolatile() ||
          SI1->getAlign() != cast<StoreInst>(I2)->getAlign())
        return false;

    return true;
  }

  bool equal() const {
    if (F1->getFunctionType() != F2->getFunctionType()) return false;
    if (F1->getCallingConv()  != F2->getCallingConv())  return false;
    if (F1->size()            != F2->size())            return false;

    auto BB1 = F1->begin(), BB2 = F2->begin();
    for (auto E1 = F1->end(); BB1 != E1; ++BB1, ++BB2) {
      if (BB1->size() != BB2->size()) return false;

      auto II1 = BB1->begin(), II2 = BB2->begin();
      for (auto IE1 = BB1->end(); II1 != IE1; ++II1, ++II2) {
        serial1(&*II1); 
        serial2(&*II2);
        if (!instructionsEqual(&*II1, &*II2)) return false;
      }
    }
    return true;
  }
};

uint64_t functionHash(const Function &F) {
  uint64_t H = F.arg_size() ^ ((uint64_t)F.size() << 32);
  uint64_t Pos = 0;
  for (const BasicBlock &BB : F)
    for (const Instruction &I : BB)
      H ^= ((uint64_t)I.getOpcode() + Pos++) * 2654435761ULL;
  return H;
}

void replaceWithThunk(Function *Old, Function *Canonical) {
  Old->deleteBody();

  BasicBlock *BB = BasicBlock::Create(Old->getContext(), "entry", Old);
  IRBuilder<> Builder(BB);

  SmallVector<Value *, 8> Args;
  for (Argument &A : Old->args())
    Args.push_back(&A);

  CallInst *CI =
      Builder.CreateCall(Canonical->getFunctionType(), Canonical, Args);
  CI->setCallingConv(Canonical->getCallingConv());
  CI->setTailCallKind(CallInst::TCK_MustTail);

  if (Old->getReturnType()->isVoidTy())
    Builder.CreateRetVoid();
  else
    Builder.CreateRet(CI);

  Old->setLinkage(GlobalValue::PrivateLinkage);
}

static bool isThunk(const Function &F) {
  if (F.size() != 1)
    return false;
  const BasicBlock &BB = F.getEntryBlock();
  if (BB.size() != 2)
    return false;
  const auto *CI = dyn_cast<CallInst>(&BB.front());
  return CI && CI->isMustTailCall();
}

static bool runOnce(Module &M) {
  bool Changed = false;

  DenseMap<uint64_t, SmallVector<Function *, 4>> Buckets;
  for (Function &F : M) {
    if (F.isDeclaration() || F.isIntrinsic() || F.empty())
      continue;
    if (F.isVarArg())
      continue;
    if (isThunk(F))
      continue; 
    
    Buckets[functionHash(F)].push_back(&F);
  }

  for (auto &KV : Buckets) {
    SmallVectorImpl<Function *> &Funcs = KV.second;
    if (Funcs.size() < 2)
      continue;

    SmallVector<unsigned, 8> Canonical(Funcs.size());
    std::iota(Canonical.begin(), Canonical.end(), 0u);

    for (unsigned i = 0; i < Funcs.size(); ++i) {
      for (unsigned j = i + 1; j < Funcs.size(); ++j) {
        if (Canonical[j] != j)
          continue; 

        FunctionComparator Cmp(Funcs[Canonical[i]], Funcs[j]);
        if (Cmp.equal())
          Canonical[j] = Canonical[i];
      }
    }

    for (unsigned j = 0; j < Funcs.size(); ++j) {
      if (Canonical[j] == j)
        continue; 

      Function *Old = Funcs[j];
      Function *Canon = Funcs[Canonical[j]];

      replaceWithThunk(Old, Canon);
      Changed = true;
    }
  }

  return Changed;
}

struct MojMergefuncPass : public ModulePass {
  static char ID;

  MojMergefuncPass() : ModulePass(ID) {}

  StringRef getPassName() const override {
    return "Moj mergefunc pass";
  }

  bool runOnModule(Module &M) override {
    
    bool EverChanged = false;
    bool ChangedThisRound;
    
    do {
      ChangedThisRound = runOnce(M);
      EverChanged |= ChangedThisRound;
    } while (ChangedThisRound);

    return EverChanged; 
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
  }
};

}
char MojMergefuncPass::ID = 0;
static RegisterPass<MojMergefuncPass> X("moj-mergefunc-pass",
                                        "Moj mergefunc pass (PROJEKAT).",
                                        false, false);
