#pragma once
#include <ir/types/type.hpp>

namespace anvil::ir {
class VectorType final : public Type {
public:
  VectorType(Type *elem, unsigned count)
      : Type(Kind::Vector), element_(elem), count_(count) {}

  Type *getElementType() const noexcept { return element_; }

  unsigned getNumElements() const noexcept { return count_; }

  void print(std::ostream &os) const override {
    os << "<" << count_ << " x ";
    element_->print(os);
    os << ">";
  }

private:
  Type *element_;
  unsigned count_;
};
} // namespace anvil::ir
