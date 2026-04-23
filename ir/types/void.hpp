#pragma once
#include <ir/types/type.hpp>

namespace anvil::ir {
class VoidType final : public Type {
public:
  VoidType() : Type(Kind::Void) {}

  void print(std::ostream &os) const override { os << "void"; }
};
} // namespace anvil::ir