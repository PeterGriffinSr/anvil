#pragma once

#include <ir/constants/aggregate.hpp>
#include <ir/constants/constant.hpp>
#include <ostream>
#include <vector>

namespace anvil::ir {
class ExtractElementConstantExpr final : public Constant {
public:
  ExtractElementConstantExpr(ConstantVector *vec, Constant *idx)
      : Constant(vec->getType()), vector_(vec), index_(idx) {}

  void print(std::ostream &os) const override {
    os << "extractelement ";
    vector_->print(os);
    os << ", ";
    index_->print(os);
  }

private:
  ConstantVector *vector_;
  Constant *index_;
};

class InsertElementConstantExpr final : public Constant {
public:
  InsertElementConstantExpr(ConstantVector *vec, Constant *val, Constant *idx)
      : Constant(vec->getType()), vector_(vec), value_(val), index_(idx) {}

  void print(std::ostream &os) const override {
    os << "insertelement ";
    vector_->print(os);
    os << ", ";
    value_->print(os);
    os << ", ";
    index_->print(os);
  }

private:
  ConstantVector *vector_;
  Constant *value_;
  Constant *index_;
};

class ShuffleVectorConstantExpr final : public Constant {
public:
  ShuffleVectorConstantExpr(ConstantVector *v1, ConstantVector *v2,
                            Constant *mask)
      : Constant(v1->getType()), v1_(v1), v2_(v2), mask_(mask) {}

  void print(std::ostream &os) const override {
    os << "shufflevector ";
    v1_->print(os);
    os << ", ";
    v2_->print(os);
    os << ", ";
    mask_->print(os);
  }

private:
  ConstantVector *v1_;
  ConstantVector *v2_;
  Constant *mask_;
};

class GetElementPtrConstantExpr final : public Constant {
public:
  GetElementPtrConstantExpr(Constant *base, std::vector<Constant *> indices)
      : Constant(base->getType()), base_(base), indices_(std::move(indices)) {}

  void print(std::ostream &os) const override {
    os << "getelementptr ";
    base_->print(os);
    for (Constant *idx : indices_) {
      os << ", ";
      idx->print(os);
    }
  }

private:
  Constant *base_;
  std::vector<Constant *> indices_;
};
} // namespace anvil::ir
