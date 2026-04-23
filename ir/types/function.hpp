#pragma once
#include <ir/types/type.hpp>
#include <vector>

namespace anvil::ir {
class FunctionType final : public Type {
public:
  FunctionType(Type *ret, std::vector<Type *> params)
      : Type(Kind::Function), return_(ret), params_(std::move(params)) {}

  Type *getReturnType() const noexcept { return return_; }

  const std::vector<Type *> &getParamTypes() const noexcept { return params_; }

  void print(std::ostream &os) const override {
    return_->print(os);
    os << "(";
    for (size_t i = 0; i < params_.size(); ++i) {
      if (i)
        os << ", ";
      params_[i]->print(os);
    }
    os << ")";
  }

private:
  Type *return_;
  std::vector<Type *> params_;
};
} // namespace anvil::ir
