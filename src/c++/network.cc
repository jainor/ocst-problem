#include "network.h"

#include "util_optimal.h"

bool Network::Bfs() {
  queue<int> q;
  q.push(source_);
  visited_[source_] = true;

  while (!q.empty()) {
    int node = q.front();
    q.pop();
    if (node == sink_) {
      return true;
    }
    for (auto it : residual_[node]) {
      if (it.second > kEps && !visited_[it.first]) {
        parent_[it.first] = node;
        visited_[it.first] = true;
        q.push(it.first);
      }
    }
  }
  return false;
}

double Network::Mincap() {
  double maxi = kInf;
  int cur = sink_;
  while (cur != source_) {
    maxi = min(maxi, residual_[parent_[cur]][cur]);
    cur = parent_[cur];
  }
  return maxi;
}

void Network::Update(double maxi) {
  int cur = sink_;
  while (cur != source_) {
    residual_[parent_[cur]][cur] -= maxi;
    residual_[cur][parent_[cur]] += maxi;
    cur = parent_[cur];
  }
}

void Network::InitMaxFlow(int s, int t) {
  this->source_ = s;
  this->sink_ = t;

  this->visited_ = std::vector<bool>(n_ + 1, false);
  this->parent_ = std::vector<int>(n_ + 1, -1);
  this->residual_.resize(n_);
  residual_.clear();
  residual_.resize(n_);

  for (auto it : arcCapacity_) {
    double cost = it.first;
    int i = it.second.first;
    int j = it.second.second;
    residual_[i][j] += cost;  // multiple arcs are allowed, they are combined
  }
}

Network::Network(
    const int n,
    const std::vector<std::pair<double, std::pair<int, int> > > arcCapacity)
    : n_(n), arcCapacity_(arcCapacity) {
  this->visited_ = std::vector<bool>(n + 1, false);
  this->parent_ = std::vector<int>(n + 1, -1);
  this->residual_.resize(n);
  source_ = sink_ = -1;
}
/*    static Network createNetwork( int n, std::vector< std::pair< double,
pair<int,int> > > arcs, const std::vector<std::pair<int,double> >
&indexCapacity){
// rewrite over arcs :v
foreach(indexCapacity,it){
int index = it->first;
double cost  = it->second;
arcs[ index ].first = cost;
}
return Network(n,arcs);
}*/

double Network::MaxFlow(int s, int t) {
  InitMaxFlow(s, t);

  double max_flow = 0;

  while (Bfs()) {
    double mf = Mincap();
    Update(mf);
    if (mf < kEps) {
      break;
    }
    max_flow += mf;
    fill(visited_.begin(), visited_.end(), false);
    fill(parent_.begin(), parent_.end(), -1);
  }
  return max_flow;
}

std::vector<int> Network::GetIndexArcsMinCut(int s, int t) {
  double max_flow = MaxFlow(s, t);
  std::vector<int> index_arcs;
  double cap_min_cut = 0;
  for (int index = 0; index < arcCapacity_.size(); ++index) {
    int i = arcCapacity_[index].second.first;
    int j = arcCapacity_[index].second.second;
    if (visited_[i] and (!visited_[j])) {
      cap_min_cut += arcCapacity_[index].first;
      index_arcs.push_back(index);
    }
  }
  assert(fabs(cap_min_cut - max_flow) <
         UtilOptimal::kEps);  // max-flow min-cut theorem :v
  return index_arcs;
}

std::vector<int> Network::GetVerticesMinCut(int s, int t) {
  double max_flow = MaxFlow(s, t);
  std::vector<int> vertices;
  for (int i = 0; i < n_; ++i) {
    if (visited_[i]) {
      vertices.push_back(i);
    }
  }
  Complement(n_, vertices);
  assert(fabs(CapCut(vertices) - max_flow) < UtilOptimal::kEps);
  return vertices;
}

std::vector<int> Network::Complement(int n, vector<int> indexes) {
  std::vector<bool> inS(n, false);  // S is the min s-t cut, s in S
  for (auto it : indexes) {
    assert(it >= 0 and it < n);
    inS[it] = true;
  }
  std::vector<int> comp;
  for (int i = 0; i < n; ++i) {
    if (!inS[i]) {
      comp.push_back(i);
    }
  }
  assert(indexes.size() + comp.size() == n);
  return comp;
}

double Network::CapCut(std::vector<int> indexes) {
  std::vector<bool> set_s(n_, false);
  for (auto it : indexes) {
    assert(it >= 0 and it < n_);
    set_s[it] = true;
  }

  double cap = 0;
  for (auto it : arcCapacity_) {
    int p = it.second.first;
    int q = it.second.second;

    if (set_s[p] and (!set_s[q])) {
      cap += it.first;
    }
  }
  return cap;
}

const double Network::kInf = 1e10;
const double Network::kEps = 1e-7;
