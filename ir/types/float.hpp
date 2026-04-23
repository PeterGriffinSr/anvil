#pragma once
#include <ir/types/type.hpp>

namespace anvil::ir {
enum class FloatKind { Half, Float, Double };

class FloatType final : public Type {
public:
  explicit FloatType(FloatKind kind) : Type(Kind::Float), kind_(kind) {}

  FloatKind getFloatKind() const noexcept { return kind_; }

  void print(std::ostream &os) const override {
    switch (kind_) {
    case FloatKind::Half:
      os << "half";
      break;
    case FloatKind::Float:
      os << "float";
      break;
    case FloatKind::Double:
      os << "double";
      break;
    }
  }

private:
  FloatKind kind_;
};
} // namespace anvil::ir
