#ifndef SOLVER_H
#define SOLVER_H

#include <string>

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "optimal_struct.h"

class Solver {
 public:
  static const int kInf = (1 << 29);
  static const double kHeuristicOp;
  static const int kTimeLimitOp;
  const std::string kFormulationName;

  Solver(OptimalStruct internalS)
      : internal_s_(internalS), env_(GRBEnv()), model_(GRBModel(env_)) {
    InitModel();
  }
  virtual Ans Solve(int timeLimit, double heuristics) = 0;
  virtual ~Solver() {}
  virtual void SetInitialSolution() = 0;
  virtual void AddConstraintsAndObjective() = 0;

  static double GetUpper(GRBModel &model_);
  static double GetLower(GRBModel &model_);
  static double GetOptimalityGap(GRBModel &model_);

  OptimalStruct internal_s_;

 protected:
  GRBEnv env_;
  GRBModel model_;

 private:
  void InitModel() {
    model_.set(GRB_IntParam_OutputFlag, 0);
    model_.set(GRB_IntParam_LazyConstraints, 1);
    model_.set(GRB_IntParam_Presolve, 0);
    model_.set(GRB_IntParam_PreCrush, 1);
    model_.set(GRB_IntParam_Threads, 1);
  }
};

// best known answer
inline double Solver::GetUpper(GRBModel &model_) {
  if (model_.get(GRB_IntAttr_SolCount) == 0) {
    return GRB_INFINITY;
  }
  return model_.get(GRB_DoubleAttr_ObjVal);
}
inline double Solver::GetLower(GRBModel &model_) {
  return max(model_.get(GRB_DoubleAttr_ObjBound), 0.0);
}

inline double Solver::GetOptimalityGap(GRBModel &model_) {
  double upper = Solver::GetUpper(model_);
  double lower = Solver::GetLower(model_);
  return fabs(lower - upper) / upper;
  // return model_.get(GRB_DoubleAttr_MIPGap);
}

#endif
