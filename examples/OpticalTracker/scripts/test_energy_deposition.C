#include "CLHEP/Units/SystemOfUnits.h"

void test_energy_deposition(TString sim_file_name="sim.root") {

  // test requirements
  const Double_t min_edep = 2*CLHEP::eV;
  const Double_t max_edep = 5*CLHEP::eV;

  // get average energy deposition
  auto sim_file = new TFile(sim_file_name);
  auto t = (TTree*) sim_file->Get("EVENT");
  auto h = new TH1D("h","energy deposition",100,-1,20);
  t->Project("h","PFRICHHits.energyDeposit");
  auto ave_edep = h->GetMean();

  // check if the average energy deposition is within expected range
  bool pass_test = ave_edep > min_edep && ave_edep < max_edep;
  std::cout << "TEST: " << (pass_test ? "passed" : "failed")
    << " with average energy deposition = " << ave_edep/CLHEP::eV << " eV"
    << " (expected in range (" << min_edep/CLHEP::eV << "," << max_edep/CLHEP::eV << ") eV)"
    << std::endl;
}
