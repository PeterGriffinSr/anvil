#pragma once

#include <ostream>

namespace anvil::ir {
class Type {
public:
  enum class Kind {
    Void,
    Integer,
    Float,
    Pointer,
    Array,
    Vector,
    Struct,
    Function,
    Label
  };

  virtual ~Type() = default;

  Kind getKind() const noexcept { return kind_; }

  virtual void print(std::ostream &os) const = 0;

protected:
  explicit Type(Kind k) : kind_(k) {}

private:
  Kind kind_;
};
} // namespace anvil::ir