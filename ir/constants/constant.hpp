#pragma once

#include <ir/value.hpp>

namespace anvil::ir {
class Constant : public Value {
public:
  using Value::Value;
  ~Constant() override = default;
};
} // namespace anvil::ir
