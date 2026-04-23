#pragma once
#include <ir/types/type.hpp>
#include <ostream>

namespace anvil::ir {
class LabelType final : public Type {
public:
  LabelType() : Type(Kind::Label) {}
  void print(std::ostream &os) const override { os << "label"; }
};
} // namespace anvil::ir