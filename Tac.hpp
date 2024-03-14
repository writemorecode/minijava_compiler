#ifndef TAC_HPP
#define TAC_HPP

#include <string>

class Tac {
  protected:
    std::string result, lhs, op, rhs;

  public:
    std::string getResult() const { return result; }
    std::string getLhs() const { return lhs; }
    std::string getOp() const { return op; }
    std::string getRhs() const { return rhs; }

    void setResult(std::string result_) { result = result_; }
    void setLhs(std::string lhs_) { lhs = lhs_; }
    void setOp(std::string op_) { op = op_; }
    void setRhs(std::string rhs_) { rhs = rhs_; }

    virtual void print() const;

    Tac(std::string result_, std::string lhs_, std::string op_,
        std::string rhs_)
        : result(result_), lhs(lhs_), op(op_), rhs(rhs_) {}
    virtual ~Tac() = default;
};

class ExpressionTac : public Tac {
  public:
    ExpressionTac(std::string result_, std::string y_, std::string op_,
                  std::string z_)
        : Tac(result_, y_, op_, z_){};
    void print() const override;
};

class UnaryExpressionTac : public Tac {
  public:
    UnaryExpressionTac(std::string result_, std::string op_, std::string z_)
        : Tac(result_, "", op_, z_){};
    void print() const override;
};

class CopyTac : public Tac {
  public:
    CopyTac(std::string y_, std::string result_) : Tac(result_, y_, ":=", ""){};
    void print() const override;
};

class ArrayCopyTac : public Tac {
  public:
    ArrayCopyTac(std::string result_, std::string index_, std::string z_)
        : Tac(result_, index_, ":=", z_){};
    void print() const override;
};

class ArrayAccessTac : public Tac {
  public:
    ArrayAccessTac(std::string result_, std::string y_, std::string z_)
        : Tac(result_, y_, "", z_){};
    void print() const override;
};

class NewTac : public Tac {
  public:
    NewTac(std::string result, std::string y_) : Tac(result, "", "new", y_){};
    void print() const override;
};

class NewArrayTac : public Tac {
  public:
    NewArrayTac(std::string result, std::string length_)
        : Tac(result, "", "new", length_){};
    void print() const override;
};

class JumpTac : public Tac {
  public:
    JumpTac(std::string _label) : Tac(_label, "", "goto", ""){};
    void print() const override;
};

class CondJumpTac : public Tac {
  public:
    CondJumpTac(std::string label, std::string cond)
        : Tac("", cond, "", label){};
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

#endif
