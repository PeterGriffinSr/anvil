#pragma once
#include <ir/types/type.hpp>

namespace anvil::ir {
class PointerType final : public Type {
public:
  explicit PointerType(Type *elem) : Type(Kind::Pointer), element_(elem) {}

  Type *getElementType() const noexcept { return element_; }

  void print(std::ostream &os) const override {
    element_->print(os);
    os << "*";
  }

private:
  Type *element_;
};
} // namespace anvil::ir
