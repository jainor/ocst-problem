#include <cassert>

#include "formulations.h"
#include "optimal_solver.h"
#include "optimal_struct.h"
#include "solver.h"

//#define TEST

const int kMinArgs = 3;  // name, formulation and outputFileName
const int kMaxArgs = 5;  // including name of the program

template <class T>
T ParseNum(char *s) {
  stringstream ss(s);
  T result;
  ss >> result;
  return result;
}

int main(int argc, char *argv[]) {
  if (argc < kMinArgs) {
    cout << "Usage: provide at least two arguments (excluding main)" << endl;
    return 0;
  }

  string str_formulation = argv[1];
  string file_output = argv[2];

  int time_limit = Solver::kTimeLimitOp;
  double heuristics = Solver::kHeuristicOp;

  if (argc >= 4) {
    time_limit = ParseNum<int>(argv[3]);
  }
  if (argc >= kMaxArgs) {
    heuristics = ParseNum<double>(argv[4]);
    if (argc > kMaxArgs) {
      cout << "Warning: you provided more arguments than needed!" << endl;
    }
  }

  cout << "Procesing: " << str_formulation << " " << file_output << " "
       << time_limit << " " << heuristics << endl;

  auto solver = InstanceSolver::GetInstance(std::cin, str_formulation);
  solver.Solve(file_output, time_limit, heuristics);

  return 0;
}
