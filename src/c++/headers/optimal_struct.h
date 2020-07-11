#ifndef OPTIMAL_STRUCT_H
#define OPTIMAL_STRUCT_H

#include <bits/stdc++.h>

using namespace std;

#define MAXN 500
#define DEBUGMODE

struct Requirement {
  int o, d, w;
  Requirement(int o, int d, int w) : o(o), d(d), w(w) {}
  Requirement() {}
};

struct OptimalStruct {
 public:
  static const int kInf;

  OptimalStruct() {}

  OptimalStruct(int n, vector<pair<double, pair<int, int>>> edges,
                vector<Requirement> requirements, double pr);

  void computeLowerBound();
  void fillWeights();
  void fillIndexArcs(vector<pair<double, pair<int, int>>> edges);
  void print();

  int n;
  double prob;
  int per_req;
  vector<pair<double, pair<int, int>>> arcs;
  vector<Requirement> requirements;
  vector<vector<int>> ady;
  vector<pair<double, pair<int, int>>> edges;
  vector<vector<int>> indexes;  // O(1)
  vector<vector<int>> weights;
  double max_edge_cost;
  double lower_bound_ocstp;
  vector<vector<int>> min_dist;
};

#endif
