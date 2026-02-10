#include "ast/BooleanExpressionNode.hpp"
#include "ir/Tac.hpp"

Operand AndNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhsName = left->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);

    auto *rhsEvalBlock = graph.newBlock();
    auto *trueBlock = graph.newBlock();
    auto *falseBlock = graph.newBlock();
    auto *joinBlock = graph.newBlock();

    auto *entryBlock = graph.getCurrentBlock();
    graph.addInstruction(new CondJumpTac(falseBlock->getName(), lhsName));
    graph.addInstruction(new JumpTac(rhsEvalBlock->getName()));
    entryBlock->setTrueBlock(rhsEvalBlock);
    entryBlock->setFalseBlock(falseBlock);

    graph.setCurrentBlock(rhsEvalBlock);
    auto rhsName = right->generateIR(graph, st);
    graph.addInstruction(new CondJumpTac(falseBlock->getName(), rhsName));
    graph.addInstruction(new JumpTac(trueBlock->getName()));
    rhsEvalBlock->setTrueBlock(trueBlock);
    rhsEvalBlock->setFalseBlock(falseBlock);

    graph.setCurrentBlock(trueBlock);
    graph.addInstruction(new CopyTac(1, name));
    graph.addInstruction(new JumpTac(joinBlock->getName()));
    trueBlock->setTrueBlock(joinBlock);

    graph.setCurrentBlock(falseBlock);
    graph.addInstruction(new CopyTac(0, name));
    graph.addInstruction(new JumpTac(joinBlock->getName()));
    falseBlock->setTrueBlock(joinBlock);

    graph.setCurrentBlock(joinBlock);
    return name;
}

Operand OrNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhsName = left->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);

    auto *rhsEvalBlock = graph.newBlock();
    auto *trueBlock = graph.newBlock();
    auto *falseBlock = graph.newBlock();
    auto *joinBlock = graph.newBlock();

    auto *entryBlock = graph.getCurrentBlock();
    graph.addInstruction(new CondJumpTac(rhsEvalBlock->getName(), lhsName));
    graph.addInstruction(new JumpTac(trueBlock->getName()));
    entryBlock->setTrueBlock(trueBlock);
    entryBlock->setFalseBlock(rhsEvalBlock);

    graph.setCurrentBlock(rhsEvalBlock);
    auto rhsName = right->generateIR(graph, st);
    graph.addInstruction(new CondJumpTac(falseBlock->getName(), rhsName));
    graph.addInstruction(new JumpTac(trueBlock->getName()));
    rhsEvalBlock->setTrueBlock(trueBlock);
    rhsEvalBlock->setFalseBlock(falseBlock);

    graph.setCurrentBlock(trueBlock);
    graph.addInstruction(new CopyTac(1, name));
    graph.addInstruction(new JumpTac(joinBlock->getName()));
    trueBlock->setTrueBlock(joinBlock);

    graph.setCurrentBlock(falseBlock);
    graph.addInstruction(new CopyTac(0, name));
    graph.addInstruction(new JumpTac(joinBlock->getName()));
    falseBlock->setTrueBlock(joinBlock);

    graph.setCurrentBlock(joinBlock);
    return name;
}
