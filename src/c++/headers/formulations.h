#ifndef FORMULATIONS_H
#define FORMULATIONS_H

#include <bits/stdc++.h>

#include "flow_based_formulation.h"
#include "flow_based_relaxed.h"
#include "path_based_formulation.h"
#include "rooted_tree_formulation.h"

class Formulations {
 public:
  static Solver* GetFormulation(OptimalStruct internalS, string formulation);
  static const string kPathBasedFormulation;
  static const string kFlowBasedRelaxed;
  static const string kFlowBasedFormulation;
  static const string kRootedBasedFormulation;

 private:
  Formulations() {}
};

struct formulationNotFound : std::exception {
  const char* what() const noexcept { return "Formulation not found"; }
};

#endif
