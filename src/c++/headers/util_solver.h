#ifndef UTIL_SOLVER_H
#define UTIL_SOLVER_H

#include <memory>
#include <vector>

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "network.h"
#include "optimal_struct.h"
#include "sec_integer.h"
#include "union_find.h"
#include "util_optimal.h"

class UtilSolver {
 public:
  static int GetInitialSolution(GRBVar *x, const OptimalStruct *internalS,
                                double *newX);
  static std::vector<std::vector<int> > GetPaths(const OptimalStruct *internalS,
                                                 const double *newX);
  // The method to add an initial incumbent
  // The path and rooted tree methods are implemented in their own class
  static void SetInitialSolutionFlow(GRBVar *x, GRBVar **f, GRBVar **y,
                                     OptimalStruct *internalS);

 private:
  UtilSolver() {}
};

#endif
