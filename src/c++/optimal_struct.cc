#include "optimal_struct.h"

#include <vector>

const int OptimalStruct::kInf = (1 << 29);

OptimalStruct::OptimalStruct(
    int n, std::vector<std::pair<double, std::pair<int, int>>> edges,
    std::vector<Requirement> requirements, double pr)
    : n(n), prob(pr) {
  this->max_edge_cost = 0;
  this->per_req = -1;
  this->requirements = requirements;
  this->edges = edges;
  computeLowerBound();
  fillIndexArcs(edges);
  fillWeights();
}
/*
   OptimalStruct( int n, double prob, double perReq):n(n),prob(prob){
   this->maxEdgeCost = 0;
   this->perReq = perReq;
   this->edges = generateRandomedges();

   generateRandomrequirements();
   fillIndexarcs(edges);
   fillWeights();

   }*/
void OptimalStruct::computeLowerBound() {
  min_dist = std::vector<vector<int>>(n, vector<int>(n, kInf));
  for (int i = 0; i < n; ++i) {
    min_dist[i][i] = 0;
  }

  for (int i = 0; i < edges.size(); ++i) {
    double c = edges[i].first;
    int x = edges[i].second.first;
    int y = edges[i].second.second;
    min_dist[x][y] = min_dist[y][x] = c;
  }
  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        min_dist[i][j] = min(min_dist[i][j], min_dist[i][k] + min_dist[k][j]);
      }
    }
  }
  lower_bound_ocstp = 0;
  for (int i = 0; i < requirements.size(); ++i) {
    int c = requirements[i].w;
    int x = requirements[i].o;
    int y = requirements[i].d;
    lower_bound_ocstp += c * min_dist[x][y];
  }
  // cout <<"lowerBound" << lowerBoundOCSTP << endl;
}
void OptimalStruct::fillWeights() {
  weights.resize(n);
  for (int i = 0; i < n; ++i) {
    weights[i].resize(n);
    for (int j = 0; j < n; ++j) {
      weights[i][j] = 0;
    }
  }
  for (int i = 0; i < requirements.size(); ++i) {
    weights[requirements[i].o][requirements[i].d] = requirements[i].w;
  }
}

void OptimalStruct::fillIndexArcs(
    std::vector<std::pair<double, std::pair<int, int>>> edges) {
  indexes.resize(n);
  for (int i = 0; i < n; ++i) {
    indexes[i].resize(n);
    for (int j = 0; j < n; ++j) {
      indexes[i][j] = -1;
    }
  }
  // sort( edges.begin() , edges.end() );
  arcs.clear();
  for (int i = 0; i < edges.size(); ++i) {
    double c = edges[i].first;
    int x = edges[i].second.first;
    int y = edges[i].second.second;

    max_edge_cost = max(c, max_edge_cost);
    indexes[x][y] = arcs.size();
    arcs.push_back(make_pair(c, make_pair(x, y)));
    indexes[y][x] = arcs.size();
    arcs.push_back(make_pair(c, make_pair(y, x)));
  }
  ady.resize(n);
  for (int i = 0; i < arcs.size(); ++i) {
    int x = arcs[i].second.first;
    int y = arcs[i].second.second;
    ady[x].push_back(i);
  }
#ifdef DEBUGMODE
  // print();
#endif
}
void OptimalStruct::print() {
  cout << "adyacence :v index" << endl;
  for (int i = 0; i < n; ++i) {
    cout << "index: " << i << endl;
    for (int j = 0; j < ady[i].size(); ++j) {
      cout << " " << ady[i][j];
    }
    cout << endl;
  }
  cout << "arcs" << endl;
  for (int i = 0; i < arcs.size(); ++i) {
    cout << "#" << i << " (" << arcs[i].second.first << " "
         << arcs[i].second.second << ") = " << arcs[i].first << endl;
  }
}
