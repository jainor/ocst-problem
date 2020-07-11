#include "formulations.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include "gtest/gtest.h"
#include "optimal_solver.h"
#include "read_instance.h"
#include "util_optimal.h"

class FormulationTest : public ::testing::Test {
 public:
  static const std::string kTestPath;
  static const double kEps;
  static const int kTimeLimit;
  static const double kheuristics;
  static const std::string kExtensionOutput;

  const int kprocessor_count = std::thread::hardware_concurrency();

  std::vector<std::string> files_input, all_formulations_ocst;

  FormulationTest() {}
  ~FormulationTest() override {}

  void SetUp() override {
    files_input = {
        "orst0",     "orst1",     "orst2",     "orst3",     "orst4",
        "orst5",     "orst6",     "orst7",     "orst8",     "orst9",
        "orstBig0",  //"orstBig1","orstBig2",
        "ocstpin0",  "ocstpin1",  "ocstpin2",  "ocstpin3",  "ocstpin4",
        "ocstpin5",  "ocstpin6",  "ocstpin7",  "ocstpin8",  "ocstpin9",
        "ocstpin10", "ocstpin11", "ocstpin12", "ocstpin13", "ocstpin14",
    };

    all_formulations_ocst = {Formulations::kPathBasedFormulation,
                             Formulations::kFlowBasedRelaxed,
                             Formulations::kFlowBasedFormulation,
                             Formulations::kRootedBasedFormulation};
  }

  void TearDown() override {}

  double GetSolution(std::string input_file) {
    std::string output_file = input_file + kExtensionOutput;
    ifstream input_stream;
    input_stream.open(output_file);
    double result;
    input_stream >> result;
    input_stream.close();
    return result;
  }

  void Execute(std::string formulation, std::string file_input) {
    const std::string cur_file = kTestPath + "/" + file_input;
    double solution = GetSolution(cur_file);

    ifstream input_stream;
    input_stream.open(cur_file);
    auto solver = InstanceSolver::GetInstance(input_stream, formulation);
    auto result = solver.Solve("", kTimeLimit, kheuristics);

    // in some cases, the rooted tree formulation takes a long time to run.
    // if (formulation == Formulations::krooted_based_formulation and
    if (result.elapsed_time >= kTimeLimit) {  // time limit reached
      EXPECT_TRUE(result.upper_bound >= solution - kEps)
          << "Time Limit, Fails on " << file_input << " " << formulation
          << " :: " << result.upper_bound << " >= " << solution << endl;
      SUCCEED();
      return;
    }
    EXPECT_NEAR(solution, result.answer, kEps)
        << "Fails on " << formulation << " " << file_input << endl;
    input_stream.close();
  }

  void ExecuteFormulation(std::string formulation) {
    EXPECT_TRUE(find(all_formulations_ocst.begin(), all_formulations_ocst.end(),
                     formulation) != all_formulations_ocst.end());
    for (int i = 0; i < files_input.size(); ++i) {
      Execute(formulation, files_input[i]);
    }
  }
};

TEST_F(FormulationTest, ConnectedInputs) {
  for (int i = 0; i < files_input.size(); ++i) {
    ifstream input_stream;
    input_stream.open(kTestPath + "/" + files_input[i]);
    OptimalStruct graphstruct = ReaderOCST::ReadInstance(input_stream);
    input_stream.close();
    EXPECT_TRUE(UtilOptimal::IsConnected(graphstruct.edges, graphstruct.n));
  }
}

TEST_F(FormulationTest, ExecuteFormulationsParallel) {
  std::vector<std::thread> formulation_threads;
  for (std::string formulation : all_formulations_ocst) {
    // we execute one thread per formulation
    formulation_threads.push_back(
        std::thread(&FormulationTest::ExecuteFormulation, this, formulation));
  }
  for (auto& thread : formulation_threads) {
    thread.join();
  }
}

const std::string FormulationTest::kTestPath = "../src/test/TestInstances";
const double FormulationTest::kEps = 0.1;
const int FormulationTest::kTimeLimit = 20;
const double FormulationTest::kheuristics = 0.01;
const std::string FormulationTest::kExtensionOutput = ".sol";

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
