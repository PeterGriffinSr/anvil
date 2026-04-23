#pragma once

#include <ir/constants/constant.hpp>
#include <ostream>
#include <string>

namespace anvil::ir {
class ConstantString final : public Constant {
public:
  ConstantString(Type *arrayTy, std::string value)
      : Constant(arrayTy), value_(std::move(value)) {}

  const std::string &value() const noexcept { return value_; }

  void print(std::ostream &os) const override {
    os << "c\"";
    for (unsigned char c : value_) {
      switch (c) {
      case '\n':
        os << "\\0A";
        break;
      case '\t':
        os << "\\09";
        break;
      case '\"':
        os << "\\22";
        break;
      case '\\':
        os << "\\5C";
        break;
      default:
        if (c < 32 || c >= 127) {
          os << '\\';
          os << "0123456789ABCDEF"[c >> 4];
          os << "0123456789ABCDEF"[c & 0xF];
        } else {
          os << c;
        }
      }
    }
    os << "\\00\"";
  }

private:
  std::string value_;
};
} // namespace anvil::ir
