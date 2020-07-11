#include "util_solver.h"

#include <memory>
#include <utility>
#include <vector>

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "network.h"
#include "optimal_struct.h"
#include "sec_integer.h"
#include "solver.h"
#include "union_find.h"
#include "util_optimal.h"

int UtilSolver::GetInitialSolution(GRBVar *x, const OptimalStruct *internalS,
                                   double *new_x) {
  std::vector<std::pair<double, pair<int, int> > > arcs = internalS->arcs;
  UnionFind union_find(internalS->n);

  sort(arcs.begin(), arcs.end());
  for (int i = 0; i < internalS->arcs.size(); ++i) {
    new_x[i] = 0;
    x[i].set(GRB_DoubleAttr_Start, 0);
  }

  for (auto &value : arcs) {
    double cost = value.first;
    std::pair<int, int> e = value.second;
    int index = internalS->indexes[e.first][e.second];
    int index2 = internalS->indexes[e.second][e.first];
    if (union_find.UnionSet(e.first, e.second)) {
      // tot += c;
      x[index].set(GRB_DoubleAttr_Start, 1);
      x[index2].set(GRB_DoubleAttr_Start, 1);
      new_x[index] = 1;
      new_x[index2] = 1;
    }
  }
  assert(union_find.components() == 1);
  double currentCost = UtilOptimal::GetCommunicationCost(*internalS, new_x);
  return currentCost;
}

std::vector<vector<int> > UtilSolver::GetPaths(const OptimalStruct *internalS,
                                               const double *new_x) {
  const int n = internalS->n;
  std::vector<vector<int> > parent(
      n, std::vector<int>(n));  // we can recover path from parents

  for (int root = 0; root < n; ++root) {
    std::vector<bool> visited(n, false);
    queue<int> queue;
    queue.push(root);
    visited[root] = true;
    parent[root][root] = -1;
    while (queue.empty() == false) {
      int cur = queue.front();
      queue.pop();
      for (int i = 0; i < n; ++i) {
        int index = internalS->indexes[cur][i];
        if (index >= 0 and new_x[index] >= 0.9 and !visited[i]) {
          visited[i] = true;
          parent[root][i] = cur;
          queue.push(i);
        }
      }
    }
  }
  return parent;
}

void UtilSolver::SetInitialSolutionFlow(GRBVar *x, GRBVar **f, GRBVar **y,
                                        OptimalStruct *internalS) {
  const int n = internalS->n;
  double *new_x = new double[internalS->arcs.size()];
  GetInitialSolution(x, internalS, new_x);
  // observation: new_x solution is a tree, bfs or dfs to get the unique path
  const std::vector<vector<int> > parent = GetPaths(internalS, new_x);
  std::vector<vector<double> > flow(
      n,
      std::vector<double>(n, 0));  // first index is the root, the second is the
  // flow from its parent

  for (int i = 0; i < internalS->requirements.size(); ++i) {
    auto cur_req = internalS->requirements[i];
    const int root = cur_req.o;
    const double fl = cur_req.w;
    const std::vector<int> &cur_parent = parent[root];

    int destination = cur_req.d;

    while (destination != root) {
      flow[root][destination] += fl;
      int p = cur_parent[destination];
      int index = internalS->indexes[p][destination];
      destination = cur_parent[destination];
    }
  }
  for (int root = 0; root < n; ++root) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        int index = internalS->indexes[i][j];
        if (index >= 0) {
          if (y) {
            y[root][index].set(GRB_DoubleAttr_Start,
                               (i == parent[root][j]) ? 1 : 0);
          }
          f[root][index].set(GRB_DoubleAttr_Start,
                             (i == parent[root][j]) ? flow[root][j] : 0);
        }
      }
    }
  }
  delete[] new_x;
}
