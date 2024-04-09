#ifndef TAC_HPP
#define TAC_HPP

#include "BytecodeMethodBlock.hpp"
#include <iostream>
#include <string>
#include <variant>

using Operand = std::variant<std::string, int>;
class Tac {
  protected:
    std::string result;
    Operand lhsOp;
    std::string op;
    Operand rhsOp;
    std::string lhs, rhs;

  private:
    static std::string to_string(const Operand &op) {
        if (const auto *ptr = std::get_if<int>(&op)) {
            return std::to_string(*ptr);
        }
        return std::get<std::string>(op);
    }

  public:
    virtual void print(std::ostream &os) const;

    virtual void
    generateBytecode([[maybe_unused]] BytecodeMethodBlock &block){};

    Tac(const std::string &result_) : result{result_} {}
    Tac(const std::string &result_, const Operand &lhs_, const std::string &op_,
        const Operand &rhs_)
        : result(result_), lhsOp(lhs_), op(op_), rhsOp(rhs_) {
        lhs = to_string(lhsOp);
        rhs = to_string(rhsOp);
    }
    Tac(const std::string &result_, const Operand &rhs_)
        : result{result_}, rhsOp{rhs_} {
        rhs = to_string(rhsOp);
    }
    Tac(const Operand &rhs_) : rhsOp{rhs_} { rhs = to_string(rhsOp); }
    virtual ~Tac() = default;
};

class ArrayCopyTac : public Tac {
  public:
    ArrayCopyTac(const std::string &result_, const Operand &index_,
                 const Operand &z_)
        : Tac(result_, index_, ":=", z_){};
    void print(std::ostream &os) const override;
};
class ArrayAccessTac : public Tac {
  public:
    ArrayAccessTac(const std::string &result_, const Operand &y_,
                   const Operand &z_)
        : Tac(result_, y_, "", z_){};
    void print(std::ostream &os) const override;
};
class ArrayLengthTac : public Tac {
  public:
    ArrayLengthTac(const std::string &result, const Operand &y_)
        : Tac(result, y_){};
    void print(std::ostream &os) const override;
};
class NewTac : public Tac {
  public:
    NewTac(const std::string &result, const Operand &y_) : Tac(result, y_){};
    void print(std::ostream &os) const override;
};

class NewArrayTac : public Tac {
  public:
    NewArrayTac(const std::string &result, const Operand &length_)
        : Tac(result, length_){};
    void print(std::ostream &os) const override;
};

class NotTac : public Tac {
  public:
    NotTac(const std::string &result_, const Operand &z_) : Tac(result_, z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
    void print(std::ostream &os) const override;
};

class CopyTac : public Tac {
  public:
    CopyTac(const Operand &y_, const std::string &result_) : Tac(result_, y_){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class CondJumpTac : public Tac {
  public:
    CondJumpTac(const Operand &label, const Operand &cond)
        : Tac("", cond, "", label){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class MethodCallTac : public Tac {
  public:
    MethodCallTac(const std::string &result, const std::string &objectName,
                  const Operand &methodName, const Operand &argCount)
        : Tac(result, methodName, objectName, argCount){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class JumpTac : public Tac {
  public:
    JumpTac(const std::string &_label) : Tac(_label){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class ParamTac : public Tac {
  public:
    ParamTac(const Operand &param) : Tac(param){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class ReturnTac : public Tac {
  public:
    ReturnTac(const Operand &name) : Tac(name){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class PrintTac : public Tac {
  public:
    PrintTac(const Operand &value) : Tac(value){};
    void print(std::ostream &os) const override;
    void generateBytecode(BytecodeMethodBlock &block) override;
};

#endif
