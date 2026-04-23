#pragma once

#include <cstdint>
#include <ir/constants/constant.hpp>
#include <ostream>

namespace anvil::ir {
class ConstantInt final : public Constant {
public:
  ConstantInt(Type *ty, int64_t val) : Constant(ty), value_(val) {}

  int64_t getValue() const noexcept { return value_; }

  void print(std::ostream &os) const override { os << value_; }

  void printAsOperand(std::ostream &os) const override { os << value_; }

private:
  int64_t value_;
};

class ConstantFP final : public Constant {
public:
  ConstantFP(Type *ty, double val) : Constant(ty), value_(val) {}

  double getValue() const noexcept { return value_; }

  void print(std::ostream &os) const override { os << value_; }

private:
  double value_;
};

class ConstantPointerNull final : public Constant {
public:
  explicit ConstantPointerNull(Type *ty) : Constant(ty) {}

  void print(std::ostream &os) const override { os << "null"; }
};

class ConstantTargetNone final : public Constant {
public:
  explicit ConstantTargetNone(Type *ty) : Constant(ty) {}

  void print(std::ostream &os) const override { os << "none"; }
};
} // namespace anvil::ir
