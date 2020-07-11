#ifndef ANS_OPTIMAL_H
#define ANS_OPTIMAL_H

#include <cmath>
#include <fstream>

struct Ans {
 public:
  static const std::string kSeparator;
  static const int kPrecision;
  static const double kEps;

  Ans(int n, int m, int r, double prob, double answer, int generated_nodes,
      double elapsed_time, double gap_answer, int added_lazy = 0,
      int added_cuts = 0, double relaxed = -1, double lower_bound = -1,
      double upper_bound = -1)
      : n(n), m(m), r(r), prob(prob) {
    this->answer = answer;
    this->generated_nodes = generated_nodes;
    this->elapsed_time = elapsed_time;
    this->gap_answer = gap_answer;
    this->added_lazy = added_lazy;
    this->added_cuts = added_cuts;
    this->relaxed = relaxed;
    this->lower_bound = lower_bound;
    this->upper_bound = upper_bound;
    this->solved = (fabs(upper_bound - lower_bound) < kEps);
  }

  friend std::ostream &operator<<(std::ostream &out, const Ans &cur);
  /*
     friend istream &operator>>( istream  &input, Ans &cur ) {
     }
   */
  const int n;
  const double prob;
  const int m;
  const int r;
  double answer;
  int generated_nodes;
  int added_lazy;
  int added_cuts;
  double relaxed;
  double lower_bound;
  double upper_bound;

  double elapsed_time;
  double gap_answer;
  bool solved;
};

#endif
