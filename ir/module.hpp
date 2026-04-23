#pragma once

#include <ir/constants/string.hpp>
#include <ir/function.hpp>
#include <ir/global.hpp>

namespace anvil::ir {

class Module {
public:
  explicit Module(std::string name) : name_(std::move(name)) {}

  const std::string &name() const { return name_; }

  void addGlobal(std::unique_ptr<GlobalVariable> g) {
    globals_.push_back(std::move(g));
  }

  GlobalVariable *createStringLiteral(Context &ctx, const std::string &str) {
    Type *i8 = ctx.getInt8Ty();
    Type *arrayTy = ctx.getArrayTy(i8, str.size() + 1);
    Type *ptrTy = ctx.getPointerTy(arrayTy);

    auto cstr = std::make_unique<ConstantString>(arrayTy, str);
    auto gv = std::make_unique<GlobalVariable>(
        ptrTy, ".str." + std::to_string(stringId_++), std::move(cstr));

    GlobalVariable *ptr = gv.get();
    addGlobal(std::move(gv));
    return ptr;
  }

  void addFunction(std::unique_ptr<Function> fn) {
    functions_.push_back(std::move(fn));
  }

  void print(std::ostream &os) const {
    os << "; ModuleID = '" << name_ << "'\n";
    if (!targetTriple_.empty())
      os << "target triple = \"" << targetTriple_ << "\"\n";
    if (!dataLayout_.empty())
      os << "target datalayout = \"" << dataLayout_ << "\"\n";
    os << "\n";

    for (const auto &g : globals_) {
      g->print(os);
      os << "\n";
    }
    if (!globals_.empty())
      os << "\n";

    for (const auto &fn : functions_) {
      fn->print(os);
      os << "\n";
    }
  }

  void setTargetTriple(std::string tt) { targetTriple_ = std::move(tt); }
  void setDataLayout(std::string dl) { dataLayout_ = std::move(dl); }

private:
  std::string name_;
  std::vector<std::unique_ptr<GlobalVariable>> globals_;
  std::vector<std::unique_ptr<Function>> functions_;
  std::string targetTriple_;
  std::string dataLayout_;
  unsigned stringId_ = 0;
};
} // namespace anvil::ir
