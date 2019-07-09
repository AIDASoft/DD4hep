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
//
//  Simple program to print all the materials in a detector on
//  a straight line between two given points
// 
//  Author     : M.Frank, CERN
//
//==========================================================================
// Framework include files
#include "DDRec/MaterialScan.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

#include <cstdio>

using namespace dd4hep;
using namespace dd4hep::rec;

/// Default constructor
MaterialScan::MaterialScan()
  : m_detector(Detector::getInstance())
{
  m_materialMgr.reset(new MaterialManager(m_detector.world().volume()));
}

/// Default constructor
MaterialScan::MaterialScan(Detector& description)
  : m_detector(description)
{
  m_materialMgr.reset(new MaterialManager(m_detector.world().volume()));
}

/// Default destructor
MaterialScan::~MaterialScan()    {
}


/// Set a specific region to limit the scan (resets the subdetector selection)
void MaterialScan::setRegion(const char* region)   {
  Region reg;
  if ( region )   {
    reg = m_detector.region(region);
  }
  setRegion(reg);
}

/// Set a specific region to limit the scan (resets the subdetector selection)
void MaterialScan::setRegion(Region region)   {
  struct PvCollector  {
    Region rg;
    std::set<const TGeoNode*>& cont;
    PvCollector(Region r, std::set<const TGeoNode*>& c) : rg(r), cont(c) {}
    void collect(TGeoNode* pv)    {
      cont.insert(pv);
      for (Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau)
        collect(pv->GetDaughter(idau));
    }
    void operator()(TGeoNode* pv)    {
      Volume v = pv->GetVolume();
      Region r = v.region();
      if ( r.isValid() )  {
        collect(pv);
        return;
      }
      for (Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau)
        (*this)(pv->GetDaughter(idau));
    }
  };
  m_placements.clear();
  if ( region.isValid() )   {
    PvCollector coll(region, m_placements);
    coll(m_detector.world().placement().ptr());
    printout(ALWAYS,"MaterialScan","+++ Set new scanning region to: %s [%ld placements]",
             region.name(), m_placements.size());
  }
  else   {
    printout(ALWAYS,"MaterialScan","+++ No region restrictions set. Back to full scanning mode.");
  }
}

/// Set a specific detector volume to limit the scan
void MaterialScan::setDetector(const char* detector)   {
  DetElement det;
  if ( detector )   {
    det = m_detector.detector(detector);
  }
  setDetector(det);
}

/// Set a specific detector volume to limit the scan
void MaterialScan::setDetector(DetElement detector)   {
  struct PvCollector  {
    std::set<const TGeoNode*>& cont;
    PvCollector(std::set<const TGeoNode*>& c) : cont(c) {}
    void operator()(TGeoNode* pv)    {
      cont.insert(pv);
      for (Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau) {
        TGeoNode* daughter = pv->GetDaughter(idau);
        (*this)(daughter);
      }
    }
  };
  if ( detector.isValid() )   {
    PlacedVolume pv = detector.placement();
    m_placements.clear();
    if ( pv.isValid() )  {
      PvCollector coll(m_placements);
      coll(pv.ptr());
    }
    printout(ALWAYS,"MaterialScan","+++ Set new scanning volume to: %s [%ld placements]",
             detector.path().c_str(), m_placements.size());
  }
  else   {
    printout(ALWAYS,"MaterialScan","+++ No subdetector restrictions set. Back to full scanning mode.");
    m_placements.clear();
  }
}

/// Set a specific volume material to limit the scan
void MaterialScan::setMaterial(const char* material)   {
  Material mat;
  if ( material )   {
    mat = m_detector.material(material);
  }
  setMaterial(mat);
}

/// Set a specific volume material to limit the scan
void MaterialScan::setMaterial(Material material)   {
  struct PvCollector  {
    Material material;
    std::set<const TGeoNode*>& cont;
    PvCollector(Material m, std::set<const TGeoNode*>& c) : material(m), cont(c) {}
    void operator()(TGeoNode* pv)    {
      Volume   v = pv->GetVolume();
      Material m = v.material();
      if ( m.ptr() == material.ptr() )  {
        cont.insert(pv);
      }
      for (Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau)
        (*this)(pv->GetDaughter(idau));
    }
  };
  m_placements.clear();
  if ( material.isValid() )   {
    PvCollector coll(material, m_placements);
    coll(m_detector.world().placement().ptr());
    printout(ALWAYS,"MaterialScan","+++ Set new scanning material to: %s [%ld placements]",
             material.name(), m_placements.size());
  }
  else   {
    printout(ALWAYS,"MaterialScan","+++ No material restrictions set. Back to full scanning mode.");
  }
}

