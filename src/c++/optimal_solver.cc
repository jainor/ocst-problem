#include "optimal_solver.h"

#include "ans_optimal.h"
#include "formulations.h"
#include "optimal_struct.h"
#include "read_instance.h"
#include "solver.h"

Ans InstanceSolver::Solve(string file_output_name, int time_limit,
                          double heuristics) const {
  Ans result = solver_->Solve(time_limit, heuristics);
  if (not file_output_name.empty()) {
    ofstream myfile;
    myfile.open(file_output_name);
    myfile << result;
    myfile.close();
  }
  return result;
}

inline void InstanceSolver::PrintBest() {
  int tot = 0;
  cout << "#PrintSolution" << endl;
  for (int i = 0; i < best_.size(); ++i) {
    int index = best_[i];
    cout << "( " << internal_s_->arcs[index].second.first;
    cout << ", " << internal_s_->arcs[index].second.second << ")";
    tot += internal_s_->arcs[index].first;
    cout << " = " << internal_s_->arcs[index].first << endl;
  }
  cout << "tot:" << tot << endl;
}

InstanceSolver InstanceSolver::GetInstance(std::istream& in,
                                           string formulation) {
  OptimalStruct input_ocst = ReaderOCST::ReadInstance(in);
  return InstanceSolver(Formulations::GetFormulation(input_ocst, formulation));
}
