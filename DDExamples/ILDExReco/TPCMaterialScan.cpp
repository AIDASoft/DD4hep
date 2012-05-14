//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
//====================================================================
//  Test example to scan the material budget of the TPC
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================

#include "DD4hep/LCDD.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TRandom.h>
#include <TGeoManager.h>
#include "TVirtualGeoTrack.h"
#include "TGeoNode.h"
#include "GearTPC.h"

#define MINSTEP 1.e-5

using namespace std;
using namespace DD4hep;
using namespace Geometry;

double getNRadlen(DetElement det,std::vector<double> &p0, std::vector<double> &p1);
void ComputeBorderBarrel(std::vector<double> &initial, std::vector<double> &final,double Rmin,double Rmax,double Zmax,double theta,double phi);

int main(int argc,char** argv)  {
  
  // instanciate geometry from copact file
  LCDD& lcdd = LCDD::getInstance();
  lcdd.fromCompact(argv[1]);
  // get the TPC
  GearTPC tpc(lcdd.detector("TPC"));
  
  std::ofstream outfileX0("tpcmaterialscan_X0.dat");
  
  double tpc_radLen=0;
  for(double theta=0;theta<=90;theta+=0.1)
    {
      for(double phi=0;phi<=360;phi+=1)
	{
	  std::vector<double> initial(3,0);
	  std::vector<double> final(3,0);
	  
	  ComputeBorderBarrel(initial, final,0,tpc.getOuterRadius(),tpc.getEndPlateZPosition(0)+tpc.getEndPlateThickness(0),theta,phi);
	  tpc_radLen=getNRadlen(tpc,initial, final);
	  outfileX0<<theta<<" "<<phi<<" "<<tpc_radLen<<endl;

	   //clearing
	  initial.clear();
	  final.clear();
	}//phi
    }//theta

  outfileX0.close();  
}



double getNRadlen(DetElement det,std::vector<double> &p0, std::vector<double> &p1)
{    
  //check if itp0 and p1 are the same point, if so don't do anything, no material in between
  if(p0==p1)
    return 0;
  
  TGeoManager *_tgeomanager=det.volume()->GetGeoManager();
  std::vector<double> _radLen;
  std::vector<std::string> _volNames;
  std::vector<std::string> _matNames;
  std::vector<double> _distance;
  std::vector<double> _intLen;
  //-----------BEAM ON-------------------
  double startpoint[3], endpoint[3], direction[3];
  double L=0;
  for(unsigned int i=0; i<3; i++) {
    startpoint[i] = p0[i];
    endpoint[i]   = p1[i];
    direction[i] = endpoint[i] - startpoint[i];
    L+=direction[i]*direction[i];
  }
  
  //normalize direction
  for(unsigned int i=0; i<3; i++)
    direction[i]=direction[i]/sqrt(L);
  _tgeomanager->AddTrack(0,11);
  TGeoNode *node1 = _tgeomanager->InitTrack(startpoint, direction);
  //check if there is a node at startpoint
  if(!node1)
    throw OutsideGeometryException("No geometry node found at given location. Either there is no node placed here or position is outside of top volume.");
  
  while (!_tgeomanager->IsOutside()) 
    {
      TGeoNode *node2;
      TVirtualGeoTrack *track; 
      node2 = _tgeomanager->FindNextBoundaryAndStep(500, 1) ;
      
      if(!node2 || _tgeomanager->IsOutside())
	break;
      
      double *position =(double*) _tgeomanager->GetCurrentPoint();
      double *previouspos =(double*) _tgeomanager->GetLastPoint();
      double length=_tgeomanager->GetStep();
      track = _tgeomanager->GetLastTrack();
      //protection against infinitive loop in root which should not happen, but well it does...
      //work around until solution within root can be found when the step gets very small e.g. 1e-10
      //and the next boundary is never reached
      
      if(length<MINSTEP)
	{
	  _tgeomanager->SetCurrentPoint(position[0]+MINSTEP*direction[0], position[1]+MINSTEP*direction[1], position[2]+MINSTEP*direction[2]);
	  length=_tgeomanager->GetStep();
	  node2 = _tgeomanager->FindNextBoundaryAndStep(500, 1) ;
	}
      
      //if the next boundary is further than end point
      if(fabs(position[0])>fabs(endpoint[0]) || fabs(position[1])>fabs(endpoint[1]) 
	 || fabs(position[2])>fabs(endpoint[2]))
	{
	  length=sqrt(pow(endpoint[0]-previouspos[0],2)
		      +pow(endpoint[1]-previouspos[1],2)
		      +pow(endpoint[2]-previouspos[2],2));
	  
	  track->AddPoint(endpoint[0], endpoint[1], endpoint[2], 0.);
	  _distance.push_back(length);
	  std::string svoln(node1->GetName());
	  std::string svolm(node1->GetMedium()->GetMaterial()->GetName());
	  _volNames.push_back(svoln);
	  _matNames.push_back(svolm);
	  _intLen.push_back(node1->GetMedium()->GetMaterial()->GetIntLen());
	  _radLen.push_back(node1->GetMedium()->GetMaterial()->GetRadLen());
	  break;
	}
      
      track->AddPoint(position[0], position[1], position[2], 0.);
      _distance.push_back(length);
      std::string svoln(node1->GetName());
      std::string svolm(node1->GetMedium()->GetMaterial()->GetName());
      _volNames.push_back(svoln);
      _matNames.push_back(svolm);
      _intLen.push_back(node1->GetMedium()->GetMaterial()->GetIntLen());
      _radLen.push_back(node1->GetMedium()->GetMaterial()->GetRadLen());
      
      node1=node2;
    }
  _tgeomanager->ClearTracks();
  _tgeomanager->CleanGarbage();
  //-----------------------------------------
  
  double nRadLen=0;
  for(unsigned int i=0; i < _radLen.size(); i++){
    if(_radLen[i]!=0)
      nRadLen += _distance[i]/_radLen[i];
  }
  return nRadLen;
}

  
void ComputeBorderBarrel(std::vector<double> &initial, std::vector<double> &final,double Rmin,double Rmax,double Zmax,double theta,double phi)
{
  
  if(tan(theta*M_PI/180)==0)
    initial[2]=Zmax+100; //fake infinty
  else
    initial[2] = Rmin/tan(theta*M_PI/180);
  //missed detector
  if(initial[2]>Zmax)
    {
      initial[0] = 0;
      initial[1] = 0;
      initial[2] = 0;
      final[0] = 0;
      final[1] = 0;
      final[2] = 0;
      return;
    }
  else
    {
      initial[0] = Rmin*cos(phi*M_PI/180);
      initial[1] = Rmin*sin(phi*M_PI/180);
    }
  
  if(tan(theta*M_PI/180)==0)
    final[2]=Zmax+100; //fake infinty
  else
    final[2] = Rmax/tan(theta*M_PI/180);
  final[0] = Rmax*cos(phi*M_PI/180);
  final[1] = Rmax*sin(phi*M_PI/180);
  
  if(final[2]>Zmax)
    {
      final[2]=Zmax;
      double r=Zmax*tan(theta*M_PI/180);
      final[1] = r*sin(phi*M_PI/180);
      final[0] = r*cos(phi*M_PI/180);
    }
}
