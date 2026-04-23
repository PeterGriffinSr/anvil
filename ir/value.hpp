#pragma once

#include <ostream>
#include <string>

#include <ir/types/type.hpp>

namespace anvil::ir {
class Value {
public:
  explicit Value(Type *type, std::string name = {})
      : type_(type), name_(std::move(name)) {}

  virtual ~Value() = default;

  Type *getType() const noexcept { return type_; }

  bool hasName() const noexcept { return !name_.empty(); }

  const std::string &getName() const noexcept { return name_; }

  void setName(std::string name) { name_ = std::move(name); }

  virtual void print(std::ostream &os) const = 0;

  virtual void printAsOperand(std::ostream &os) const {
    if (hasName())
      os << "%" << name_;
    else
      print(os);
  }

protected:
  Type *type_;
  std::string name_;
};
} // namespace anvil::ir
