#ifndef SEC_INTEGER_H
#define SEC_INTEGER_H

#include <stack>
#include <vector>
/*
   Integer separation
*/

class SECInteger {
 public:
  static bool FindCycle(const int n, const int current, const int parent,
                        const std::vector<std::vector<double> >& x,
                        std::stack<int>& Cycle, std::vector<bool>& visited);
  static void FindCycle(const int n, const std::vector<std::vector<double> >& x,
                        std::vector<int>& Cycle);

 private:
  SECInteger() {}
};

#endif
