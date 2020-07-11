#include "flow_based_formulation.h"

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "network.h"
#include "optimal_struct.h"
#include "sec_integer.h"
#include "sec_separation.h"
#include "solver.h"
#include "util_optimal.h"
#include "util_solver.h"

Ans FlowBased::Solve(int timeLimit, double heuristics) {
  const int n = internal_s_.n;
  const int m = internal_s_.arcs.size();
  const double prob = internal_s_.prob;
  const int edges = internal_s_.edges.size();
  const int r = internal_s_.requirements.size();

  try {
    model_.set(GRB_DoubleParam_TimeLimit, timeLimit);
    model_.set(GRB_DoubleParam_Heuristics, heuristics);

    if (not UtilOptimal::IsConnected(internal_s_.edges, n)) {
      return Ans(n, m, r, prob, -1, -1, -1, -1);
    }

    x_ = model_.addVars(m, GRB_BINARY);  // edges

    f_ = new GRBVar *[n];  // rnodes * edges
    y_ = new GRBVar *[n];  // rnodes * arcs

    // setup variables
    for (int i = 0; i < n; ++i) {
      f_[i] = model_.addVars(m, GRB_CONTINUOUS);
      y_[i] = model_.addVars(m, GRB_BINARY);
    }

    AddConstraintsAndObjective();
    SetInitialSolution();

    SECSep cb = SECSep(x_, &internal_s_);
    model_.setCallback(&cb);

    model_.optimize();

    double objval = Solver::kInf;
    const int generated_nodes = model_.get(GRB_DoubleAttr_NodeCount);
    const double elapsed_time = model_.get(GRB_DoubleAttr_Runtime);
    const int lazy = cb.lazy();
    const int cuts = cb.cuts();

    double relaxed;
    GRBModel model_relaxed = model_.relax();
    model_relaxed.optimize();

    if (model_relaxed.get(GRB_IntAttr_Status) == GRB_INFEASIBLE) {
      relaxed = -1;
    } else if (model_relaxed.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
      relaxed = model_relaxed.get(GRB_DoubleAttr_ObjVal);
    }

    const double lower_bound = Solver::GetLower(model_);
    const double upper_bound = Solver::GetUpper(model_);

    int optimstatus = model_.get(GRB_IntAttr_Status);

    /*
       if (optimstatus == GRB_INF_OR_UNBD) {
       model_.set(GRB_IntParam_Presolve, 0);
       model_.optimize();
       optimstatus = model_.get(GRB_IntAttr_Status);
       }*/

    if (optimstatus == GRB_OPTIMAL) {
      objval = model_.get(GRB_DoubleAttr_ObjVal);
    } else if (optimstatus == GRB_TIME_LIMIT) {
      objval = Solver::GetUpper(model_);
    } else {
      cout << "Optimization was stopped with status = " << optimstatus << endl;
      assert(false);
    }

    delete[] f_;
    delete[] y_;

    return Ans(n, edges, r, prob, objval, generated_nodes, elapsed_time,
               Solver::GetOptimalityGap(model_), lazy, cuts, relaxed,
               lower_bound, upper_bound);
  } catch (GRBException e) {
    cout << "Error code: " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during optimization" << endl;
  }
  return Ans(-1, -1, -1, -1, -1, -1, -1, -1);
}

void FlowBased::AddConstraintsAndObjective() {
  const int n = internal_s_.n;
  const int m = internal_s_.arcs.size();
  const double prob = internal_s_.prob;
  const int edges = internal_s_.edges.size();
  const int r = internal_s_.requirements.size();

  const std::vector<vector<int> > &index = internal_s_.indexes;
  const std::vector<vector<int> > &W = internal_s_.weights;
  const std::vector<Requirement> &require = internal_s_.requirements;
  const std::vector<std::pair<double, std::pair<int, int> > > &arcs =
      internal_s_.arcs;

  int nonzero = 0;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      if (index[i][j] != -1) {
        int ind1 = index[i][j];
        int ind2 = index[j][i];
        assert(ind2 >= 0);
        x_[ind1] = x_[ind2];
      }
    }
  }

  // n-1 active edges
  GRBLinExpr restr = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      if (index[i][j] >= 0) {
        int ind = index[i][j];
        restr += x_[ind];
        nonzero += 1;
      }
    }
  }
  model_.addConstr(restr, GRB_EQUAL, n - 1);

  for (int o = 0; o < n; ++o) {
    GRBLinExpr restrOr = 0;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < i; ++j) {
        if (index[i][j] >= 0) {
          int ind1 = index[i][j];
          int ind2 = index[j][i];
          assert(ind2 >= 0);

          restrOr += y_[o][ind1] + y_[o][ind2];
          model_.addConstr(y_[o][ind1] + y_[o][ind2], GRB_LESS_EQUAL, x_[ind1]);
          nonzero += 1;
        }
      }
    }
    model_.addConstr(restrOr, GRB_EQUAL, n - 1);
  }

  // fluxo entrando no No tem que ser 0.
  for (int o = 0; o < n; ++o) {
    GRBLinExpr restr = 0;
    for (int i = 0; i < n; ++i) {
      int ind = index[i][o];
      if (ind >= 0) {
        restr += f_[o][ind];
        nonzero += 1;
      }
    }
    model_.addConstr(restr, GRB_EQUAL, 0);
  }
  // demand of node
  for (int o = 0; o < n; ++o) {
    for (int j = 0; j < n; ++j) {
      if (j != o) {
        GRBLinExpr restr;
        for (int i = 0; i < n; ++i) {
          int ind = index[i][j];
          if (ind >= 0) {
            restr += f_[o][ind];
            nonzero += 1;
          }
        }
        for (int k = 0; k < n; ++k) {
          int ind = index[j][k];
          if (ind >= 0) {
            restr -= f_[o][ind];
            nonzero += 1;
          }
        }
        model_.addConstr(restr, GRB_EQUAL, W[o][j]);
      }
    }
  }
  // fluxo saindo do no O.
  for (int o = 0; o < n; ++o) {
    GRBLinExpr restr = 0;
    for (int i = 0; i < n; ++i) {
      int ind = index[o][i];
      if (ind >= 0) {
        restr += f_[o][ind];
        nonzero += 1;
      }
    }
    int tot = 0;
    for (int d = 0; d < n; ++d) {
      tot += W[o][d];
      nonzero += 1;
    }
    model_.addConstr(restr, GRB_EQUAL, tot);
  }

  for (int o = 0; o < n; ++o) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (index[i][j] >= 0) {
          int ind1 = index[i][j];
          int ind2 = index[j][i];
          assert(ind2 >= 0);

          GRBLinExpr restr = 0;
          restr = f_[o][ind1] + f_[o][ind2];
          nonzero += 2;

          int sumW = 0;
          for (int d = 0; d < n; ++d) {
            sumW += W[o][d];
            if (W[o][d] != 0) {
              nonzero += 1;
            }
          }

          model_.addConstr(restr, GRB_LESS_EQUAL, sumW * x_[ind1]);
        }
      }
    }
  }

  // set up objective
  GRBLinExpr objective = 0;
  for (int o = 0; o < n; ++o) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (index[i][j] >= 0) {
          int ind = index[i][j];
          int ind2 = index[j][i];
          assert(ind2 >= 0);
          assert(arcs[ind].first == arcs[ind2].first);
          objective += arcs[ind].first * f_[o][ind];
        }
      }
    }
  }

  model_.setObjective(objective, GRB_MINIMIZE);
}

void FlowBased::SetInitialSolution() {
  UtilSolver::SetInitialSolutionFlow(x_, f_, y_, &internal_s_);
}
