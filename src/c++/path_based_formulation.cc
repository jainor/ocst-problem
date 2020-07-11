#include "path_based_formulation.h"

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "network.h"
#include "optimal_struct.h"
#include "sec_integer.h"
#include "sec_separation.h"
#include "solver.h"
#include "util_optimal.h"
#include "util_solver.h"

Ans PathBased::Solve(int time_limit, double heuristics) {
  const int n = internal_s_.n;
  const double prob = internal_s_.prob;
  const int m = internal_s_.arcs.size();
  const int edges = internal_s_.edges.size();
  const int r = internal_s_.requirements.size();

  try {
    model_.set(GRB_DoubleParam_TimeLimit, time_limit);
    model_.set(GRB_DoubleParam_Heuristics, heuristics);

    if (not UtilOptimal::IsConnected(internal_s_.edges, n)) {
      return Ans(n, m, r, prob, -1, -1, -1, -1);
    }

    x_ = model_.addVars(m, GRB_BINARY);  // edges
    y_ = new GRBVar *[r];                // requirements * edges

    for (int i = 0; i < r; ++i) {
      y_[i] = model_.addVars(m, GRB_CONTINUOUS);
    }

    AddConstraintsAndObjective();
    SetInitialSolution();

    SECSep cb = SECSep(x_, &internal_s_);
    model_.setCallback(&cb);

    double objval = kInf;
    model_.optimize();
    const int generated_nodes = model_.get(GRB_DoubleAttr_NodeCount);
    const double elapsed_time = model_.get(GRB_DoubleAttr_Runtime);
    const int lazy = cb.lazy();
    const int cuts = cb.cuts();

    double relaxed = kInf;
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
  ;
}
void PathBased::AddConstraintsAndObjective() {
  const int n = internal_s_.n;
  const double prob = internal_s_.prob;
  const int m = internal_s_.arcs.size();
  const int edges = internal_s_.edges.size();
  const int r = internal_s_.requirements.size();

  const std::vector<vector<int> > &index = internal_s_.indexes;
  const std::vector<Requirement> &require = internal_s_.requirements;
  const std::vector<std::pair<double, std::pair<int, int> > > &arcs =
      internal_s_.arcs;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      int ind1 = index[i][j];
      int ind2 = index[j][i];
      if (ind1 != -1) {
        assert(ind2 != -1);
        x_[ind1] = x_[ind2];
      }
    }
  }

  // n-1 active edges
  GRBLinExpr ract = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      int ind1 = index[i][j];
      if (ind1 != -1) {
        ract += x_[ind1];
      }
    }
  }
  model_.addConstr(ract, GRB_EQUAL, n - 1);

  // one unit flow going in
  for (int ir = 0; ir < r; ++ir) {
    int destination = require[ir].d;

    GRBLinExpr restr = 0;
    GRBLinExpr restr2 = 0;

    for (int j = 0; j < n; ++j) {
      if (index[j][destination] >= 0) {
        restr += y_[ir][index[j][destination]];
      }
      if (index[destination][j] >= 0) {
        restr2 += y_[ir][index[destination][j]];
      }
    }
    model_.addConstr(restr, GRB_EQUAL, 1);
    model_.addConstr(restr2, GRB_EQUAL, 0);
  }
  // intermediate nodes
  for (int ir = 0; ir < r; ++ir) {
    int origen = require[ir].o;
    int destination = require[ir].d;
    for (int j = 0; j < n; ++j) {
      if (j != origen and j != destination) {
        GRBLinExpr restr = 0;
        for (int i = 0; i < n; ++i) {
          if (index[i][j] >= 0) {
            restr += y_[ir][index[i][j]];
          }
        }
        for (int k = 0; k < n; ++k) {
          if (index[j][k] >= 0) {
            restr -= y_[ir][index[j][k]];
          }
        }

        model_.addConstr(restr, GRB_EQUAL, 0);
      }
    }
  }
  // one unit flow going out
  for (int ir = 0; ir < r; ++ir) {
    int origen = require[ir].o;
    GRBLinExpr restr = 0;
    GRBLinExpr restr2 = 0;
    for (int k = 0; k < n; ++k) {
      if (index[origen][k] >= 0) {
        restr += y_[ir][index[origen][k]];
      }
      if (index[k][origen] >= 0) {
        restr2 += y_[ir][index[k][origen]];
      }
    }
    model_.addConstr(restr, GRB_EQUAL, 1);
    model_.addConstr(restr2, GRB_EQUAL, 0);
  }

  // only use used edges.
  for (int ir = 0; ir < r; ++ir) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (index[i][j] >= 0) {
          int ind1 = index[i][j];
          int ind2 = index[j][i];
          assert(ind2 >= 0);

          GRBLinExpr restr = y_[ir][ind1] + y_[ir][ind2];
          model_.addConstr(restr, GRB_LESS_EQUAL, x_[ind1]);
        }
      }
    }
  }
  GRBLinExpr objective = 0;
  for (int ir = 0; ir < r; ++ir) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (index[i][j] >= 0) {
          int ind = index[i][j];
          objective += require[ir].w * arcs[ind].first * y_[ir][ind];
        }
      }
    }
  }
  model_.setObjective(objective, GRB_MINIMIZE);
}

void PathBased::SetInitialSolution() {
  double *new_x = new double[internal_s_.arcs.size()];
  UtilSolver::GetInitialSolution(x_, &internal_s_, new_x);
  // observation: new_x solution is a tree, bfs or dfs to get the unique path
  std::vector<vector<int> > parent = UtilSolver::GetPaths(&internal_s_, new_x);
  for (int i = 0; i < internal_s_.requirements.size(); ++i) {
    auto x = internal_s_.requirements[i];
    const int s = x.o;
    int t = x.d;
    std::vector<int> &cur_parent = parent[s];
    while (t != s) {
      int p = cur_parent[t];
      int index = internal_s_.indexes[p][t];
      assert(index >= 0);
      y_[i][index].set(GRB_DoubleAttr_Start, 1);
      t = p;
    }
  }
  delete[] new_x;
}
