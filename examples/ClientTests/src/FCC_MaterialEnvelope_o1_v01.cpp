//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M. Petric
//               A driver just to place a envelope, to placement of dead material
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include "XML/Utilities.h"

using namespace std;

using dd4hep::DetElement;
using dd4hep::Detector;
using dd4hep::Ref_t;
using dd4hep::SensitiveDetector;

// workaround for DD4hep v00-14 (and older)
#ifndef DD4HEP_VERSION_GE
#define DD4HEP_VERSION_GE(a, b) 0
#endif

static Ref_t create_detector(Detector& theDetector, xml_h e, SensitiveDetector) {
  xml_det_t x_det = e;
  string det_name = x_det.nameStr();
  DetElement sdet(det_name, x_det.id());

  dd4hep::xml::createPlacedEnvelope(theDetector, e, sdet);

  return sdet;
}

DECLARE_DETELEMENT(MaterialEnvelope_o1_v01, create_detector)
