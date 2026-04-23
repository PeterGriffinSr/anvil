#pragma once

#include <ir/basic_block.hpp>
#include <ir/context.hpp>
#include <ir/instruction.hpp>
#include <memory>
#include <vector>

namespace anvil::ir {

class IRBuilder {
public:
  IRBuilder(Context *ctx, BasicBlock *bb) : ctx_(ctx), block_(bb) {}

  Instruction *CreateBr(BasicBlock *dest) {
    return createTerminator(nullptr, Instruction::Opcode::Br, {dest});
  }

  Instruction *CreateCondBr(Value *cond, BasicBlock *thenBB,
                            BasicBlock *elseBB) {
    return createTerminator(nullptr, Instruction::Opcode::Br,
                            {cond, thenBB, elseBB});
  }

  Instruction *CreateRet(Value *val) {
    return createTerminator(val->getType(), Instruction::Opcode::Ret, {val});
  }

  Instruction *CreateRetVoid() {
    return createTerminator(nullptr, Instruction::Opcode::Ret, {});
  }

  Instruction *CreateAdd(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::Add, lhs, rhs);
  }

  Instruction *CreateSub(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::Sub, lhs, rhs);
  }

  Instruction *CreateMul(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::Mul, lhs, rhs);
  }

  Instruction *CreateUDiv(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::UDiv, lhs, rhs);
  }

  Instruction *CreateSDiv(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::SDiv, lhs, rhs);
  }

  Instruction *CreateURem(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::URem, lhs, rhs);
  }

  Instruction *CreateSRem(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::SRem, lhs, rhs);
  }

  Instruction *CreateShl(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::Shl, lhs, rhs);
  }

  Instruction *CreateLShr(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::LShr, lhs, rhs);
  }

  Instruction *CreateAShr(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::AShr, lhs, rhs);
  }

  Instruction *CreateAnd(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::And, lhs, rhs);
  }

  Instruction *CreateOr(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::Or, lhs, rhs);
  }

  Instruction *CreateXor(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::Xor, lhs, rhs);
  }

  Instruction *CreateFNeg(Value *v) {
    return createInst(v->getType(), Instruction::Opcode::FNeg, {v});
  }

  Instruction *CreateFAdd(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::FAdd, lhs, rhs);
  }

  Instruction *CreateFSub(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::FSub, lhs, rhs);
  }

  Instruction *CreateFMul(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::FMul, lhs, rhs);
  }

  Instruction *CreateFDiv(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::FDiv, lhs, rhs);
  }

  Instruction *CreateFRem(Value *lhs, Value *rhs) {
    return binOp(Instruction::Opcode::FRem, lhs, rhs);
  }

  Instruction *CreateAlloca(Type *ty) {
    return createInst(ctx_->getPointerTy(ty), Instruction::Opcode::Alloca);
  }

  Instruction *CreateLoad(Type *ty, Value *ptr) {
    return createInst(ty, Instruction::Opcode::Load, {ptr});
  }

  Instruction *CreateStore(Value *val, Value *ptr) {
    return createInst(nullptr, Instruction::Opcode::Store, {val, ptr});
  }

  Instruction *CreateGEP(Value *base, const std::vector<Value *> &indices) {
    auto *ptrTy = dynamic_cast<PointerType *>(base->getType());
    if (!ptrTy)
      throw std::runtime_error("GEP base must be pointer");

    std::vector<Value *> ops{base};
    ops.insert(ops.end(), indices.begin(), indices.end());

    return createInst(ctx_->getPointerTy(ptrTy->getElementType()),
                      Instruction::Opcode::GetElementPtr, ops);
  }

  Instruction *CreateTrunc(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::Trunc, v, ty);
  }

  Instruction *CreateZExt(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::ZExt, v, ty);
  }

  Instruction *CreateSExt(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::SExt, v, ty);
  }

  Instruction *CreateFPTrunc(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::FPTrunc, v, ty);
  }

  Instruction *CreateFPExt(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::FPExt, v, ty);
  }

  Instruction *CreateFPToUI(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::FPToUI, v, ty);
  }

  Instruction *CreateFPToSI(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::FPToSI, v, ty);
  }

  Instruction *CreateUIToFP(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::UIToFP, v, ty);
  }

  Instruction *CreateSIToFP(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::SIToFP, v, ty);
  }

  Instruction *CreatePtrToInt(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::PtrToInt, v, ty);
  }

  Instruction *CreateIntToPtr(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::IntToPtr, v, ty);
  }

  Instruction *CreateBitCast(Value *v, Type *ty) {
    return castOp(Instruction::Opcode::BitCast, v, ty);
  }

  Instruction *CreateUnreachable() {
    return createTerminator(nullptr, Instruction::Opcode::Unreachable, {});
  }

  Instruction *
  CreateSwitch(Value *cond, BasicBlock *defaultBB,
               const std::vector<std::pair<Value *, BasicBlock *>> &cases) {
    std::vector<Value *> ops{cond, defaultBB};
    for (auto &[val, bb] : cases) {
      ops.push_back(val);
      ops.push_back(bb);
    }
    return createTerminator(nullptr, Instruction::Opcode::Switch, ops);
  }

  Instruction *CreateIndirectBr(Value *addr,
                                const std::vector<BasicBlock *> &targets) {
    std::vector<Value *> ops{addr};
    for (auto *bb : targets)
      ops.push_back(bb);
    return createTerminator(nullptr, Instruction::Opcode::IndirectBr, ops);
  }

  Instruction *CreateInvoke(Value *callee, const std::vector<Value *> &args,
                            BasicBlock *normalDest, BasicBlock *unwindDest,
                            Type *retTy) {
    std::vector<Value *> ops{callee};
    ops.insert(ops.end(), args.begin(), args.end());
    ops.push_back(normalDest);
    ops.push_back(unwindDest);
    return createTerminator(retTy, Instruction::Opcode::Invoke, ops);
  }

  Instruction *CreateResume(Value *exception) {
    return createTerminator(nullptr, Instruction::Opcode::Resume, {exception});
  }

  Instruction *CreateICmp(Instruction::ICmpPredicate pred, Value *lhs,
                          Value *rhs) {
    auto inst =
        createInst(ctx_->getInt1Ty(), Instruction::Opcode::ICmp, {lhs, rhs});
    inst->setICmpPredicate(pred);
    return inst;
  }

  Instruction *CreateFCmp(Value *lhs, Value *rhs,
                          Instruction::ICmpPredicate pred) {
    auto inst =
        createInst(ctx_->getInt1Ty(), Instruction::Opcode::FCmp, {lhs, rhs});
    inst->setICmpPredicate(pred);
    return inst;
  }

  Instruction *CreateSelect(Value *cond, Value *trueVal, Value *falseVal) {
    return createInst(trueVal->getType(), Instruction::Opcode::Select,
                      {cond, trueVal, falseVal});
  }

  Instruction *CreateCall(Type *retTy, Value *callee,
                          const std::vector<Value *> &args) {
    std::vector<Value *> ops{callee};
    ops.insert(ops.end(), args.begin(), args.end());
    return createInst(retTy, Instruction::Opcode::Call, ops);
  }

  Instruction *CreateTailCall(Type *retTy, Value *callee,
                              const std::vector<Value *> &args) {
    std::vector<Value *> ops{callee};
    ops.insert(ops.end(), args.begin(), args.end());
    return createInst(retTy, Instruction::Opcode::Tail, ops);
  }

  Instruction *CreatePHI(Type *type) {
    return createInst(type, Instruction::Opcode::PHI);
  }

  void AddIncoming(Instruction *phi, Value *val, BasicBlock *bb) {
    phi->addIncoming(val, bb);
  }

  void SetInsertPoint(BasicBlock *bb) { block_ = bb; }

private:
  Instruction *createInst(Type *type, Instruction::Opcode op,
                          const std::vector<Value *> &operands = {}) {
    auto inst = std::make_unique<Instruction>(type, op, operands);
    Instruction *ptr = inst.get();
    block_->addInstruction(std::move(inst));
    return ptr;
  }

  Instruction *createTerminator(Type *type, Instruction::Opcode op,
                                const std::vector<Value *> &operands = {}) {
    auto inst = std::make_unique<Instruction>(type, op, operands);
    Instruction *ptr = inst.get();
    block_->setTerminator(std::move(inst));
    return ptr;
  }

  Instruction *binOp(Instruction::Opcode op, Value *lhs, Value *rhs) {
    return createInst(lhs->getType(), op, {lhs, rhs});
  }

  Instruction *castOp(Instruction::Opcode op, Value *v, Type *ty) {
    return createInst(ty, op, {v});
  }

  Context *ctx_;
  BasicBlock *block_;
};
} // namespace anvil::ir
