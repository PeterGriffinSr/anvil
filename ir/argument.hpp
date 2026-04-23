#pragma once

#include <ir/value.hpp>

namespace anvil::ir {
class Argument final : public Value {
public:
  Argument(Type *ty, std::string name = {}) : Value(ty, std::move(name)) {}

  void print(std::ostream &os) const override {
    type_->print(os);
    if (hasName())
      os << " %" << name_;
  }
};
} // namespace anvil::ir
