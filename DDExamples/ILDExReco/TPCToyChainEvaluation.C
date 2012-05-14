void TPCToyChainEvaluation()
{
  gStyle->SetPalette(1);

  TCanvas *can=new TCanvas("can","can");
  can->Divide(2,2);
  can->cd(1);
 
  TFile *sim=new TFile("TPCSimulation_Output.root");
  SimTree->Draw("yPos:xPos>>sim_xy(1000,-2000,2000,1000,-2000,2000)");
  SimTree->Draw("sqrt(yPos*yPos+xPos*xPos):zPos>>sim_yz(1000,-2000,2000,1000,0,2000)");
  TH2D *h_sim_xy=(TH2D*)sim->Get("sim_xy");
  TH2D *h_sim_yz=(TH2D*)sim->Get("sim_yz");


  TFile *hit=new TFile("TPCHitReco_Output.root");
  HitTree->Draw("yPos:xPos>>hit_xy(1000,-2000,2000,1000,-2000,2000)");
  HitTree->Draw("sqrt(yPos*yPos+xPos*xPos):zPos>>hit_yz(1000,-2000,2000,1000,0,2000)");
  TH2D *h_hit_xy=(TH2D*)hit->Get("hit_xy");
  TH2D *h_hit_yz=(TH2D*)hit->Get("hit_yz");


//   h_sim_xy->SetMarkerStyle(2);
//   h_sim_yz->SetMarkerStyle(2);
  h_sim_xy->SetMarkerColor(kRed);
  h_sim_yz->SetMarkerColor(kRed);
//   h_hit_xy->SetMarkerStyle(4);
//   h_hit_yz->SetMarkerStyle(4);
  h_hit_xy->SetMarkerColor(kBlue);
  h_hit_yz->SetMarkerColor(kBlue);
  

 
  h_sim_xy->Draw();
  h_hit_xy->Draw("sames");
  can->cd(2);
  h_sim_yz->Draw();
  h_hit_yz->Draw("sames");


  //track fitting
  TFile *input=new TFile("TPCHitReco_Output.root");
  int EvID_in;
  std::vector<double> *xPos=0;
  std::vector<double> *yPos=0;
  std::vector<double> *zPos=0;
  std::vector<double> *charge=0;
  TTree *data = (TTree*)gDirectory->Get("HitTree");
  // Set branch addresses.
  data->SetBranchAddress("EvID",&EvID_in);			 
  data->SetBranchAddress("xPos",&xPos) ;		 
  data->SetBranchAddress("yPos",&yPos) ;		 
  data->SetBranchAddress("zPos",&zPos) ;		 
  data->SetBranchAddress("charge",&charge) ;		 
  
  TH1F *resY=new TH1F("resY","resY",100,-10,10);
  TH1F *resR=new TH1F("resR","resR",100,-10,10);
  //Event Loop
  for(int i=0;i<data->GetEntries();i++)
    {
      //pad loop
      data->GetEntry(i);
    
      if(xPos->size()==0)
	continue;
      //assume only one track per event, no B field
      //fit a straight line in x, y and r,z
      //loop the spacepoints x,y,z,E and fill from vector into array
      double *x= new double[xPos->size()];
      double *y= new double[yPos->size()];
      double *z= new double[zPos->size()];
      double *r= new double[xPos->size()];
  
      for(int p=0;p<xPos->size();p++)
	{
	  x[p]=(*xPos)[p];
	  y[p]=(*yPos)[p];
	  z[p]=(*zPos)[p];
	  r[p]=sqrt((*xPos)[p]*(*xPos)[p]+(*yPos)[p]*(*yPos)[p]);
	}

      TGraph *g1 = new TGraph(xPos->size(), x, y);
      TF1 *f1 = new TF1("f1","pol1");
      g1->Fit("f1","QN");
      TGraph *g2 = new TGraph(xPos->size(), r, z);
      TF1 *f2 = new TF1("f2","pol1");
      g2->Fit("f2","QN");
      
      //Residuals
      for(int p=0;p<xPos->size();p++)
	{
	  resY->Fill(y[p]-f1->Eval(x[p]));
	  resR->Fill(z[p]-f2->Eval(r[p]));
	}

      delete f1;
      delete g1;
      delete f2;
      delete g2;
      delete []x;
      delete []y;
      delete []z;
      delete []r;
    }
  can->cd(3);
  resY->Draw();
  can->cd(4);
  resR->Draw();
}


