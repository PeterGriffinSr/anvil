#pragma once
#include <ir/types/type.hpp>
#include <vector>

namespace anvil::ir {
class StructType final : public Type {
public:
  explicit StructType(std::vector<Type *> elems)
      : Type(Kind::Struct), elements_(std::move(elems)) {}

  const std::vector<Type *> &getElements() const noexcept { return elements_; }

  void print(std::ostream &os) const override {
    os << "{ ";
    for (size_t i = 0; i < elements_.size(); ++i) {
      if (i)
        os << ", ";
      elements_[i]->print(os);
    }
    os << " }";
  }

private:
  std::vector<Type *> elements_;
};
} // namespace anvil::ir
