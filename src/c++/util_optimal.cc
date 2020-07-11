#include "util_optimal.h"

#include <utility>
#include <vector>

#include "optimal_struct.h"
#include "solver.h"
#include "union_find.h"

const double UtilOptimal::kEps = 1e-5;
const double kMinVal = 0.01;

double UtilOptimal::GetCommunicationCost(OptimalStruct internalS,
                                         const GRBVar *x) {
  const int m = internalS.arcs.size();
  double *value = new double[m];
  for (int i = 0; i < m; ++i) {
    value[i] = x[i].get(GRB_DoubleAttr_X);
  }
  return GetCommunicationCost(internalS, value);
}

void UtilOptimal::FloydWarshall(std::vector<vector<double> > &min_dist) {
  const int n = min_dist.size();
  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        min_dist[i][j] = min(min_dist[i][j], min_dist[i][k] + min_dist[k][j]);
      }
    }
  }
}

double UtilOptimal::GetCommunicationCost(OptimalStruct internalS,
                                         const double *x) {
  const int n = internalS.n;
  const std::vector<vector<int> > &index = internalS.indexes;
  const std::vector<vector<int> > &W = internalS.weights;
  const std::vector<Requirement> &require = internalS.requirements;
  const std::vector<std::pair<double, std::pair<int, int> > > &arcs =
      internalS.arcs;
  const int m = arcs.size();

  std::vector<vector<double> > min_dist(n, vector<double>(n, Solver::kInf));
  for (int i = 0; i < n; ++i) {
    min_dist[i][i] = 0.0;
  }

  int edges = 0;
  for (int i = 0; i < m; ++i) {
    double value = x[i];
    if (value > kMinVal) {
      int u = arcs[i].second.first;
      int v = arcs[i].second.second;
      int c = arcs[i].first;
      min_dist[u][v] = c;
      min_dist[v][u] = c;  // not redundant for rooted tree formulation
      ++edges;
    }
  }
  assert(edges == 2 * (n - 1) || edges == (n - 1));

  FloydWarshall(min_dist);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      assert(min_dist[i][j] <
             Solver::kInf);  // exist a path if and only if distance is
    }                        // different from Solver::kInf
  }

  double communicationcost = 0;
  for (int i = 0; i < require.size(); ++i) {
    communicationcost += require[i].w * min_dist[require[i].o][require[i].d];
  }
  return communicationcost;
}

bool UtilOptimal::IsConnected(
    std::vector<std::pair<double, std::pair<int, int> > > edges, int n) {
  UnionFind union_find(n);

  for (auto &x : edges) {
    std::pair<int, int> e = x.second;
    union_find.UnionSet(e.first, e.second);
  }
  return union_find.components() == 1;
}
