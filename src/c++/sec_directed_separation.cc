#include "sec_directed_separation.h"

#include <utility>
#include <vector>

void SECDirectedSep::callback() {
  const std::vector<vector<int>> &index = internal_s_->indexes;
  const std::vector<Requirement> &require = internal_s_->requirements;
  const std::vector<std::pair<double, pair<int, int>>> &arcs =
      internal_s_->arcs;
  try {
    if (where == GRB_CB_MIPSOL) {
      std::vector<vector<double>> sol =
          std::vector<vector<double>>(n_, vector<double>(n_, 0.0));

      LoadXVars(sol);

      std::vector<int> Cycle;
      SECInteger::FindCycle(n_, sol, Cycle);

      if (not Cycle.empty()) {
        assert(false);
        int cycleSize = Cycle.size();
        assert(cycleSize >= 3);

        GRBLinExpr restr = 0;
        for (int i = 0; i < Cycle.size(); ++i) {
          int ix = Cycle[i];
          int iy = Cycle[(i + 1) % cycleSize];
          assert(index[ix][iy] != -1);
          restr += x_[index[ix][iy]];
        }
        addLazy(restr, GRB_LESS_EQUAL, cycleSize - 1);
        ++lazy_;
      }

    } else if (where == GRB_CB_MIPNODE and
               getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL) {
      std::vector<vector<double>> sol =
          std::vector<vector<double>>(n_, vector<double>(n_, 0.0));

      LoadXVarsRelax(sol);

      for (int i = 0; i < n_; ++i) {
        for (int j = 0; j < i; ++j) {
          if (index[i][j] >= 0) {
            double tmpSol = sol[i][j] + sol[j][i];
            sol[i][j] = sol[j][i] = tmpSol;
          }
        }
      }

      std::vector<std::pair<double, pair<int, int>>> resultCapacity = arcs;
      for (int i = 0; i < m_; ++i) {
        resultCapacity[i].first =
            sol[arcs[i].second.first][arcs[i].second.second];
      }

      Network CSInetwork = Network(n_, resultCapacity);
      /*SEC network*/
      std::vector<double> deltaJ(n_, 0.0);
      double totcap = 0;
      for (int j = 0; j < n_; ++j) {
        double cap = 0;
        for (int k = 0; k < n_; ++k) {
          if (j != k) {
            assert(sol[j][k] == sol[k][j]);
            const int indjk = index[j][k];
            if (indjk != -1) {
              assert(j == arcs[indjk].second.first);
              assert(k == arcs[indjk].second.second);
              // assert( fabs(sol[j][k] - xAns[indjk]) < eps);
              cap += sol[j][k];
            }
          }
        }
        deltaJ[j] = cap;
        totcap += cap;
      }
      assert(fabs(totcap - (2 * n_ - 2)) < UtilOptimal::kEps);
      /**/
      bool cutCSI = false;
      bool cutSEC = false;

      int rFixed = 0;
      std::vector<int> indexCSSI;
      for (int rConfig = 0; rConfig < 2; ++rConfig) {
        int s;
        int t;
        for (int j = 0; j < n_; ++j) {
          if (j != rFixed) {
            if (rConfig == 0) {
              s = rFixed;
              t = j;
            } else {
              s = j;
              t = rFixed;
            }

            double maxFlow = CSInetwork.MaxFlow(s, t);
            // CSI constraints
            if (maxFlow + UtilOptimal::kEps < 1.0) {
              cutCSI = true;
              std::vector<int> indexes = CSInetwork.GetIndexArcsMinCut(s, t);
              assert(fabs(maxFlow - CSInetwork.CapCut(
                                        CSInetwork.GetVerticesMinCut(s, t))) <
                     UtilOptimal::kEps);
              GRBLinExpr restr = 0;

              for (int i = 0; i < indexes.size(); ++i) {
                assert(indexes[i] >= 0 and indexes[i] < m_);
                int p = arcs[indexes[i]].second.first;
                int q = arcs[indexes[i]].second.second;
                restr += x_[index[p][q]] + x_[index[q][p]];
              }
              addCut(restr, GRB_GREATER_EQUAL, 1);
            }

            // SEC
            std::vector<std::pair<double, pair<int, int>>> capSEC;
            const double kNormalize = 0.5;

            for (int si = 0; si < n_; ++si) {
              for (int sj = 0; sj < n_; ++sj) {
                if (si != sj) {
                  const int indij = index[si][sj];
                  if (indij != -1) {
                    capSEC.push_back(
                        make_pair(sol[si][sj] * kNormalize, make_pair(si, sj)));
                  }
                }
              }
            }
            for (int ind = 0; ind < n_; ++ind) {
              if (ind != t) {
                capSEC.push_back(make_pair(1, make_pair(ind, t)));
              }
              if (ind != s) {
                capSEC.push_back(
                    make_pair(deltaJ[ind] * kNormalize, make_pair(s, ind)));
              }
            }

            Network SECnetwork(n_, capSEC);
            // assert( SECnetwork.GetVerticesMinCut(s,t).size() > 1);
            double minCutSEC = SECnetwork.MaxFlow(s, t);
            // cout <<"min cut SEC " << minCutSEC << endl;

            assert(minCutSEC <= (double)n_ + UtilOptimal::kEps);
            if (minCutSEC + UtilOptimal::kEps < n_) {
              std::vector<int> verticesS = SECnetwork.GetVerticesMinCut(s, t);
              const int sizeS = verticesS.size();
              // cout << sizeS <<endl;
              // observe that we need just one of the two arcs :v {i,j} =
              // (i,j) (j,i)
              GRBLinExpr restr = 0;
              double sumS = 0;
              for (int id = 0; id < sizeS; ++id) {
                for (int jid = 0; jid < sizeS; ++jid) {
                  if (id != jid) {
                    const int p = verticesS[id];
                    const int q = verticesS[jid];
                    const int indpq = index[p][q];
                    if (indpq != -1) {
                      restr += x_[indpq];
                      sumS += sol[p][q];
                    }
                  }
                }
              }
              addCut(restr, GRB_LESS_EQUAL, sizeS - 1);
              // cout << s <<" -  "<< t <<" "<< sumS <<" size "<< sizeS <<"
              // mincut " << minCutSEC<<  endl;
              assert(sizeS > 1);
              assert(sumS > sizeS - 1);
              cutSEC = true;
            }
          }
        }
      }
      assert((!cutCSI) or cutSEC);  // SEC \subset CSI
      if (cutSEC) {
        ++cuts_;
      }
      CandidateIncumbent(sol);
    }
  } catch (GRBException e) {
    cout << "Error number: " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during callback" << endl;
  }
}

