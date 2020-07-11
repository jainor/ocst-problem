#include "rooted_tree_formulation.h"

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "network.h"
#include "optimal_struct.h"
#include "sec_directed_separation.h"
#include "sec_integer.h"
#include "sec_separation.h"
#include "solver.h"
#include "util_optimal.h"
#include "util_solver.h"

Ans RootedBased::Solve(int timeLimit, double heuristics) {
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

    x_ = model_.addVars(m, GRB_BINARY);
    p_ = new GRBVar *[n];
    d_ = new GRBVar *[n];

    for (int i = 0; i < n; ++i) {
      p_[i] = model_.addVars(n, GRB_CONTINUOUS);
      d_[i] = model_.addVars(n, GRB_CONTINUOUS);
    }

    AddConstraintsAndObjective();
    SetInitialSolution();

    SECDirectedSep cb = SECDirectedSep(x_, &internal_s_);
    model_.setCallback(&cb);

    model_.optimize();

    const int generated_nodes = model_.get(GRB_DoubleAttr_NodeCount);
    const double elapsed_time = model_.get(GRB_DoubleAttr_Runtime);
    const int lazy = cb.lazy();
    const int cuts = cb.cuts();

    // relax the current model_
    double relaxed = 0;
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

    double objval = kInf;

    if (optimstatus == GRB_OPTIMAL) {
      objval = model_.get(GRB_DoubleAttr_ObjVal);
    } else if (optimstatus == GRB_TIME_LIMIT) {
      objval = Solver::GetUpper(model_);
    } else {
      cout << "Optimization was stopped with status = " << optimstatus << endl;
      assert(false);
    }

    delete[] p_;
    delete[] d_;

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

void RootedBased::AddConstraintsAndObjective() {
  const int n = internal_s_.n;
  const int m = internal_s_.arcs.size();
  const double prob = internal_s_.prob;
  const int edges = internal_s_.edges.size();
  const int r = internal_s_.requirements.size();

  const int M = internal_s_.max_edge_cost * (n - 1);

  const std::vector<vector<int> > &index = internal_s_.indexes;
  const std::vector<vector<int> > &W = internal_s_.weights;
  const std::vector<Requirement> &require = internal_s_.requirements;
  const std::vector<std::pair<double, std::pair<int, int> > > &arcs =
      internal_s_.arcs;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      d_[i][j] = d_[j][i];
    }
    model_.addConstr(d_[i][i], GRB_EQUAL, 0);
  }

  // Restriction incoming edges equal to 1
  for (int i = 0; i < n; ++i) {
    GRBLinExpr restr = 0;
    for (int j = 0; j < n; ++j) {
      if (index[j][i] >= 0) {
        restr += x_[index[j][i]];
      }
    }
    model_.addConstr(restr, GRB_EQUAL, (i != kRoot) ? 1 : 0);
  }
  // Xij <= Pij
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (index[i][j] >= 0) {
        model_.addConstr(x_[index[i][j]], GRB_LESS_EQUAL, p_[i][j]);
      }
    }
  }
  // Pij + Pji <= 1
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      GRBLinExpr restr = p_[j][i] + p_[i][j];
      model_.addConstr(restr, GRB_LESS_EQUAL, 1);
    }
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      for (int k = 0; k < n; ++k) {
        if (i != j and j != k and i != k) {
          GRBLinExpr zero = 0;
          /*
          //p ij + x jk <= 1 + p ik
          GRBLinExpr restr1 = p[i][j];
          if( index[j][k] >= 0 ) restr1 += x[ index[j][k]];

          GRBLinExpr restr2 = 1 + p[i][k];
          model_.addConstr(restr1 ,GRB_LESS_EQUAL, restr2);

          //p ik + x jk ≤ 1 + p ij
          GRBLinExpr restr3 = p[i][k];
          if( index[j][k] >= 0 ) restr3 = restr3 +  x[index[j][k]];

          GRBLinExpr restr4 = 1 + p[i][j];
          model_.addConstr(restr3 ,GRB_LESS_EQUAL, restr4);
           */

          // Formulation 2
          // p ij + p jk + xkj  ≤ 1 + p ik
          GRBLinExpr restr9 = p_[i][j] + p_[j][k];
          if (index[k][j] >= 0) {
            restr9 += x_[index[k][j]];
          }

          GRBLinExpr restr10 = 1 + p_[i][k];
          model_.addConstr(restr9, GRB_LESS_EQUAL, restr10);

          // D ik + C kj − M (2 − X kj − P ik ) ≤ D ij

          GRBLinExpr restr5 =
              d_[i][k] + (index[k][j] >= 0 ? arcs[index[k][j]].first : 0) -
              M * (2 - (index[k][j] >= 0 ? x_[index[k][j]] : zero) - p_[i][k]);
          GRBLinExpr restr6 = d_[i][j];
          model_.addConstr(restr5, GRB_LESS_EQUAL, restr6);

          // d ik + c kj − M (1 − x kj + p ij + p ji ) <= d ij
          GRBLinExpr restr7 =
              d_[i][k] + (index[k][j] >= 0 ? arcs[index[k][j]].first : 0) -
              M * (1 - (index[k][j] >= 0 ? x_[index[k][j]] : zero) + p_[i][j] +
                   p_[j][i]);
          GRBLinExpr restr8 = d_[i][j];
          model_.addConstr(restr7, GRB_LESS_EQUAL, restr8);
        }
      }
    }
  }

  // d ij ≥ c ij (x ij + x ji )
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      if (index[i][j] >= 0) {
        GRBLinExpr restr = x_[index[j][i]] + x_[index[i][j]];
        model_.addConstr(arcs[index[i][j]].first * restr, GRB_LESS_EQUAL,
                         d_[i][j]);
      }
      model_.addConstr(internal_s_.min_dist[i][j], GRB_LESS_EQUAL, d_[i][j]);
    }
  }

  GRBLinExpr objective = 0;
  for (int i = 0; i < r; ++i) {
    objective += require[i].w * d_[require[i].o][require[i].d];
  }
  model_.setObjective(objective, GRB_MINIMIZE);
}

