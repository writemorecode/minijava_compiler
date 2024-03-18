#ifndef TAC_HPP
#define TAC_HPP

#include <string>

class Tac {
  protected:
    std::string result, lhs, op, rhs;

  public:
    virtual void print(std::ostream &os) const;

    Tac(const std::string &result_, const std::string &lhs_,
        const std::string &op_, const std::string &rhs_)
        : result(result_), lhs(lhs_), op(op_), rhs(rhs_) {}
    virtual ~Tac() = default;
};

class UnaryExpressionTac : public Tac {
  public:
    UnaryExpressionTac(const std::string &result_, const std::string &op_,
                       const std::string &z_)
        : Tac(result_, "", op_, z_){};
    void print(std::ostream &os) const override;
};

class NotTac : public UnaryExpressionTac {
  public:
    NotTac(const std::string &result_, const std::string &z_)
        : UnaryExpressionTac(result_, "!", z_){};
};

class CopyTac : public Tac {
  public:
    CopyTac(const std::string &y_, const std::string &result_)
        : Tac(result_, y_, ":=", ""){};
    void print(std::ostream &os) const override;
};

class ArrayCopyTac : public Tac {
  public:
    ArrayCopyTac(const std::string &result_, const std::string &index_,
                 const std::string &z_)
        : Tac(result_, index_, ":=", z_){};
    void print(std::ostream &os) const override;
};

class ArrayAccessTac : public Tac {
  public:
    ArrayAccessTac(const std::string &result_, const std::string &y_,
                   const std::string &z_)
        : Tac(result_, y_, "", z_){};
    void print(std::ostream &os) const override;
};

class NewTac : public Tac {
  public:
    NewTac(const std::string &result, const std::string &y_)
        : Tac(result, "", "new", y_){};
    void print(std::ostream &os) const override;
};

class NewArrayTac : public Tac {
  public:
    NewArrayTac(const std::string &result, const std::string &length_)
        : Tac(result, "", "new", length_){};
    void print(std::ostream &os) const override;
};

class JumpTac : public Tac {
  public:
    JumpTac(const std::string &_label) : Tac(_label, "", "goto", ""){};
    void print(std::ostream &os) const override;
};

class CondJumpTac : public Tac {
  public:
    CondJumpTac(const std::string &label, const std::string &cond)
        : Tac("", cond, "", label){};
    void print(std::ostream &os) const override;
};

class ParamTac : public Tac {
  public:
    ParamTac(const std::string &param) : Tac("", "", "", param){};
    void print(std::ostream &os) const override;
};

class MethodCallTac : public Tac {
  public:
    MethodCallTac(const std::string &result, const std::string &methodName,
                  const std::string &argCount)
        : Tac(result, methodName, "", argCount){};
    void print(std::ostream &os) const override;
};

class ReturnTac : public Tac {
  public:
    ReturnTac(const std::string &name) : Tac(name, "", "", ""){};
    void print(std::ostream &os) const override;
};

class PrintTac : public Tac {
  public:
    PrintTac(const std::string &value) : Tac(value, "", "", ""){};
    void print(std::ostream &os) const override;
};

#endif
