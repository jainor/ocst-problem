#ifndef FLOW_BASED_RELAXED_H
#define FLOW_BASED_RELAXED_H

#include "solver.h"

class FlowBasedRelaxed : public Solver {
 public:
  FlowBasedRelaxed(OptimalStruct internalS) : Solver(internalS) {}
  Ans Solve(int timeLimit, double heuristics);
  ~FlowBasedRelaxed() {}

  void SetInitialSolution();
  void AddConstraintsAndObjective();

 private:
  GRBVar *x_;
  GRBVar **f_;
};

#endif
