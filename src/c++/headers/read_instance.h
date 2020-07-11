#ifndef READ_INSTANCE_H
#define READ_INSTANCE_H

#include "optimal_struct.h"
#include "solver.h"

// Facade Dessign Pattern
class OCSTProblem {};

class ReaderOCST {
 private:
  ReaderOCST() {}

 public:
  static OptimalStruct ReadInstance(std::istream& in);
  static OptimalStruct ReadInstance(string filename);
};

OptimalStruct ReaderOCST::ReadInstance(std::istream& in) {
  int n, m, r;
  double pr;
  in >> n >> m;
  in >> pr;

  vector<pair<double, pair<int, int>>> Edges;
  vector<Requirement> requirements;
  for (int i = 0; i < m; ++i) {
    int a, b;
    double c;
    in >> a >> b >> c;
    Edges.push_back(make_pair(c, make_pair(a, b)));
  }
  in >> r;
  for (int i = 0; i < r; ++i) {
    int o, d, w;
    in >> o >> d >> w;
    requirements.push_back(Requirement(o, d, w));
  }
  return OptimalStruct(n, Edges, requirements, pr);
}

OptimalStruct ReaderOCST::ReadInstance(string filename) {
  ifstream myfile;
  myfile.open(filename);
  return ReaderOCST::ReadInstance(myfile);
}

#endif
