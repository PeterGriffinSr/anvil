#pragma once

#include <cassert>
#include <ostream>
#include <vector>

#include <ir/value.hpp>

namespace anvil::ir {
class BasicBlock;

class Instruction : public Value {
public:
  enum class Opcode {
    Ret,
    Br,
    Switch,
    IndirectBr,
    Invoke,
    Resume,
    Unreachable,
    FNeg,
    Add,
    FAdd,
    Sub,
    FSub,
    Mul,
    FMul,
    UDiv,
    SDiv,
    FDiv,
    URem,
    SRem,
    FRem,
    Shl,
    LShr,
    AShr,
    And,
    Or,
    Xor,
    Alloca,
    Load,
    Store,
    GetElementPtr,
    Trunc,
    ZExt,
    SExt,
    FPTrunc,
    FPExt,
    FPToUI,
    FPToSI,
    UIToFP,
    SIToFP,
    PtrToInt,
    IntToPtr,
    BitCast,
    ICmp,
    FCmp,
    PHI,
    Select,
    Call,
    Tail,
  };

  enum class ICmpPredicate { EQ, NE, UGT, UGE, ULT, ULE, SGT, SGE, SLT, SLE };

  Instruction(Type *type, Opcode opcode, std::vector<Value *> operands = {},
              BasicBlock *parent = nullptr)
      : Value(type), opcode_(opcode), operands_(std::move(operands)),
        parent_(parent) {}

  void printTerminator(std::ostream &os) const;
  void printOtherOps(std::ostream &os) const;

  Opcode getOpcode() const noexcept { return opcode_; }
  BasicBlock *getParent() const noexcept { return parent_; }
  void setParent(BasicBlock *bb) noexcept { parent_ = bb; }

  void setId(unsigned id) noexcept { id_ = id; }
  unsigned getId() const noexcept { return id_; }

  size_t getNumOperands() const noexcept { return operands_.size(); }

  Value *getOperand(size_t i) const {
    assert(i < operands_.size());
    return operands_[i];
  }

  const std::vector<Value *> &getOperands() const noexcept { return operands_; }

  void addIncoming(Value *value, BasicBlock *block) {
    assert(opcode_ == Opcode::PHI && "Not a PHI instruction");
    incoming_.emplace_back(value, block);
  }

  const std::vector<std::pair<Value *, BasicBlock *>> &
  getIncoming() const noexcept {
    return incoming_;
  }

  void setICmpPredicate(ICmpPredicate pred) noexcept { icmpPred_ = pred; }

  ICmpPredicate getICmpPredicate() const noexcept { return icmpPred_; }

  bool isTerminator() const noexcept {
    switch (opcode_) {
    case Opcode::Ret:
    case Opcode::Br:
    case Opcode::Switch:
    case Opcode::IndirectBr:
    case Opcode::Invoke:
    case Opcode::Resume:
    case Opcode::Unreachable:
      return true;
    default:
      return false;
    }
  }

  std::string icmpPredStr() const {
    switch (icmpPred_) {
    case ICmpPredicate::EQ:
      return "eq";
    case ICmpPredicate::NE:
      return "ne";
    case ICmpPredicate::UGT:
      return "ugt";
    case ICmpPredicate::UGE:
      return "uge";
    case ICmpPredicate::ULT:
      return "ult";
    case ICmpPredicate::ULE:
      return "ule";
    case ICmpPredicate::SGT:
      return "sgt";
    case ICmpPredicate::SGE:
      return "sge";
    case ICmpPredicate::SLT:
      return "slt";
    case ICmpPredicate::SLE:
      return "sle";
    default:
      return "unknown";
    }
  }

  void print(std::ostream &os) const override {
    bool producesValue = !isTerminator() && type_ != nullptr &&
                         type_->getKind() != Type::Kind::Void;

    if (producesValue)
      os << "%" << id_ << " = ";

    if (isTerminator())
      printTerminator(os);
    else
      printOtherOps(os);
  }

  void printAsOperand(std::ostream &os) const override { os << "%" << id_; }

private:
  std::string opcodeStr() const {
    switch (opcode_) {
#define OP(x, s)                                                               \
  case Opcode::x:                                                              \
    return s
      OP(Ret, "ret");
      OP(Br, "br");
      OP(Switch, "switch");
      OP(IndirectBr, "indirectbr");
      OP(Invoke, "invoke");
      OP(Resume, "resume");
      OP(Unreachable, "unreachable");
      OP(FNeg, "fneg");
      OP(Add, "add");
      OP(FAdd, "fadd");
      OP(Sub, "sub");
      OP(FSub, "fsub");
      OP(Mul, "mul");
      OP(FMul, "fmul");
      OP(UDiv, "udiv");
      OP(SDiv, "sdiv");
      OP(FDiv, "fdiv");
      OP(URem, "urem");
      OP(SRem, "srem");
      OP(FRem, "frem");
      OP(Shl, "shl");
      OP(LShr, "lshr");
      OP(AShr, "ashr");
      OP(And, "and");
      OP(Or, "or");
      OP(Xor, "xor");
      OP(Alloca, "alloca");
      OP(Load, "load");
      OP(Store, "store");
      OP(GetElementPtr, "getelementptr");
      OP(Trunc, "trunc");
      OP(ZExt, "zext");
      OP(SExt, "sext");
      OP(FPTrunc, "fptrunc");
      OP(FPExt, "fpext");
      OP(FPToUI, "fptoui");
      OP(FPToSI, "fptosi");
      OP(UIToFP, "uitofp");
      OP(SIToFP, "sitofp");
      OP(PtrToInt, "ptrtoint");
      OP(IntToPtr, "inttoptr");
      OP(BitCast, "bitcast");
      OP(ICmp, "icmp");
      OP(FCmp, "fcmp");
      OP(PHI, "phi");
      OP(Select, "select");
      OP(Call, "call");
      OP(Tail, "tail");
#undef OP
    }
    return "unknown";
  }

  Opcode opcode_;
  std::vector<Value *> operands_;
  std::vector<std::pair<Value *, BasicBlock *>> incoming_;
  ICmpPredicate icmpPred_{};

  BasicBlock *parent_ = nullptr;
  unsigned id_ = 0;
};
} // namespace anvil::ir
