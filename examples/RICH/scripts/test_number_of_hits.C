void test_number_of_hits(TString sim_file_name="sim.root") {

  // test requirements
  const Double_t expected_number_of_hits = 230.0;
  const Double_t allowed_deviation       = 15.0;

  // get average number of hits
  auto sim_file = new TFile(sim_file_name);
  auto t = (TTree*) sim_file->Get("EVENT");
  auto h = new TH1D("h","<hits>",500,0,1000);
  t->Project("h","@PFRICHHits.size()");
  auto ave_hits = h->GetMean();

  // check if this is the expected number of hits
  bool pass_test = abs(ave_hits - expected_number_of_hits) < allowed_deviation;
  std::cout << "TEST: " << (pass_test ? "passed" : "failed")
    << " with average number of hits = " << ave_hits
    << " (expected " << expected_number_of_hits
    << "+/-" << allowed_deviation << ")"
    << std::endl;
}
