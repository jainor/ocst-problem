#ifndef FLOW_BASED_FORMULATION_H
#define FLOW_BASED_FORMULATION_H

#include "solver.h"

class FlowBased : public Solver {
 public:
  FlowBased(OptimalStruct internalS) : Solver(internalS) {}
  Ans Solve(int timeLimit, double heuristics);
  ~FlowBased() {}

  void SetInitialSolution();
  void AddConstraintsAndObjective();

 private:
  GRBVar **f_;
  GRBVar **y_;
  GRBVar *x_;
};

#endif
