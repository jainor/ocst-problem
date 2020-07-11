#ifndef PATH_BASED_FORMULATION_H
#define PATH_BASED_FORMULATION_H

#include "gurobi_c++.h"
#include "solver.h"

class PathBased : public Solver {
 public:
  PathBased(OptimalStruct internalS) : Solver(internalS) {}
  Ans Solve(int timeLimit, double heuristics);
  ~PathBased() {}

  void SetInitialSolution();
  void AddConstraintsAndObjective();

 private:
  GRBVar *x_;
  GRBVar **y_;
};

#endif
