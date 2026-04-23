#pragma once

#include <ir/constants/constant.hpp>
#include <ir/constants/printing.hpp>
#include <vector>

namespace anvil::ir {
class ConstantArray final : public Constant {
public:
  ConstantArray(Type *ty, std::vector<Constant *> elems)
      : Constant(ty), elements_(std::move(elems)) {}

  void print(std::ostream &os) const override {
    printList(os, elements_, '[', ']');
  }

private:
  std::vector<Constant *> elements_;
};

class ConstantStruct final : public Constant {
public:
  ConstantStruct(Type *ty, std::vector<Constant *> elems)
      : Constant(ty), elements_(std::move(elems)) {}

  void print(std::ostream &os) const override {
    printList(os, elements_, '{', '}');
  }

private:
  std::vector<Constant *> elements_;
};

class ConstantVector final : public Constant {
public:
  ConstantVector(Type *ty, std::vector<Constant *> elems)
      : Constant(ty), elements_(std::move(elems)) {}

  void print(std::ostream &os) const override {
    printList(os, elements_, '<', '>');
  }

private:
  std::vector<Constant *> elements_;
};
} // namespace anvil::ir
