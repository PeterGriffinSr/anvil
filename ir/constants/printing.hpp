#pragma once

#include <ir/constants/constant.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace anvil::ir {
class Constant;

inline void printList(std::ostream &os, const std::vector<Constant *> &elems,
                      char open, char close, const std::string &sep = ", ") {
  os << open;
  for (size_t i = 0; i < elems.size(); ++i) {
    if (i)
      os << sep;
    elems[i]->print(os);
  }
  os << close;
}
} // namespace anvil::ir
