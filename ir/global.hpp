#pragma once

#include <ir/constants/constant.hpp>
#include <ir/value.hpp>

namespace anvil::ir {
class GlobalVariable : public Value {
public:
  GlobalVariable(Type *ty, std::string name, std::unique_ptr<Constant> init,
                 bool isConstant = true)
      : Value(ty, std::move(name)), initializer_(std::move(init)),
        isConstant_(isConstant) {}

  void print(std::ostream &os) const override {
    os << "@" << name_ << " = ";
    os << (isConstant_ ? "private unnamed_addr constant " : "global ");

    if (auto *ptr = dynamic_cast<PointerType *>(type_))
      ptr->getElementType()->print(os);
    else
      type_->print(os);

    os << " ";

    if (initializer_)
      initializer_->print(os);
    else
      os << "zeroinitializer";
  }

  void printAsOperand(std::ostream &os) const override { os << "@" << name_; }

  Constant *getInitializer() const { return initializer_.get(); }
  bool isConstant() const { return isConstant_; }

private:
  std::unique_ptr<Constant> initializer_;
  bool isConstant_;
};
} // namespace anvil::ir
