#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <ir/constants/scalar.hpp>
#include <ir/types/types.hpp>

namespace anvil::ir {
class Context {
public:
  Context() = default;

  VoidType *getVoidTy() {
    if (!voidTy_)
      voidTy_ = make<VoidType>();
    return voidTy_;
  }

  IntegerType *getIntTy(unsigned bits) {
    for (auto &t : types_) {
      if (auto *i = dynamic_cast<IntegerType *>(t.get()))
        if (i->getBitWidth() == bits)
          return i;
    }
    return make<IntegerType>(bits);
  }

  FloatType *getFloatTy(FloatKind kind) {
    for (auto &t : types_) {
      if (auto *f = dynamic_cast<FloatType *>(t.get()))
        if (f->getFloatKind() == kind)
          return f;
    }
    return make<FloatType>(kind);
  }

  IntegerType *getInt1Ty() { return getIntTy(1); }

  IntegerType *getInt8Ty() { return getIntTy(8); }

  IntegerType *getInt16Ty() { return getIntTy(16); }

  IntegerType *getInt32Ty() { return getIntTy(32); }

  IntegerType *getInt64Ty() { return getIntTy(64); }

  FloatType *getHalfTy() { return getFloatTy(FloatKind::Half); }

  FloatType *getFloatTy() { return getFloatTy(FloatKind::Float); }

  FloatType *getDoubleTy() { return getFloatTy(FloatKind::Double); }

  PointerType *getPointerTy(Type *elem) {
    for (auto &t : types_) {
      if (auto *p = dynamic_cast<PointerType *>(t.get()))
        if (p->getElementType() == elem)
          return p;
    }
    return make<PointerType>(elem);
  }

  ArrayType *getArrayTy(Type *elem, unsigned count) {
    for (auto &t : types_) {
      if (auto *a = dynamic_cast<ArrayType *>(t.get()))
        if (a->getElementType() == elem && a->getNumElements() == count)
          return a;
    }
    return make<ArrayType>(elem, count);
  }

  VectorType *getVectorTy(Type *elem, unsigned count) {
    for (auto &t : types_) {
      if (auto *v = dynamic_cast<VectorType *>(t.get()))
        if (v->getElementType() == elem && v->getNumElements() == count)
          return v;
    }
    return make<VectorType>(elem, count);
  }

  StructType *getStructTy(const std::vector<Type *> &elements) {
    for (auto &t : types_) {
      if (auto *s = dynamic_cast<StructType *>(t.get()))
        if (s->getElements() == elements)
          return s;
    }
    return make<StructType>(elements);
  }

  FunctionType *getFunctionTy(Type *ret, const std::vector<Type *> &params) {
    for (auto &t : types_) {
      if (auto *f = dynamic_cast<FunctionType *>(t.get()))
        if (f->getReturnType() == ret && f->getParamTypes() == params)
          return f;
    }
    return make<FunctionType>(ret, params);
  }

  LabelType *getLabelTy() {
    if (!labelTy_)
      labelTy_ = make<LabelType>();
    return labelTy_;
  }

  // ── Constant factory ────────────────────────────────────────────
  // Constants are owned by the Context. Callers receive raw pointers
  // that remain valid for the lifetime of the Context.

  ConstantInt *getConstantInt(IntegerType *ty, int64_t val) {
    return makeConst<ConstantInt>(ty, val);
  }

  ConstantFP *getConstantFP(FloatType *ty, double val) {
    return makeConst<ConstantFP>(ty, val);
  }

  ConstantPointerNull *getConstantNull(PointerType *ty) {
    return makeConst<ConstantPointerNull>(ty);
  }

private:
  template <typename T, typename... Args> T *make(Args &&...args) {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    T *raw = ptr.get();
    types_.emplace_back(std::move(ptr));
    return raw;
  }

  template <typename T, typename... Args> T *makeConst(Args &&...args) {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    T *raw = ptr.get();
    constants_.emplace_back(std::move(ptr));
    return raw;
  }

  std::vector<std::unique_ptr<Type>> types_;
  std::vector<std::unique_ptr<Value>> constants_;

  VoidType *voidTy_ = nullptr;
  LabelType *labelTy_ = nullptr;
};
} // namespace anvil::ir
