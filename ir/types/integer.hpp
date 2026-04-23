#pragma once

#include <ir/types/type.hpp>

namespace anvil::ir {
class IntegerType final : public Type {
public:
  explicit IntegerType(unsigned bits) : Type(Kind::Integer), bits_(bits) {}

  unsigned getBitWidth() const noexcept { return bits_; }

  void print(std::ostream &os) const override { os << "i" << bits_; }

private:
  unsigned bits_;
};
} // namespace anvil::ir