void RootedBased::SetInitialSolution() {
  int n = internal_s_.n;
  std::vector<std::pair<double, std::pair<int, int> > > arcs = internal_s_.arcs;
  UnionFind UF(internal_s_.n);
  double *new_x = new double[internal_s_.arcs.size()];
  UtilSolver::GetInitialSolution(x_, &internal_s_, new_x);

  std::vector<vector<double> > dist(n, std::vector<double>(n, Solver::kInf));
  for (int i = 0; i < n; ++i) {
    dist[i][i] = 0;
  }

  for (int i = 0; i < internal_s_.arcs.size(); ++i) {
    if (new_x[i] > 0.1) {
      int xx = internal_s_.arcs[i].second.first;
      int yy = internal_s_.arcs[i].second.second;
      dist[xx][yy] = internal_s_.arcs[i].first;
    }
  }

  UtilOptimal::FloydWarshall(dist);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      assert(dist[i][j] != Solver::kInf);
      d_[i][j].set(GRB_DoubleAttr_Start, dist[i][j]);
    }
  }

  for (int i = 0; i < internal_s_.arcs.size(); ++i) {
    x_[i].set(GRB_DoubleAttr_Start, 0);
  }
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      p_[i][j].set(GRB_DoubleAttr_Start, 0);
    }
  }

  std::vector<vector<int> > parents = UtilSolver::GetPaths(&internal_s_, new_x);
  std::vector<int> &curParent = parents[kRoot];
  for (int vertex = 0; vertex < n; ++vertex) {
    if (kRoot != vertex) {
      int current = vertex;

      while (current != kRoot) {
        const int parent_current = curParent[current];
        p_[parent_current][vertex].set(GRB_DoubleAttr_Start, 1);
        current = parent_current;
      }
      int index = internal_s_.indexes[curParent[vertex]][vertex];
      assert(index >= 0);
      x_[index].set(GRB_DoubleAttr_Start, 1);
    }
  }

  double current_cost = UtilOptimal::GetCommunicationCost(internal_s_, new_x);
  delete[] new_x;
}
