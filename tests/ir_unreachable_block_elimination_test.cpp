#include <gtest/gtest.h>

#include "ir/CFG.hpp"
#include "ir/passes/UnreachableBlockEliminationPass.hpp"

TEST(IRUnreachableBlockElimination, RemovesDetachedSubgraph) {
    CFG graph;

    auto *root = graph.addMethodRootBlock("Main", "main");
    auto *reachable = graph.newBlock();
    root->setTrueBlock(reachable);

    auto *detached_a = graph.newBlock();
    auto *detached_b = graph.newBlock();
    detached_a->setTrueBlock(detached_b);

    graph.setCurrentBlock(detached_b);
    EXPECT_EQ(graph.getBlockCount(), 4U);

    UnreachableBlockEliminationPass pass;
    EXPECT_TRUE(pass.run(graph));
    EXPECT_EQ(graph.getBlockCount(), 2U);
    EXPECT_EQ(graph.getMethodRoots().size(), 1U);
    EXPECT_EQ(graph.getCurrentBlock(), nullptr);

    EXPECT_FALSE(pass.run(graph));
    EXPECT_EQ(graph.getBlockCount(), 2U);
}

TEST(IRUnreachableBlockElimination, NoChangeWhenAllBlocksReachable) {
    CFG graph;

    auto *root = graph.addMethodRootBlock("Main", "main");
    auto *then_block = graph.newBlock();
    auto *else_block = graph.newBlock();
    root->setTrueBlock(then_block);
    root->setFalseBlock(else_block);
    else_block->setTrueBlock(then_block);

    graph.setCurrentBlock(then_block);
    EXPECT_EQ(graph.getBlockCount(), 3U);

    UnreachableBlockEliminationPass pass;
    EXPECT_FALSE(pass.run(graph));
    EXPECT_EQ(graph.getBlockCount(), 3U);
    EXPECT_EQ(graph.getMethodRoots().size(), 1U);
    EXPECT_EQ(graph.getCurrentBlock(), then_block);
}

TEST(IRUnreachableBlockElimination, KeepsMultipleMethodRoots) {
    CFG graph;

    auto *root_one = graph.addMethodRootBlock("Main", "main");
    auto *root_two = graph.addMethodRootBlock("Foo", "run");
    auto *reachable_from_root_one = graph.newBlock();
    root_one->setTrueBlock(reachable_from_root_one);

    (void)graph.newBlock();

    graph.setCurrentBlock(root_two);
    EXPECT_EQ(graph.getBlockCount(), 4U);
    EXPECT_EQ(graph.getMethodRoots().size(), 2U);

    UnreachableBlockEliminationPass pass;
    EXPECT_TRUE(pass.run(graph));
    EXPECT_EQ(graph.getBlockCount(), 3U);
    EXPECT_EQ(graph.getMethodRoots().size(), 2U);
    EXPECT_EQ(graph.getCurrentBlock(), root_two);
}
