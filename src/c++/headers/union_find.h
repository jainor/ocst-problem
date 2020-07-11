#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>

class UnionFind {
 public:
  explicit UnionFind(int n);
  int Find(int t);
  bool UnionSet(int u, int v);
  int components() const { return components_; }

 private:
  std::vector<int> parent_;
  int n_;
  int components_;
};

#endif
