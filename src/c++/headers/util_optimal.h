#ifndef UTIL_OPTIMAL_H
#define UTIL_OPTIMAL_H

#include <bits/stdc++.h>

#include "gurobi_c++.h"
#include "optimal_struct.h"
#include "union_find.h"

using namespace std;

class UtilOptimal {
 public:
  static const double kEps;
  static double GetCommunicationCost(OptimalStruct internalS, const double *x);
  static double GetCommunicationCost(OptimalStruct internalS, const GRBVar *x);
  static void FloydWarshall(vector<vector<double> > &minD);
  static bool IsConnected(vector<pair<double, pair<int, int> > > Edges, int n);

 private:
  UtilOptimal();
};

#endif
