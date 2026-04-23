#pragma once

#include <ir/basic_block.hpp>
#include <ir/instruction.hpp>
#include <ir/types/pointer.hpp>

namespace anvil::ir {
inline void Instruction::printTerminator(std::ostream &os) const {
  switch (opcode_) {
  case Opcode::Br:
    if (operands_.size() == 1) {
      os << "br label %" << static_cast<BasicBlock *>(operands_[0])->getName();
    } else if (operands_.size() == 3) {
      os << "br i1 ";
      operands_[0]->printAsOperand(os);
      os << ", label %" << static_cast<BasicBlock *>(operands_[1])->getName();
      os << ", label %" << static_cast<BasicBlock *>(operands_[2])->getName();
    } else {
      os << "br";
    }
    break;

  case Opcode::Ret:
    if (!operands_.empty()) {
      os << "ret ";
      operands_[0]->getType()->print(os);
      os << " ";
      operands_[0]->printAsOperand(os);
    } else {
      os << "ret void";
    }
    break;

  default:
    os << "unknown terminator";
    break;
  }
}

inline void Instruction::printOtherOps(std::ostream &os) const {
  if (opcode_ == Opcode::PHI) {
    os << "phi ";
    type_->print(os);
    os << " ";
    for (size_t i = 0; i < incoming_.size(); ++i) {
      if (i > 0)
        os << ", ";
      os << "[";
      incoming_[i].first->printAsOperand(os);
      os << ", %";
      os << incoming_[i].second->getName();
      os << "]";
    }
    return;
  }

  if (opcode_ == Opcode::ICmp) {
    os << "icmp " << icmpPredStr() << " ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    os << ", ";
    operands_[1]->printAsOperand(os);
    return;
  }

  if (opcode_ == Opcode::FCmp) {
    os << "fcmp " << icmpPredStr() << " ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    os << ", ";
    operands_[1]->printAsOperand(os);
    return;
  }

  if (opcode_ == Opcode::Alloca) {
    os << "alloca ";
    if (auto *pt = dynamic_cast<PointerType *>(type_))
      pt->getElementType()->print(os);
    else
      type_->print(os);
    return;
  }

  if (opcode_ == Opcode::Load) {
    os << "load ";
    type_->print(os);
    os << ", ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    return;
  }

  if (opcode_ == Opcode::Store) {
    os << "store ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    os << ", ";
    operands_[1]->getType()->print(os);
    os << " ";
    operands_[1]->printAsOperand(os);
    return;
  }

  if (opcode_ == Opcode::GetElementPtr) {
    os << "getelementptr ";
    if (auto *pt = dynamic_cast<PointerType *>(operands_[0]->getType()))
      pt->getElementType()->print(os);
    else
      operands_[0]->getType()->print(os);
    os << ", ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    for (size_t i = 1; i < operands_.size(); ++i) {
      os << ", ";
      operands_[i]->getType()->print(os);
      os << " ";
      operands_[i]->printAsOperand(os);
    }
    return;
  }

  if (opcode_ == Opcode::Select) {
    os << "select i1 ";
    operands_[0]->printAsOperand(os);
    os << ", ";
    operands_[1]->getType()->print(os);
    os << " ";
    operands_[1]->printAsOperand(os);
    os << ", ";
    operands_[2]->getType()->print(os);
    os << " ";
    operands_[2]->printAsOperand(os);
    return;
  }

  if (opcode_ == Opcode::Call || opcode_ == Opcode::Tail) {
    if (opcode_ == Opcode::Tail)
      os << "tail ";
    os << "call ";
    type_->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    os << "(";
    for (size_t i = 1; i < operands_.size(); ++i) {
      if (i > 1)
        os << ", ";
      operands_[i]->getType()->print(os);
      os << " ";
      operands_[i]->printAsOperand(os);
    }
    os << ")";
    return;
  }

  if (operands_.size() == 1) {
    os << opcodeStr() << " ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    os << " to ";
    type_->print(os);
    return;
  }
  if (operands_.size() == 2) {
    os << opcodeStr() << " ";
    operands_[0]->getType()->print(os);
    os << " ";
    operands_[0]->printAsOperand(os);
    os << ", ";
    operands_[1]->printAsOperand(os);
    return;
  }

  os << opcodeStr() << " ";
  for (size_t i = 0; i < operands_.size(); ++i) {
    if (i > 0)
      os << ", ";
    operands_[i]->getType()->print(os);
    os << " ";
    operands_[i]->printAsOperand(os);
  }
}
} // namespace anvil::ir
