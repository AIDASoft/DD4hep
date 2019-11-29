//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

/// Framework include files
#include <DD4hep/Factories.h>
#include <DD4hep/Printout.h>
#include <DDDigi/FalphaNoise.h>

/// C/C++ include files
#include <random>
#include <iostream>

#include <TH1.h>
#include <TCanvas.h>
#include <TPad.h>

using namespace dd4hep;

/// Plugin to test the pink noise generator
/**
 *  Factory: DD4hep_DummyPlugin
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    27/11/2019
 */
static long test_FalphaNoise(Detector& , int argc, char** argv) {
  using namespace dd4hep::detail;
  bool   draw = false;
  size_t poles = 5;
  size_t shots = 10;
  double alpha = 0.5, variance = 1.0;
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-shots",argv[i],3) )
      shots = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-alpha",argv[i],3) )
      alpha       = ::atof(argv[++i]);
    else if ( 0 == ::strncmp("-variance",argv[i],3) )
      variance    = ::atof(argv[++i]);
    else if ( 0 == ::strncmp("-poles",argv[i],3) )
      poles       = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-draw",argv[i],3) )
      draw = true;
    else  {
      std::cout <<
        "Usage: -plugin DD4hep_FalphaNoise -arg [-arg]                            \n"
        "     -shots    <value>  Number of samples to be generated [default: 10]  \n"
        "     -alpha    <value>  Parameter for the 1/f**alpha distribution        \n"
        "                        default: 0.5                                     \n"
        "     -variance <value>  Distribution variance [default: 1]               \n"
        "     -poles    <value>  Number of IRR poles to fill the distribution     \n"
        "     -draw              Fill and draw hoistogram with distribution       \n"
        "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
      ::exit(EINVAL);
    }
  }

  std::default_random_engine generator;
  FalphaNoise noise(poles, alpha, variance);
  FalphaNoise noise0(poles, 0,    variance);
  FalphaNoise noise1(poles, 1,    variance);
  FalphaNoise noise2(poles, 1.98, variance);
  std::stringstream cpara;
  cpara << " distribution alpha=" << alpha << " sigma=" << variance;
  TH1D* hist11 = new TH1D("D11", ("1/f**alpha"+cpara.str()).c_str(), 50, -5e0*variance, 5e0*variance);
  TH1D* hist12 = new TH1D("D12", ("1/f**alpha"+cpara.str()).c_str(), 50, -5e0*variance, 5e0*variance);
  TH1D* hist13 = new TH1D("D13", ("1/f**alpha"+cpara.str()).c_str(), 50, -5e0*variance, 5e0*variance);
  TH1D* hist14 = new TH1D("D14", ("1/f**alpha"+cpara.str()).c_str(), 50, -5e0*variance, 5e0*variance);
  TH1D* hist21 = new TH1D("D21", ("log10(1/f**alpha)"+cpara.str()).c_str(), 300, 1e0, 1e2*variance);
  TH1D* hist22 = new TH1D("D22", "log10(1/f**alpha) alpha=0", 300, 1e0, 1e2*variance);
  TH1D* hist23 = new TH1D("D23", "log10(1/f**alpha) alpha=1", 300, 1e0, 1e2*variance);
  TH1D* hist24 = new TH1D("D24", "log10(1/f**alpha) alpha=2", 300, 1e0, 1e2*variance);

  printout(INFO, "FalphaNoise", "Executing %ld shots .... variance=%.3f alpha=%.3f",
           shots, noise.variance(), noise.alpha());
  for(size_t i=0; i < shots; ++i)  {
    double val = noise(generator);
    hist11->Fill(val, 1.);
    hist21->Fill(std::exp(std::abs(val)), 1.);

    val = noise0(generator);
    hist12->Fill(val, 1.);
    hist22->Fill(std::exp(std::abs(val)), 1.);

    val = noise1(generator);
    hist13->Fill(val, 1.);
    hist23->Fill(std::exp(std::abs(val)), 1.);

    val = noise2(generator);
    hist14->Fill(val, 1.);
    hist24->Fill(std::exp(std::abs(val)), 1.);
  }

  printout(INFO, "FalphaNoise", "Distribution  Mean             %10.5f", hist11->GetMean());
  printout(INFO, "FalphaNoise", "Distribution  Mean Uncertainty %10.5f", hist11->GetMeanError());
  printout(INFO, "FalphaNoise", "Distribution  RMS              %10.5f", hist11->GetRMS());
  printout(INFO, "FalphaNoise", "Distribution  RMS  Uncertainty %10.5f", hist11->GetRMSError());
  hist11->GetXaxis()->SetTitle("Energy [arb.units]");
  hist11->GetYaxis()->SetTitle("Counts");
  hist21->GetXaxis()->SetTitle("exp(Energy) [arb.units]");
  hist21->GetYaxis()->SetTitle("Counts");

  hist21->Scale(1.0 / hist21->GetBinContent(1));
  hist22->Scale(1.0 / hist22->GetBinContent(1));
  hist23->Scale(1.0 / hist23->GetBinContent(1));
  hist24->Scale(1.0 / hist24->GetBinContent(1));
  if ( draw )   {
    TCanvas *c = new TCanvas("c1","multigraph",700,500);
    c->Divide(1,2);
    c->cd(1);

    gPad->SetGrid();
    hist12->SetLineColor(kBlue);
    hist12->Draw("L");

    hist11->SetLineWidth(1);
    hist11->SetLineColor(kRed);
    hist11->Draw("SAME");

    hist11->SetLineWidth(1);
    hist11->SetLineColor(kRed);
    hist11->Draw("LSAME");
    
    hist13->SetLineColor(kMagenta);
    hist13->Draw("LSAME");

    hist14->SetLineColor(kGreen);
    hist14->Draw("LSAME");

    c->cd(2);
    gPad->SetGrid();
    gPad->SetLogx();
    gPad->SetLogy();
    hist21->Draw("");
    hist21->SetLineWidth(1);
    hist21->SetLineColor(kRed);
    hist21->Draw("LPSAME");

    hist24->SetLineWidth(1);
    hist24->SetLineColor(kGreen);
    hist24->Draw("LPSAME");
    
    hist22->SetLineWidth(1);
    hist22->SetLineColor(kBlue);
    hist22->Draw("LPSAME");
    hist23->SetLineWidth(1);
    hist23->SetLineColor(kMagenta);
    hist23->Draw("LPSAME");
    gPad->Update();
    gPad->Modified();
  }
  return 1;
}
DECLARE_APPLY(DD4hep_FalphaNoise,test_FalphaNoise)
