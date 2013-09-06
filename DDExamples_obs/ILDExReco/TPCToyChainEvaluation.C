void TPCToyChainEvaluation()
{
  //Simple script to evaluate the output from
  //TPC toy chain:
  //Steps: 
  // ./DDExamples/ILDExReco/TPCToySimulation file:../DDExamples/ILDExDet/compact/ILDExTPC.xml ( writes TPCSimulation_Output.root )
  // ./DDExamples/ILDExReco/TPCDigitisation file:../DDExamples/ILDExDet/compact/ILDExTPC.xml ( writes TPCDigitisation_Output.root )
  // ./DDExamples/ILDExReco/TPCHitReco file:../DDExamples/ILDExDet/compact/ILDExTPC.xml ( writes TPCHitReco_Output.root )
  
  gStyle->SetPalette(1);

  TCanvas *can=new TCanvas("can","can",700,1200);
  can->Divide(2,3);
  can->cd(1);
 
  TFile *sim=new TFile("TPCSimulation_Output.root");
  SimTree->Draw("yPos:xPos>>sim_xyEP1(1000,-2000,2000,1000,-2000,2000)","zPos>0");
  SimTree->Draw("yPos:xPos>>sim_xyEP2(1000,-2000,2000,1000,-2000,2000)","zPos<0");
  SimTree->Draw("sqrt(yPos*yPos+xPos*xPos):zPos>>sim_yz(1000,-2500,2500,1000,0,2000)");
  TH2D *h_sim_xyEP1=(TH2D*)sim->Get("sim_xyEP1");
  TH2D *h_sim_xyEP2=(TH2D*)sim->Get("sim_xyEP2");
  TH2D *h_sim_yz=(TH2D*)sim->Get("sim_yz");


  TFile *hit=new TFile("TPCHitReco_Output.root");
  HitTree->Draw("yPos:xPos>>hit_xyEP1(1000,-2000,2000,1000,-2000,2000)","zPos>0");
  HitTree->Draw("yPos:xPos>>hit_xyEP2(1000,-2000,2000,1000,-2000,2000)","zPos<0");
  HitTree->Draw("sqrt(yPos*yPos+xPos*xPos):zPos>>hit_yz(1000,-2500,2500,1000,0,2000)");
  TH2D *h_hit_xyEP1=(TH2D*)hit->Get("hit_xyEP1");
  TH2D *h_hit_xyEP2=(TH2D*)hit->Get("hit_xyEP2");
  TH2D *h_hit_yz=(TH2D*)hit->Get("hit_yz");


//   h_sim_xy->SetMarkerStyle(2);
//   h_sim_yz->SetMarkerStyle(2);
  h_sim_xyEP1->SetTitle("positive Endplate");
  h_sim_xyEP1->SetMarkerColor(kRed);
  h_sim_xyEP2->SetTitle("negative Endplate");
  h_sim_xyEP2->SetMarkerColor(kOrange+1);
  h_sim_yz->SetTitle("r-z projection");
  h_sim_yz->SetMarkerColor(kRed);
//   h_hit_xy->SetMarkerStyle(4);
//   h_hit_yz->SetMarkerStyle(4);
  h_hit_xyEP1->SetMarkerColor(kBlue);
  h_hit_xyEP2->SetMarkerColor(kAzure+1);
  h_hit_yz->SetMarkerColor(kBlue);
  

 
  h_sim_xyEP1->Draw();
  h_hit_xyEP1->Draw("sames");
  can->cd(2);
  h_sim_xyEP2->Draw();
  h_hit_xyEP2->Draw("sames");


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
  
  TH1F *resXY=new TH1F("resXY","resXY",100,-4,4);
  TH1F *resY=new TH1F("resY","residuals in y",100,-4,4);
  TH1F *resX=new TH1F("resX","residuals in x",100,-4,4);
  TH1F *resZ=new TH1F("resZ","residuals in z",100,-4,4);
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
      TF1 *f1 = new TF1("f1","[0]+[1]*x");
      g1->Fit("f1","QN");
      TGraph *g2 = new TGraph(xPos->size(), r, z);
      TF1 *f2 = new TF1("f2","[0]+[1]*x");
      g2->Fit("f2","QN");
      
      //Residuals
      for(int p=0;p<xPos->size();p++)
	{
	  resY->Fill(y[p]-f1->Eval(x[p]));
	  resX->Fill(x[p]-(y[p]-f1->GetParameter(0))/f1->GetParameter(1));
	  resZ->Fill(z[p]-f2->Eval(r[p]));
// 	  cout<<f1->GetParameter(0)<<" "<<f1->GetParameter(1)<<" | "
// 	      <<f1->Eval(x[p])<<" "<<f1->GetParameter(0)+f1->GetParameter(1)*x[p]<<" | "
// 	      <<x[p]<<" "<<(f1->Eval(x[p])-f1->GetParameter(0))/f1->GetParameter(1)<<endl;
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
  resX->Draw();
  can->cd(4);
  resY->Draw();

  can->cd(5);
  h_sim_yz->Draw();
  h_hit_yz->Draw("sames");
  can->cd(6);
  resZ->Draw();
//   can->cd(7);
//   resXY->Draw();
}


