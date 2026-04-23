#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include <ir/argument.hpp>
#include <ir/basic_block.hpp>
#include <ir/types/function.hpp>
#include <ir/value.hpp>

namespace anvil::ir {
class Function final : public Value {
public:
  Function(FunctionType *type, std::string name)
      : Value(type, std::move(name)), funcType_(type) {
    for (Type *paramTy : funcType_->getParamTypes())
      args_.emplace_back(std::make_unique<Argument>(paramTy));
  }

  FunctionType *getFunctionType() const noexcept { return funcType_; }

  const std::vector<std::unique_ptr<Argument>> &getArguments() const noexcept {
    return args_;
  }

  Argument *getArgument(unsigned idx) const { return args_.at(idx).get(); }

  void printAsOperand(std::ostream &os) const override { os << "@" << name_; }

  void addBlock(std::unique_ptr<BasicBlock> bb) {
    blocks_.push_back(std::move(bb));
  }

  const std::vector<std::unique_ptr<BasicBlock>> &getBlocks() const noexcept {
    return blocks_;
  }

  void print(std::ostream &os) const override {
    os << "define ";
    funcType_->getReturnType()->print(os);
    os << " @" << name_ << "(";

    for (size_t i = 0; i < args_.size(); ++i) {
      if (i)
        os << ", ";
      args_[i]->print(os);
    }

    os << ") {\n";

    assignIds();

    for (const auto &bb : blocks_)
      bb->print(os);

    os << "}\n";
  }

  unsigned getNextId() { return nextId_++; }

private:
  void assignIds() const {
    unsigned id = 0;
    for (const auto &bb : blocks_) {
      for (const auto &inst : bb->getInstructions()) {
        if (inst->getType() != nullptr &&
            inst->getType()->getKind() != Type::Kind::Void)
          inst->setId(id++);
      }
      // Terminators never produce a value - no ID needed
    }
  }

  mutable unsigned nextId_ = 0;
  FunctionType *funcType_;
  std::vector<std::unique_ptr<Argument>> args_;
  std::vector<std::unique_ptr<BasicBlock>> blocks_;
};
} // namespace anvil::ir
