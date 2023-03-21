void test_energy_deposition(TString sim_file_name="sim.root") {

  // test requirements
  const Double_t min_edep = 2 /*eV*/;
  const Double_t max_edep = 5 /*eV*/;

  // get average energy deposition
  auto sim_file = new TFile(sim_file_name);
  auto t = (TTree*) sim_file->Get("EVENT");
  auto h = new TH1D("h","energy deposition",100,-1,20);
  t->Project("h","PFRICHHits.energyDeposit");
  auto ave_edep = h->GetMean() * 1e6; // convert from MeV to eV

  // check if the average energy deposition is within expected range
  bool pass_test = ave_edep > min_edep && ave_edep < max_edep;
  std::cout << "TEST: " << (pass_test ? "passed" : "failed")
            << " with average energy deposition = " << ave_edep << " eV"
            << " (expected in range (" << min_edep << "," << max_edep << ") eV)"
            << std::endl;
}
