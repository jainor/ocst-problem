#include "ans_optimal.h"

#include <iomanip>

const std::string Ans::kSeparator = ",";
const int Ans::kPrecision = 4;
const double Ans::kEps = 1e-9;
const int knorm = 100;

std::ostream &operator<<(std::ostream &out, const Ans &cur) {
  out << "vertices" << Ans::kSeparator;
  out << "prob" << Ans::kSeparator;
  out << "edges" << Ans::kSeparator;
  out << "requirements" << Ans::kSeparator;
  out << "answer" << Ans::kSeparator;
  out << "gap" << Ans::kSeparator;
  out << "generatedNodes" << Ans::kSeparator;
  out << "time" << Ans::kSeparator;
  out << "lazyConstraints" << Ans::kSeparator;
  out << "cutsGenerated" << Ans::kSeparator;
  out << "relaxedSolution" << Ans::kSeparator;
  out << "lowerBound" << Ans::kSeparator;
  out << "upperBound" << Ans::kSeparator;
  out << "solved" << std::endl;

  out << cur.n << Ans::kSeparator;
  out << (static_cast<int>(knorm * cur.prob)) << Ans::kSeparator;
  out << cur.m << Ans::kSeparator;
  out << cur.r << Ans::kSeparator;
  out << std::fixed << std::setprecision(1);
  out << cur.answer << Ans::kSeparator;
  out << std::fixed << std::setprecision(Ans::kPrecision);
  out << cur.gap_answer << Ans::kSeparator;
  out << cur.generated_nodes << Ans::kSeparator;
  out << std::fixed << std::setprecision(Ans::kPrecision);
  out << cur.elapsed_time << Ans::kSeparator;
  out << cur.added_lazy << Ans::kSeparator;
  out << cur.added_cuts << Ans::kSeparator;
  out << cur.relaxed << Ans::kSeparator;
  out << std::fixed << std::setprecision(Ans::kPrecision);
  out << cur.lower_bound << Ans::kSeparator;
  out << std::fixed << std::setprecision(Ans::kPrecision);
  out << cur.upper_bound << Ans::kSeparator;
  out << (cur.solved ? 1 : 0) << std::endl;
  return out;
}
/*
        friend istream &operator>>( istream  &input, ans &cur ) {
            in >> cur.n;
            in >> cur.prob;
            in >> cur.m;
            in >> cur.r;
            in >> cur.answer;
            in >> cur.gapAnswer;
            in >> cur.generatedNodes;
            in >> cur.elapsedTime;
            in >> cur.addedLazy;
            in >> cur.addedCuts;
            in >> cur.relaxed;
            in >> cur.lowerBound;
            in >> cur.upperBound;
            return in;
        }
*/
