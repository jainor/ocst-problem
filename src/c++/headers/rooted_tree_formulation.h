#ifndef ROOTED_TREE_FORMULATION_H
#define ROOTED_TREE_FORMULATION_H

#include "solver.h"

class RootedBased : public Solver {
 public:
  RootedBased(OptimalStruct internalS) : Solver(internalS) {}
  Ans Solve(int timeLimit, double heuristics);
  ~RootedBased() {}

  void SetInitialSolution();
  void AddConstraintsAndObjective();

 private:
  const int kRoot = 0;
  GRBVar *x_;
  GRBVar **p_;
  GRBVar **d_;
};

#endif
