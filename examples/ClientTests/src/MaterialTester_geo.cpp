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

#include "RVersion.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,20,0)

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Printout.h"

// ROOT include file
#include "TGeoElement.h"
#include "TGeoPhysicalConstants.h"
#include "TGeant4PhysicalConstants.h"
#include "TMath.h"

namespace units = dd4hep;
using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h xml_det, SensitiveDetector /* sens */)  {
  xml_det_t    x_det = xml_det;
  string       det_name = x_det.nameStr();
  Assembly     assembly(det_name+"_assembly");
  DetElement   det(det_name,x_det.typeStr(), x_det.id());

  for(xml_coll_t k(x_det,_Unicode(test)); k; ++k)  {	
    xml_comp_t c = k;
    Material mat = description.material(c.nameStr());
    TGeoMaterial* material = mat->GetMaterial();
    printout(INFO,det_name,"+++ Material:%s [%p, %p] Z=%6.2f A=%6.2f D=%9.4f [g/cm3]",
             material->GetName(), mat.ptr(), material, material->GetZ(),
             material->GetA(), material->GetDensity());
    
    printout(INFO,det_name,"+++          Radiation Length:%9.4f [cm] Interaction length:%9.4f [cm] Mixture:%s",
             material->GetRadLen()/TGeant4Unit::mm*units::cm, material->GetIntLen()/TGeant4Unit::mm*units::cm,
             //material->GetRadLen(), material->GetIntLen(),
             yes_no(material->IsMixture()));
    printout(INFO,det_name,"+++          Elements:%d Index:%d",
             material->GetNelements(), material->GetIndex());
    for(Int_t i=0, n=material->GetNelements(); i<n; ++i)  {
      TGeoElement* e = material->GetElement(i);
      Double_t a=0., z=0., w=0.;
      material->GetElementProp(a,z,w,i);
      printout(INFO,det_name,"+++          ELT[%02d]: %s Z=%3d N=%3d N_eff=%7.2f A=%6.2f Weight=%9.4f ",
               i, e->GetName(), e->Z(), e->N(), e->Neff(), e->A(), w);
      if ( material->IsMixture() )   {
        TGeoMixture* mix = (TGeoMixture*)material;
        Int_t* nmix = mix->GetNmixt();
        Double_t* wmix = mix->GetWmixt();
        printout(INFO,det_name,"+++                   Zmix:%7.3f Nmix:%3d Amix:%7.3f Wmix:%7.3f",
                 mix->GetZmixt()[i],nmix ? nmix[i] : -1,
                 mix->GetAmixt()[i],wmix ? wmix[i] : -1e0);
      }
    }
  }

  printout(INFO,det_name,"+++ Basic units:");
  printout(INFO,det_name,"+++ Length: mm:         %12.3f  Geant4: %8.3f mm    dd4hep: %8.3f mm    TGeo: %8.3g",
           units::mm, units::mm/TGeant4Unit::mm, units::mm/units::mm, units::mm/TGeoUnit::mm);
  printout(INFO,det_name,"+++ Time:   s:          %12.0g  Geant4: %8.3f s     dd4hep: %8.3f s     TGeo: %8.3g",
           units::s , units::s /TGeant4Unit::s , units::s /units::s,  units::s /TGeoUnit::s );
  printout(INFO,det_name,"+++ Energy: eV:         %12.3g  Geant4: %8.3f eV    dd4hep: %8.3f eV    TGeo: %8.3g",
           units::eV, units::eV/TGeant4Unit::eV, units::eV/units::eV, units::eV/TGeoUnit::eV);
  printout(INFO,det_name,"+++ Energy: MeV:        %12.3f  Geant4: %8.3f MeV   dd4hep: %8.3f MeV   TGeo: %8.3g",
           units::MeV, units::MeV/TGeant4Unit::MeV, units::MeV /units::MeV, units::MeV /TGeoUnit::MeV);
  printout(INFO,det_name,"+++ 1./Fine structure:  %12.3f  Geant4: %8.3f       dd4hep: %8.3f       TGeo: %8.3f",
           1./units::fine_structure_const, 1./TGeant4Unit::fine_structure_const,
           1./units::fine_structure_const, 1./TGeoUnit::fine_structure_const);
  printout(INFO,det_name,"+++ Universe density:   %12.3g  Geant4: %8.3g g/cm3 dd4hep: %8.3g g/cm3 TGeo: %8.3g",
           units::universe_mean_density,
           units::universe_mean_density/TGeant4Unit::g*TGeant4Unit::cm3,
           units::universe_mean_density/units::g*units::cm3,
           units::universe_mean_density/TGeoUnit::g*TGeoUnit::cm3);
  printout(INFO,det_name,"+++ STP_temperature:    %12.3f  Geant4: %8.3f K     dd4hep: %8.3f K     TGeo: %8.3f K",
           STP_temperature, STP_temperature/TGeant4Unit::kelvin,
           STP_temperature/units::kelvin, STP_temperature/TGeoUnit::kelvin);
  printout(INFO,det_name,"+++ STP_pressure:       %12.0f  Geant4: %8.3f hPa   dd4hep: %8.3f hPa   TGeo: %8.3f hPa",
           STP_pressure, STP_pressure/TGeant4Unit::pascal/1e2,
           STP_pressure/units::pascal/1e2,STP_pressure/TGeoUnit::pascal/1e2);
  
  PlacedVolume pv = description.pickMotherVolume(det).placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(MaterialTester,create_element)
#endif
