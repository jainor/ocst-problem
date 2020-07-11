#include "util_optimal.h"

#include <cmath>

#include "gtest/gtest.h"

const double kEps = 1e-8;

TEST(UtilOptimal, FloydWarshall) {
  const int n = 5;
  vector<vector<double> > dist = {{0, 13, 1, 2, 1},
                                  {13, 0, 2, 7, 1},
                                  {1, 2, 0, 5, 10},
                                  {2, 7, 5, 0, 4},
                                  {1, 1, 10, 4, 0}};
  UtilOptimal::FloydWarshall(dist);
  EXPECT_TRUE(fabs(2 - dist[0][1]) < kEps);
}

/*int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}*/