void SECDirectedSep::CandidateIncumbent(std::vector<vector<double>> &sol) {
  const std::vector<vector<int>> &index = internal_s_->indexes;
  const std::vector<Requirement> &require = internal_s_->requirements;
  const std::vector<std::pair<double, pair<int, int>>> &arcs =
      internal_s_->arcs;

  double *newX = new double[m_];
  UnionFind uF(internal_s_->n);

  std::vector<std::pair<double, pair<int, int>>> V;

  for (int i = 0; i < m_; ++i) {
    int u = arcs[i].second.first;
    int v = arcs[i].second.second;
    assert(index[u][v] == i);
    // assert( sol[u][v] <= 1.0 and sol[u][v] >= 0);
    V.push_back(make_pair(sol[u][v], make_pair(u, v)));
    newX[i] = 0;
  }

  random_shuffle(V.rbegin(), V.rend());
  int added = 0;
  for (int i = 0; i < m_ and uF.components() > 1; ++i) {
    int u = V[i].second.first;
    int v = V[i].second.second;
    if (uF.UnionSet(u, v)) {
      added++;
      newX[index[u][v]] = 1.0;
    }
  }

  assert(added == n_ - 1);
  double currentCost = UtilOptimal::GetCommunicationCost(*internal_s_, newX);

  double prevAns = getDoubleInfo(GRB_CB_MIPNODE_OBJBND);

  if (currentCost < prevAns - 1) {
    cout << currentCost << endl;
  }
  setSolution(x_, newX, m_);
  useSolution();

  double currentAns = getDoubleInfo(GRB_CB_MIPNODE_OBJBND);
  assert(currentCost >= currentAns);

  // end
  delete[] newX;
}
