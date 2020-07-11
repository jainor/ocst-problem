#ifndef SEPARATION_H
#define SEPARATION_H

#include <memory>
#include <utility>
#include <vector>

#include "ans_optimal.h"
#include "gurobi_c++.h"
#include "network.h"
#include "optimal_struct.h"
#include "sec_integer.h"
#include "union_find.h"
#include "util_optimal.h"

class SepCallBack : public GRBCallback {
 public:
  SepCallBack(GRBVar *x, OptimalStruct *internalS)
      : n_(internalS->n),
        m_(internalS->arcs.size()),
        r_(internalS->requirements.size()) {
    this->x_ = x;
    this->internal_s_ = internalS;
    this->cuts_ = 0;
    this->lazy_ = 0;
  }
  const int lazy() { return lazy_; }
  const int cuts() { return cuts_; }

 protected:
  virtual void callback() = 0;
  virtual void CandidateIncumbent(std::vector<vector<double>> &sol) = 0;

  // sol must be an n*n matrix filled with 0's

  void LoadXVarsRelax(std::vector<vector<double>> &sol);
  void LoadXVars(std::vector<vector<double>> &sol);

  const int n_;
  const int m_;
  const int r_;
  GRBVar *x_;
  OptimalStruct *internal_s_;
  int cuts_;
  int lazy_;
};

inline void SepCallBack::LoadXVarsRelax(std::vector<vector<double>> &sol) {
  const std::vector<std::pair<double, std::pair<int, int>>> &arcs =
      internal_s_->arcs;
  for (int i = 0; i < m_; ++i) {
    int u = arcs[i].second.first;
    int v = arcs[i].second.second;
    sol[u][v] = getNodeRel(x_[i]);
  }
}

inline void SepCallBack::LoadXVars(std::vector<vector<double>> &sol) {
  const std::vector<std::pair<double, std::pair<int, int>>> &arcs =
      internal_s_->arcs;
  double *xAns = getSolution(x_, m_);
  for (int i = 0; i < m_; ++i) {
    int u = arcs[i].second.first;
    int v = arcs[i].second.second;
    sol[u][v] = xAns[i];
  }
  delete[] xAns;
}

#endif
