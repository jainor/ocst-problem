#include "sec_integer.h"

#include <stack>
#include <vector>

/*
   Integer separation
*/

const double kMinVal = 0.5;  // it should be call with value equal to 1.

bool SECInteger::FindCycle(const int n, const int current, const int parent,
                           const std::vector<std::vector<double> >& x,
                           std::stack<int>& cycle, std::vector<bool>& visited) {
  visited[current] = true;
  cycle.push(current);
  for (int i = 0; i < n; ++i) {
    if (i != parent and x[current][i] > kMinVal) {
      if (visited[i]) {
        cycle.push(i);
        return true;
      }
      if (FindCycle(n, i, current, x, cycle, visited)) {
        return true;
      }
    }
  }

  cycle.pop();
  return false;
}

void SECInteger::FindCycle(const int n,
                           const std::vector<std::vector<double> >& x,
                           std::vector<int>& cycle) {
  std::vector<bool> visited(n, false);
  std::stack<int> stack;
  for (int i = 0; i < n; ++i) {
    if (!visited[i] and FindCycle(n, i, -1, x, stack, visited)) {
      int last = stack.top();
      stack.pop();
      cycle.push_back(last);
      while (last != stack.top()) {
        cycle.push_back(stack.top());
        stack.pop();
      }
      return;
    }
  }
}
