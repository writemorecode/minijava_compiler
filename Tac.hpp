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

class CopyTac : public Tac {
  public:
    CopyTac(std::string y_, std::string result_) : Tac(result_, y_, ":=", ""){};
    void print() const override;
};

class ArrayAccessTac : public Tac {
  public:
    ArrayAccessTac(std::string result_, std::string y_, std::string z_)
        : Tac(result_, y_, "", z_){};
    void print() const override;
};

#endif
