#ifndef NETWORK_H
#define NETWORK_H

#include <unordered_map>
#include <utility>
#include <vector>

class Network {
 public:
  Network(
      const int n,
      const std::vector<std::pair<double, std::pair<int, int> > > arcCapacity);

  /*Edmonds–Karp algorithm*/
  double MaxFlow(int s, int t);
  std::vector<int> GetIndexArcsMinCut(int s, int t);
  std::vector<int> GetVerticesMinCut(int s, int t);
  static std::vector<int> Complement(int n, std::vector<int> indexes);
  double CapCut(std::vector<int> indexes);

 private:
  static const double kEps;
  static const double kInf;

  bool Bfs();
  double Mincap();
  void Update(double maxi);
  void InitMaxFlow(int s, int t);

  const int n_;
  const std::vector<std::pair<double, std::pair<int, int> > > arcCapacity_;

  int source_, sink_;
  std::vector<bool> visited_;
  std::vector<int> parent_;
  std::vector<std::unordered_map<int, double> > residual_;
};

#endif
