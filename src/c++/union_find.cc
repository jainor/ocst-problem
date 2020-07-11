#include "union_find.h"

UnionFind::UnionFind(int n) {
  n_ = n;
  components_ = n;
  parent_.resize(n_);
  for (int i = 0; i < n_; ++i) {
    parent_[i] = i;
  }
}

int UnionFind::Find(int t) {
  return (t == parent_[t]) ? t : (parent_[t] = Find(parent_[t]));
}

bool UnionFind::UnionSet(int u, int v) {
  int uu = Find(parent_[u]);
  int vv = Find(parent_[v]);

  if (uu == vv) {
    return false;
  }
  parent_[uu] = vv;
  this->components_--;
  return true;
}
