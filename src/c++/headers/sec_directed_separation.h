#ifndef SEC_DIRECTED_SEPARATION_H
#define SEC_DIRECTED_SEPARATION_H

#include <vector>

#include "separation.h"

class SECDirectedSep : public SepCallBack {
 protected:
  void callback();
  void CandidateIncumbent(std::vector<std::vector<double>> &sol);

 public:
  SECDirectedSep(GRBVar *x, OptimalStruct *internalS)
      : SepCallBack(x, internalS) {}
};

#endif