/// Scan along a line and store the matrials internally
const MaterialVec& MaterialScan::scan(double x0, double y0, double z0, double x1, double y1, double z1, double epsilon)  const  {
  Vector3D p0(x0, y0, z0), p1(x1, y1, z1);
  return m_materialMgr->materialsBetween(p0, p1, epsilon);
}

/// Scan along a line and print the materials traversed
void MaterialScan::print(const Vector3D& p0, const Vector3D& p1, double epsilon)  const    {
  const auto& placements = m_materialMgr->placementsBetween(p0, p1, epsilon);
  auto& matMgr = *m_materialMgr;
  Vector3D end, direction;
  double sum_x0 = 0;
  double sum_lambda = 0;
  double path_length = 0, total_length = 0;
  const char* fmt1 = " | %5d %-20s %3.0f %8.3f %8.4f %11.4f  %11.4f %10.3f %8.2f %11.6f %11.6f  (%7.2f,%7.2f,%7.2f)\n";
  const char* fmt2 = " | %5d %-20s %3.0f %8.3f %8.4f %11.6g  %11.6g %10.3f %8.2f %11.6f %11.6f  (%7.2f,%7.2f,%7.2f)\n";
  const char* line = " +--------------------------------------------------------------------------------------------------------------------------------------------------\n";

  direction = (p1-p0).unit();
  
  ::printf("%s + Material scan between: x_0 = (%7.2f,%7.2f,%7.2f) [cm] and x_1 = (%7.2f,%7.2f,%7.2f) [cm] : \n%s",
           line,p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],line);
  ::printf(" |     \\   %-11s        Atomic                 Radiation   Interaction               Path   Integrated  Integrated    Material\n","Material");
  ::printf(" | Num. \\  %-11s   Number/Z   Mass/A  Density    Length       Length    Thickness   Length      X0        Lambda      Endpoint  \n","Name");
  ::printf(" | Layer \\ %-11s            [g/mole]  [g/cm3]     [cm]        [cm]          [cm]      [cm]     [cm]        [cm]     (     cm,     cm,     cm)\n","");
  ::printf("%s",line);
  MaterialVec materials;
  for( unsigned i=0,n=placements.size(); i<n; ++i){
    TGeoNode*  pv  = placements[i].first.ptr();
    double length  = placements[i].second;
    total_length  += length;
    end = p0 + total_length * direction;
    if ( !m_placements.empty() && m_placements.find(pv) == m_placements.end() )  {
#if 0
      ::printf("%p %s  %s  %s\n",
               placements[i].first.ptr(),
               placements[i].first->GetName(),
               placements[i].first->GetVolume()->GetName(),
               placements[i].first->GetMedium()->GetName());
#endif
      continue;
    }
    TGeoMaterial* mat = placements[i].first->GetMedium()->GetMaterial();
    double nx0     = length / mat->GetRadLen();
    double nLambda = length / mat->GetIntLen();

    sum_x0        += nx0;
    sum_lambda    += nLambda;
    path_length   += length;
    materials.push_back(std::make_pair(placements[i].first->GetMedium(),length));
    const char* fmt = mat->GetRadLen() >= 1e5 ? fmt2 : fmt1;
    ::printf(fmt, i+1, mat->GetName(), mat->GetZ(), mat->GetA(),
             mat->GetDensity(), mat->GetRadLen(), mat->GetIntLen(), 
             length, path_length, sum_x0, sum_lambda, end[0], end[1], end[2]);
    //mat->Print();
  }
  printf("%s",line);
  const MaterialData& avg = matMgr.createAveragedMaterial(materials);
  const char* fmt = avg.radiationLength() >= 1e5 ? fmt2 : fmt1;
  ::printf(fmt,0,"Average Material",avg.Z(),avg.A(),avg.density(), 
           avg.radiationLength(), avg.interactionLength(),
           path_length, path_length, 
           path_length/avg.radiationLength(), 
           path_length/avg.interactionLength(),
           end[0], end[1], end[2]);
  printf("%s",line);
}

/// Scan along a line and print the materials traversed
void MaterialScan::print(double x0, double y0, double z0, double x1, double y1, double z1, double epsilon)  const  {
  Vector3D p0(x0, y0, z0), p1(x1, y1, z1);
  print(p0, p1, epsilon);
}
