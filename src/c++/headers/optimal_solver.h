#ifndef OPTIMAL_SOLVER_H
#define OPTIMAL_SOLVER_H

#include "ans_optimal.h"
#include "optimal_struct.h"
#include "solver.h"

class InstanceSolver {
 public:
  static InstanceSolver GetInstance(std::istream& in, string formulation);

  Ans Solve(string file_output_name, int time_limit, double heuristics) const;
  void PrintBest();

 private:
  InstanceSolver(Solver* solver)
      : internal_s_(&solver->internal_s_), solver_(solver) {}

  const OptimalStruct* internal_s_;
  Solver* solver_;

  vector<int> best_;
  int generated_;
  int active_;
};

#endif
