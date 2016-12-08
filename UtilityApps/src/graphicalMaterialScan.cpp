// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//
//  Simple program to make a "CT-scan" of a detector materials
//  makes series of slices perpendicular to X, Y, or Z axis
//  produces a TFile with a TH2F per slice
//    scans the material along a few (2*nSamples, in the 2 slice dimensions) paths across each bin
//    fills histogram bins with:
//       average X0/length across these paths
//       average lambda/length across these paths
//       for each material, fraction of path length which crosses that material
//   (inspired by material scan)
//  Author     : D.Jeans, UTokyo
//
//==========================================================================

#include "TError.h"

#include "TFile.h"
#include "TH2F.h"

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialManager.h"

#include <iostream>
#include <string>
#include <map>

#undef NDEBUG 
#include <cassert>

using namespace DD4hep;
using namespace DDRec;
using DDSurfaces::Vector3D;

using std::cout;
using std::endl;

int main(int argc, char** argv)   {
  struct Handler  {
    Handler() { SetErrorHandler(Handler::print); }
    static void print(int level, Bool_t abort, const char *location, const char *msg)  {
      if ( level > kInfo || abort ) ::printf("%s: %s\n", location, msg);
    }
    static void usage()  {
      std::cout << " usage: graphicalMaterialScan compact.xml axis xMin yMin zMin xMax yMax zMax nSlices nBins nSamples" << std::endl
		<< " axis (X, Y, or Z)             : perpendicular to the slices" << std::endl 
		<< " xMin yMin zMin xMax yMax zMax : range of scans " << std::endl 
		<< " nSlices                       : number of slices (equally spaced along chose axis)" << std::endl 
		<< " nBins                         : number of bins along each axis of histograms" << std::endl 
		<< " nSamples                      : the number of times each bin is sampled " << std::endl 
                << "        -> produces graphical scans of the detector material "
                << std::endl;
      exit(1);
    }
  } _handler;

  // "axis" is the normal to the slices, which are equally spaced between the corresponding max and min
  // each slice has nBins x nBins in the specified range
  // the material in each bin is sampled along 2*nTests paths

  if( argc != 12 ) Handler::usage();

  std::string inFile = argv[1]; // input geometry description compact xml file

  std::string XYZ    = argv[2]; // the axis

  TString labx, laby;
  unsigned int index[3] = {99,99,99};
  if ( XYZ=="x" || XYZ=="X" ) {
    index[0] = 0; // this is the one perpendicular to slices
    index[1] = 2;
    index[2] = 1;
    labx="Z [cm]";
    laby="Y [cm]";
  } else if ( XYZ=="y" || XYZ=="Y" ) {
    index[0] = 1;
    index[1] = 2;
    index[2] = 0;
    labx="Z [cm]";
    laby="Z [cm]";
  } else if ( XYZ=="z" || XYZ=="Z" ) {
    index[0] = 2;
    index[1] = 0;
    index[2] = 1;
    labx="X [cm]";
    laby="Y [cm]";
  } else {
    cout << "invalid XYZ" << endl;
    return -1;
  }

  double x0,y0,z0,x1,y1,z1;
  unsigned int nslice, nbins, mm;
  std::stringstream sstr;
  sstr << 
    argv[3] << " " << argv[4] << " " << argv[5] << " " << 
    argv[6] << " " << argv[7] << " " << argv[8] << " " << 
    argv[9] << " " << argv[10] << " " << argv[11] << " " << "NONE";
  sstr >> x0 >> y0 >> z0 >> x1 >> y1 >> z1 >> nslice >> nbins >> mm;
  if ( !sstr.good() ) Handler::usage();

  if ( x0>x1 ) { double temp=x0; x0=x1; x1=temp; }
  if ( y0>y1 ) { double temp=y0; y0=y1; y1=temp; }
  if ( z0>z1 ) { double temp=z0; z0=z1; z1=temp; }

  if ( ! ( nbins>0 && nslice>0 ) ) {
    cout << "funny # bins/slices " << endl;
    return 1;
  }

  double mmin[3]={x0,y0,z0};
  double mmax[3]={x1,y1,z1};


  //------
  
  Geometry::LCDD& lcdd = Geometry::LCDD::getInstance();
  lcdd.fromCompact(inFile);

  //-----

  TFile* f = new TFile("graphicalMaterialScan.root","recreate");

  Vector3D p0, p1; // the two points between which material is calculated

  MaterialManager matMgr;

  for (unsigned int isl=0; isl<nslice; isl++) { // loop over slices

    double sz = nslice > 1 ? 
      mmin[index[0]] + isl*( mmax[index[0]] - mmin[index[0]] )/( nslice - 1 ) :
      (mmin[index[0]] + mmax[index[0]])/2. ;

    p0.array()[ index[0] ] = sz;
    p1.array()[ index[0] ] = sz;

    cout << "scanning slice " << isl << " at "+XYZ+" = " << sz << endl;

    TString dirn = "Slice"; dirn+=isl;
    f->mkdir(dirn);
    f->cd(dirn);

    std::map < std::string , TH2F* > scanmap;


    TString hn = "slice"; hn+=isl; hn+="_X0";
    TString hnn = "X0 "; hnn += XYZ; hnn+="="; hnn += Form("%7.3f",sz); hnn+=" [cm]";

    for (int j=1; j<=2; j++) {
      if ( mmax[index[j]] - mmin[index[j]] < 1e-4 ) {
	cout << "ERROR: max and min of axis are the same!" << endl;
	assert(0);
      }
    }


    TH2F* h2slice = new TH2F( hn, hnn, nbins, mmin[index[1]], mmax[index[1]], nbins, mmin[index[2]], mmax[index[2]] );
    scanmap["x0"] = h2slice;

    hn = "slice"; hn+=isl; hn+="_lambda";
    hnn = "lambda "; hnn += XYZ; hnn+="="; hnn += Form("%7.3f",sz); hnn+=" [cm]";
    h2slice = new TH2F( hn, hnn, nbins, mmin[index[1]], mmax[index[1]], nbins, mmin[index[2]], mmax[index[2]] );
    scanmap["lambda"] = h2slice;


    for (int ix=1; ix<=h2slice->GetNbinsX(); ix++) {  // loop over one axis of slice

      double xmin = h2slice->GetXaxis()->GetBinLowEdge(ix);
      double xmax = h2slice->GetXaxis()->GetBinUpEdge(ix);

      for (int iy=1; iy<=h2slice->GetNbinsY(); iy++) { // and the other axis

	double ymin = h2slice->GetYaxis()->GetBinLowEdge(iy);
	double ymax = h2slice->GetYaxis()->GetBinUpEdge(iy);

	// for this bin, estimate the material
	double sum_lambda(0);
	double sum_x0(0);
	double sum_length(0);

	std::map < std::string , float > materialmap;

	for (unsigned int jx=0; jx<2*mm; jx++) {
	  if ( jx<mm ) {
	    double xcom = xmin + (1+jx)*( xmax - xmin )/(mm+1.);
	    p0.array()[index[1]] = xcom;  p0.array()[index[2]] = ymin;
	    p1.array()[index[1]] = xcom;  p1.array()[index[2]] = ymax;
	  } else {
	    double ycom =  ymin + (jx-mm+1)*( ymax - ymin )/(mm+1.);
	    p0.array()[index[1]] = xmin;  p0.array()[index[2]] = ycom;
	    p1.array()[index[1]] = xmax;  p1.array()[index[2]] = ycom;
	  }

	  const MaterialVec& materials = matMgr.materialsBetween(p0, p1);
	  for( unsigned i=0,n=materials.size();i<n;++i){
	    TGeoMaterial* mat =  materials[i].first->GetMaterial();
	    double length = materials[i].second;
	    sum_length += length;
	    double nx0 = length / mat->GetRadLen();
	    sum_x0 += nx0;
	    double nLambda = length / mat->GetIntLen();
	    sum_lambda += nLambda;

	    std::string mname = mat->GetName();
	    if ( materialmap.find( mname )!=materialmap.end() ) {
	      materialmap[mname]+=length;
	    } else {
	      materialmap[mname]=length;
	    }

	  }

	}
	
	scanmap["x0"]->SetBinContent(ix, iy, sum_x0/sum_length); // normalise to cm (ie x0/cm density: indep of bin size)
	scanmap["lambda"]->SetBinContent(ix, iy, sum_lambda/sum_length);

	for (  std::map < std::string , float >::iterator jj = materialmap.begin(); jj!=materialmap.end(); jj++) {
	  if ( scanmap.find( jj->first )==scanmap.end() ) {
	    hn = "slice"; hn+=isl; hn+="_"+jj->first;
	    hnn = jj->first; hnn += " "+XYZ; hnn+="="; 
	    // hnn+=sz; 
	    hnn += Form("%7.3f",sz);
	    hnn+=" [cm]";
	    scanmap[jj->first] = new TH2F( hn, hnn, nbins, mmin[index[1]], mmax[index[1]], nbins, mmin[index[2]], mmax[index[2]] );
	  }
	  scanmap[jj->first]->SetBinContent(ix, iy, jj->second / sum_length );
	}


      }
    }

    for (  std::map < std::string , TH2F* >::iterator jj = scanmap.begin(); jj!=scanmap.end(); jj++) {
      jj->second->SetOption("zcol");
      jj->second->GetXaxis()->SetTitle(labx);
      jj->second->GetYaxis()->SetTitle(laby);
    }


  }


  f->Write();
  f->Close();

  return 0;
}
