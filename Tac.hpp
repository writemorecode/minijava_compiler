#ifndef TAC_HPP
#define TAC_HPP

#include <string>
// #include <utility>
// #include <utility>

class Tac {
  protected:
    std::string result, lhs, op, rhs;

  public:
    [[nodiscard]] std::string getResult() const { return result; }
    [[nodiscard]] std::string getLhs() const { return lhs; }
    [[nodiscard]] std::string getOp() const { return op; }
    [[nodiscard]] std::string getRhs() const { return rhs; }

    void setResult(std::string result_) { result = std::move(result_); }
    void setLhs(std::string lhs_) { lhs = std::move(lhs_); }
    void setOp(std::string op_) { op = std::move(op_); }
    void setRhs(std::string rhs_) { rhs = std::move(rhs_); }

    virtual void print() const;

    Tac(std::string result_, std::string lhs_, std::string op_,
        std::string rhs_)
        : result(std::move(std::move(result_))),
          lhs(std::move(std::move(lhs_))), op(std::move(std::move(op_))),
          rhs(std::move(std::move(rhs_))) {}
    virtual ~Tac() = default;
};

class ExpressionTac : public Tac {
  public:
    ExpressionTac(std::string result_, std::string y_, std::string op_,
                  std::string z_)
        : Tac(std::move(result_), std::move(y_), std::move(op_),
              std::move(z_)){};
    void print() const override;
};

class UnaryExpressionTac : public Tac {
  public:
    UnaryExpressionTac(std::string result_, std::string op_, std::string z_)
        : Tac(std::move(result_), "", std::move(op_), std::move(z_)){};
    void print() const override;
};

class CopyTac : public Tac {
  public:
    CopyTac(std::string y_, std::string result_)
        : Tac(std::move(result_), std::move(y_), ":=", ""){};
    void print() const override;
};

class ArrayCopyTac : public Tac {
  public:
    ArrayCopyTac(std::string result_, std::string index_, std::string z_)
        : Tac(std::move(result_), std::move(index_), ":=", std::move(z_)){};
    void print() const override;
};

class ArrayAccessTac : public Tac {
  public:
    ArrayAccessTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "", std::move(z_)){};
    void print() const override;
};

class NewTac : public Tac {
  public:
    NewTac(std::string result, std::string y_)
        : Tac(std::move(result), "", "new", std::move(y_)){};
    void print() const override;
};

class NewArrayTac : public Tac {
  public:
    NewArrayTac(std::string result, std::string length_)
        : Tac(std::move(result), "", "new", std::move(length_)){};
    void print() const override;
};

class JumpTac : public Tac {
  public:
    JumpTac(std::string _label) : Tac(std::move(_label), "", "goto", ""){};
    void print() const override;
};

class CondJumpTac : public Tac {
  public:
    CondJumpTac(std::string label, std::string cond)
        : Tac("", std::move(cond), "", std::move(label)){};
    void print() const override;
};

class ParamTac : public Tac {
  public:
    ParamTac(const std::string &param) : Tac("", "", "", param){};
    void print() const override;
};

class MethodCallTac : public Tac {
  public:
    MethodCallTac(const std::string &result, const std::string &methodName,
                  const std::string &argCount)
        : Tac(result, methodName, "", argCount){};
    void print() const override;
};

class ReturnTac : public Tac {
  public:
    ReturnTac(const std::string &name) : Tac(name, "", "", ""){};
    void print() const override;
};

class PrintTac : public Tac {
  public:
    PrintTac(const std::string &value) : Tac(value, "", "", ""){};
    void print() const override;
};

#endif
