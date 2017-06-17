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

// Framework include files
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

// ROOT include file
#include "TGeoElement.h"

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
             material->GetName(), mat.ptr(), m, material->GetZ(),
             material->GetA(), material->GetDensity());
    printout(INFO,det_name,"+++          Radiation Length:%9.4f Interaction length:%9.4f Mixture:%s",
             material->GetRadLen(), material->GetIntLen(),
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
        TGeoMixture* mix = (TGeoMixture*)m;
        Int_t* nmix = mix->GetNmixt();
        Double_t* wmix = mix->GetWmixt();
        printout(INFO,det_name,"+++                   Zmix:%7.3f Nmix:%3d Amix:%7.3f Wmix:%7.3f",
                 mix->GetZmixt()[i],nmix ? nmix[i] : -1,
                 mix->GetAmixt()[i],wmix ? wmix[i] : -1e0);
      }
    }
  }


  PlacedVolume pv = description.pickMotherVolume(det).placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(MaterialTester,create_element)
