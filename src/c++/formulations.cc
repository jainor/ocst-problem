#include "formulations.h"

const string Formulations::kPathBasedFormulation = "pathbased";
const string Formulations::kFlowBasedRelaxed = "flowbasedrelaxed";
const string Formulations::kFlowBasedFormulation = "flowbased";
const string Formulations::kRootedBasedFormulation = "rootedbased";

Solver *Formulations::GetFormulation(OptimalStruct internalS,
                                     string formulation) {
  Solver *ptr = NULL;
  if (formulation == kPathBasedFormulation) {
    ptr = new PathBased(internalS);
  } else if (formulation == kFlowBasedRelaxed) {
    ptr = new FlowBasedRelaxed(internalS);
  } else if (formulation == kFlowBasedFormulation) {
    ptr = new FlowBased(internalS);
  } else if (formulation == kRootedBasedFormulation) {
    ptr = new RootedBased(internalS);
  } else {
    throw formulationNotFound();
  }
  return ptr;
}
