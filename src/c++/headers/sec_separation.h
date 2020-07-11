#ifndef SEC_SEPARATION_H
#define SEC_SEPARATION_H

#include <vector>

#include "separation.h"

class SECSep : public SepCallBack {
 public:
  SECSep(GRBVar *x, OptimalStruct *internalS) : SepCallBack(x, internalS) {}

 protected:
  void callback();
  void CandidateIncumbent(std::vector<std::vector<double>> &sol);
};

#endif
