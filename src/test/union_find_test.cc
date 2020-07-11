#include "union_find.h"

#include "gtest/gtest.h"

TEST(UnionFind, operations) {
  UnionFind union_find(5);
  EXPECT_EQ(4, union_find.Find(4));
  EXPECT_EQ(0, union_find.Find(0));
  EXPECT_TRUE(union_find.UnionSet(0, 4));
  EXPECT_TRUE(union_find.UnionSet(0, 3));
  EXPECT_FALSE(union_find.UnionSet(3, 4));
  EXPECT_EQ(3, union_find.components());
  EXPECT_FALSE(union_find.UnionSet(0, 4));
  EXPECT_TRUE(union_find.UnionSet(1, 2));
  EXPECT_EQ(2, union_find.components());
}

TEST(UnionFind, Runtime) {
  try {
    UnionFind union_find(-123);
  } catch (std::exception &e) {
    SUCCEED();
    return;
  }
  ADD_FAILURE() << "Didn't throw exception as expected";
}

/*
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}*/
