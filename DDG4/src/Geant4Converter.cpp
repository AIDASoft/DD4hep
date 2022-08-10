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
#include <DD4hep/Detector.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/Shapes.h>
#include <DD4hep/Volumes.h>
#include <DD4hep/Printout.h>
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/PropertyTable.h>
#include <DD4hep/detail/ShapesInterna.h>
#include <DD4hep/detail/ObjectsInterna.h>
#include <DD4hep/detail/DetectorInterna.h>

#include <DDG4/Geant4Field.h>
#include <DDG4/Geant4Helpers.h>
#include <DDG4/Geant4Converter.h>
#include <DDG4/Geant4UserLimits.h>
#include <DDG4/Geant4PlacementParameterisation.h>
#include "Geant4ShapeConverter.h"

// ROOT includes
#include <TClass.h>
#include <TGeoBoolNode.h>

// Geant4 include files
#include <G4Version.hh>
#include <G4VisAttributes.hh>
#include <G4PVParameterised.hh>
#include <G4ProductionCuts.hh>
#include <G4VUserRegionInformation.hh>

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Ellipsoid.hh>
#include <G4UnionSolid.hh>
#include <G4ReflectedSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>
#include <G4VSensitiveDetector.hh>

#include <G4Region.hh>
#include <G4Element.hh>
#include <G4Isotope.hh>
#include <G4Material.hh>
#include <G4UserLimits.hh>
#include <G4FieldManager.hh>
#include <G4LogicalVolume.hh>
#include <G4ReflectionFactory.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4ElectroMagneticField.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4MaterialPropertiesTable.hh>
#if G4VERSION_NUMBER >= 1040
#include <G4MaterialPropertiesIndex.hh>
#endif
#include <G4ScaledSolid.hh>
#include <CLHEP/Units/SystemOfUnits.h>

// C/C++ include files
#include <iostream>
#include <iomanip>
#include <sstream>

namespace units = dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::sim;
using namespace dd4hep;
using namespace std;

#include <DDG4/Geant4AssemblyVolume.h>
#include <DD4hep/DetectorTools.h>

static constexpr const double CM_2_MM = (CLHEP::centimeter/dd4hep::centimeter);
static constexpr const char* GEANT4_TAG_IGNORE = "Geant4-ignore";
static constexpr const char* GEANT4_TAG_PLUGIN = "Geant4-plugin";

namespace {
  static string indent = "";
  bool is_left_handed(const TGeoMatrix* m)   {
    const Double_t* r = m->GetRotationMatrix();
    if ( r )    {
      Double_t det =
        r[0]*r[4]*r[8] + r[3]*r[7]*r[2] + r[6]*r[1]*r[5] -
        r[2]*r[4]*r[6] - r[5]*r[7]*r[0] - r[8]*r[1]*r[3];
      return det < 0e0;
    }
    return false;
  }

  class G4UserRegionInformation : public G4VUserRegionInformation {
  public:
    Region region;
    double threshold;
    bool storeSecondaries;
    G4UserRegionInformation()
      : threshold(0.0), storeSecondaries(false) {
    }
    virtual ~G4UserRegionInformation() {
    }
    virtual void Print() const {
      if (region.isValid())
        printout(DEBUG, "Region", "Name:%s", region.name());
    }
  };

  pair<double,double> g4PropertyConversion(int index)   {
#if G4VERSION_NUMBER >= 1040
    switch(index)  {
    case kRINDEX:                         return make_pair(CLHEP::keV/units::keV, 1.0);
    case kREFLECTIVITY:                   return make_pair(CLHEP::keV/units::keV, 1.0);
    case kREALRINDEX:                     return make_pair(CLHEP::keV/units::keV, 1.0);
    case kIMAGINARYRINDEX:                return make_pair(CLHEP::keV/units::keV, 1.0);
    case kEFFICIENCY:                     return make_pair(CLHEP::keV/units::keV, 1.0);
    case kTRANSMITTANCE:                  return make_pair(CLHEP::keV/units::keV, 1.0);
    case kSPECULARLOBECONSTANT:           return make_pair(CLHEP::keV/units::keV, 1.0);
    case kSPECULARSPIKECONSTANT:          return make_pair(CLHEP::keV/units::keV, 1.0);
    case kBACKSCATTERCONSTANT:            return make_pair(CLHEP::keV/units::keV, 1.0);
    case kGROUPVEL:                       return make_pair(CLHEP::keV/units::keV, (CLHEP::m/CLHEP::s)/(units::m/units::s));  // meter/second
    case kMIEHG:                          return make_pair(CLHEP::keV/units::keV, CLHEP::m/units::m);
    case kRAYLEIGH:                       return make_pair(CLHEP::keV/units::keV, CLHEP::m/units::m);  // ??? says its a length
    case kWLSCOMPONENT:                   return make_pair(CLHEP::keV/units::keV, 1.0);
    case kWLSABSLENGTH:                   return make_pair(CLHEP::keV/units::keV, CLHEP::m/units::m);
    case kABSLENGTH:                      return make_pair(CLHEP::keV/units::keV, CLHEP::m/units::m);
#if G4VERSION_NUMBER >= 1100
    case kWLSCOMPONENT2:                  return make_pair(CLHEP::keV/units::keV, 1.0);
    case kWLSABSLENGTH2:                  return make_pair(CLHEP::keV/units::keV, CLHEP::m/units::m);
    case kSCINTILLATIONCOMPONENT1:        return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    case kSCINTILLATIONCOMPONENT2:        return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    case kSCINTILLATIONCOMPONENT3:        return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
#else
    case kFASTCOMPONENT:                  return make_pair(CLHEP::keV/units::keV, 1.0);
    case kSLOWCOMPONENT:                  return make_pair(CLHEP::keV/units::keV, 1.0);
#endif
    case kPROTONSCINTILLATIONYIELD:       return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV); // Yields: 1/energy
    case kDEUTERONSCINTILLATIONYIELD:     return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    case kTRITONSCINTILLATIONYIELD:       return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    case kALPHASCINTILLATIONYIELD:        return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    case kIONSCINTILLATIONYIELD:          return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    case kELECTRONSCINTILLATIONYIELD:     return make_pair(CLHEP::keV/units::keV, units::keV/CLHEP::keV);
    default:
      break;
    }
    printout(FATAL,"Geant4Converter", "+++ Cannot convert material property with index: %d", index);
#else
    printout(FATAL,"Geant4Converter", "+++ Cannot convert material property with index: %d [Need Geant4 > 10.03]", index);
#endif
    return make_pair(0e0,0e0);
  }

  double g4ConstPropertyConversion(int index)   {
#if G4VERSION_NUMBER >= 1040
    switch(index)   {
    case kSURFACEROUGHNESS:            return CLHEP::m/units::m;                             // Length
    case kISOTHERMAL_COMPRESSIBILITY:  return (CLHEP::m3/CLHEP::keV)/(units::m3/CLHEP::keV); // Volume/Energy
    case kRS_SCALE_FACTOR:             return 1.0;  // ??
    case kWLSMEANNUMBERPHOTONS:        return 1.0;  // ??
    case kWLSTIMECONSTANT:             return CLHEP::second/units::second;                   // Time
    case kMIEHG_FORWARD:               return 1.0;
    case kMIEHG_BACKWARD:              return 1.0;
    case kMIEHG_FORWARD_RATIO:         return 1.0;
    case kSCINTILLATIONYIELD:          return units::keV/CLHEP::keV;                         // Energy
    case kRESOLUTIONSCALE:             return 1.0;
    case kFERMIPOT:                    return CLHEP::keV/units::keV;                         // Energy
    case kDIFFUSION:                   return 1.0;
    case kSPINFLIP:                    return 1.0;
    case kLOSS:                        return 1.0;  // ??
    case kLOSSCS:                      return CLHEP::barn/units::barn;  // ??
    case kABSCS:                       return CLHEP::barn/units::barn;  // ??
    case kSCATCS:                      return CLHEP::barn/units::barn;  // ??
    case kMR_NBTHETA:                  return 1.0;
    case kMR_NBE:                      return 1.0;
    case kMR_RRMS:                     return 1.0;  // ??
    case kMR_CORRLEN:                  return CLHEP::m/units::m;                             // Length
    case kMR_THETAMIN:                 return 1.0;
    case kMR_THETAMAX:                 return 1.0;
    case kMR_EMIN:                     return CLHEP::keV/units::keV;                         // Energy
    case kMR_EMAX:                     return CLHEP::keV/units::keV;                         // Energy
    case kMR_ANGNOTHETA:               return 1.0;
    case kMR_ANGNOPHI:                 return 1.0;
    case kMR_ANGCUT:                   return 1.0;

#if G4VERSION_NUMBER >= 1100
    case kSCINTILLATIONTIMECONSTANT1:  return CLHEP::second/units::second;                   // Time
    case kSCINTILLATIONTIMECONSTANT2:  return CLHEP::second/units::second;                   // Time
    case kSCINTILLATIONTIMECONSTANT3:  return CLHEP::second/units::second;                   // Time
    case kSCINTILLATIONRISETIME1:      return CLHEP::second/units::second;                   // Time
    case kSCINTILLATIONRISETIME2:      return CLHEP::second/units::second;                   // Time
    case kSCINTILLATIONRISETIME3:      return CLHEP::second/units::second;                   // Time
    case kSCINTILLATIONYIELD1:         return 1.0;
    case kSCINTILLATIONYIELD2:         return 1.0;
    case kSCINTILLATIONYIELD3:         return 1.0;
    case kPROTONSCINTILLATIONYIELD1:   return 1.0;
    case kPROTONSCINTILLATIONYIELD2:   return 1.0;
    case kPROTONSCINTILLATIONYIELD3:   return 1.0;
    case kDEUTERONSCINTILLATIONYIELD1: return 1.0;
    case kDEUTERONSCINTILLATIONYIELD2: return 1.0;
    case kDEUTERONSCINTILLATIONYIELD3: return 1.0;
    case kALPHASCINTILLATIONYIELD1:    return 1.0;
    case kALPHASCINTILLATIONYIELD2:    return 1.0;
    case kALPHASCINTILLATIONYIELD3:    return 1.0;
    case kIONSCINTILLATIONYIELD1:      return 1.0;
    case kIONSCINTILLATIONYIELD2:      return 1.0;
    case kIONSCINTILLATIONYIELD3:      return 1.0;
    case kELECTRONSCINTILLATIONYIELD1: return 1.0;
    case kELECTRONSCINTILLATIONYIELD2: return 1.0;
    case kELECTRONSCINTILLATIONYIELD3: return 1.0;
#else
    case kFASTTIMECONSTANT:            return CLHEP::second/units::second;                   // Time
    case kFASTSCINTILLATIONRISETIME:   return CLHEP::second/units::second;                   // Time
    case kSLOWTIMECONSTANT:            return CLHEP::second/units::second;                   // Time
    case kSLOWSCINTILLATIONRISETIME:   return CLHEP::second/units::second;                   // Time
    case kYIELDRATIO:                  return 1.0;
#endif
    default:
      break;
    }
    printout(FATAL,"Geant4Converter", "+++ Cannot convert CONST material property with index: %d", index);
#else
    printout(FATAL,"Geant4Converter", "+++ Cannot convert material property with index: %d [Need Geant4 > 10.03]", index);
#endif
    return 0.0;
  }
}

/// Initializing Constructor
Geant4Converter::Geant4Converter(const Detector& description_ref)
  : Geant4Mapping(description_ref), checkOverlaps(true) {
  this->Geant4Mapping::init();
  m_propagateRegions = true;
  outputLevel = PrintLevel(printLevel() - 1);
}

/// Initializing Constructor
Geant4Converter::Geant4Converter(const Detector& description_ref, PrintLevel level)
  : Geant4Mapping(description_ref), checkOverlaps(true) {
  this->Geant4Mapping::init();
  m_propagateRegions = true;
  outputLevel = level;
}

/// Standard destructor
Geant4Converter::~Geant4Converter() {
}

/// Handle the conversion of isotopes
void* Geant4Converter::handleIsotope(const string& /* name */, const TGeoIsotope* iso) const {
  G4Isotope* g4i = data().g4Isotopes[iso];
  if ( !g4i )  {
    double a_conv = (CLHEP::g / CLHEP::mole);
    g4i = new G4Isotope(iso->GetName(), iso->GetZ(), iso->GetN(), iso->GetA()*a_conv);
    printout(debugElements ? ALWAYS : outputLevel,
             "Geant4Converter", "++ Created G4 Isotope %s from data: Z=%d N=%d A=%.3f [g/mole]",
             iso->GetName(), iso->GetZ(), iso->GetN(), iso->GetA());
    data().g4Isotopes[iso] = g4i;
  }
  return g4i;
}

/// Handle the conversion of elements
void* Geant4Converter::handleElement(const string& name, const Atom element) const {
  G4Element* g4e = data().g4Elements[element];
  if ( !g4e ) {
    PrintLevel lvl = debugElements ? ALWAYS : outputLevel;
    if (element->GetNisotopes() > 0) {
      g4e = new G4Element(name, element->GetTitle(), element->GetNisotopes());
      for (int i = 0, n = element->GetNisotopes(); i < n; ++i) {
        TGeoIsotope* iso = element->GetIsotope(i);
        G4Isotope* g4iso = (G4Isotope*)handleIsotope(iso->GetName(), iso);
        g4e->AddIsotope(g4iso, element->GetRelativeAbundance(i));
      }
    }
    else {
      // This adds in Geant4 the natural isotopes, which we normally do not want. We want to steer it outselves.
      double a_conv = (CLHEP::g / CLHEP::mole);
      g4e = new G4Element(element->GetTitle(), name, element->Z(), element->A()*a_conv);
      printout(lvl, "Geant4Converter", "++ Created G4 Isotope %s from data: Z=%d N=%d A=%.3f [g/mole]",
               element->GetName(), element->Z(), element->N(), element->A());
    }
    stringstream str;
    str << (*g4e) << endl;
    printout(lvl, "Geant4Converter", "++ Created G4 element %s", str.str().c_str());
    data().g4Elements[element] = g4e;
  }
  return g4e;
}

/// Dump material in GDML format to output stream
void* Geant4Converter::handleMaterial(const string& name, Material medium) const {
  Geant4GeometryInfo& info = data();
  G4Material*         mat  = info.g4Materials[medium];
  if ( !mat )  {
    PrintLevel    lvl      = debugMaterials ? ALWAYS : outputLevel;
    TGeoMaterial* material = medium->GetMaterial();
    G4State       state    = kStateUndefined;
    double        density  = material->GetDensity() * (CLHEP::gram / CLHEP::cm3);
    if ( density < 1e-25 )
      density = 1e-25;
    switch ( material->GetState() ) {
    case TGeoMaterial::kMatStateSolid:
      state = kStateSolid;
      break;
    case TGeoMaterial::kMatStateLiquid:
      state = kStateLiquid;
      break;
    case TGeoMaterial::kMatStateGas:
      state = kStateGas;
      break;
    default:
    case TGeoMaterial::kMatStateUndefined:
      state = kStateUndefined;
      break;
    }
    printout(lvl,"Geant4Material","+++ Setting up material %s", name.c_str());
    if ( material->IsMixture() )  {
      double A_total = 0.0;
      double W_total = 0.0;
      TGeoMixture* mix = (TGeoMixture*) material;
      int    nElements = mix->GetNelements();
      mat = new G4Material(name, density, nElements, state, 
                           material->GetTemperature(), material->GetPressure());
      for (int i = 0; i < nElements; ++i)  {
        A_total += (mix->GetAmixt())[i];
        W_total += (mix->GetWmixt())[i];
      }
      for (int i = 0; i < nElements; ++i) {
        TGeoElement* e = mix->GetElement(i);
        G4Element* g4e = (G4Element*) handleElement(e->GetName(), Atom(e));
        if (!g4e) {
          printout(ERROR, name, 
                   "Missing element component %s for material %s. A=%f W=%f", 
                   e->GetName(), mix->GetName(), A_total, W_total);
        }
        //mat->AddElement(g4e, (mix->GetAmixt())[i] / A_total);
        mat->AddElement(g4e, (mix->GetWmixt())[i] / W_total);
      }
    }
    else {
      double z = material->GetZ(), a = material->GetA();
      if ( z < 1.0000001 ) z = 1.0;
      if ( a < 0.5000001 ) a = 1.0;
      mat = new G4Material(name, z, a, density, state, 
                           material->GetTemperature(), material->GetPressure());
    }
    string plugin_name;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
    /// Attach the material properties if any
    G4MaterialPropertiesTable* tab = 0;
    TListIter propIt(&material->GetProperties());
    for(TObject* obj=propIt.Next(); obj; obj = propIt.Next())  {
      string       exc_str;
      TNamed*      named  = (TNamed*)obj;
      TGDMLMatrix* matrix = info.manager->GetGDMLMatrix(named->GetTitle());
      const char*  cptr   = ::strstr(matrix->GetName(), GEANT4_TAG_IGNORE);
      if ( 0 != cptr )   {
        printout(INFO,name,"++ Ignore property %s [%s]. Not Suitable for Geant4.",
                 matrix->GetName(), matrix->GetTitle());
        continue;
      }
      cptr = ::strstr(matrix->GetTitle(), GEANT4_TAG_IGNORE);
      if ( 0 != cptr )   {
        printout(INFO,name,"++ Ignore property %s [%s]. Not Suitable for Geant4.",
                 matrix->GetName(), matrix->GetTitle());
        continue;
      }
      Geant4GeometryInfo::PropertyVector* v =
        (Geant4GeometryInfo::PropertyVector*)handleMaterialProperties(matrix);
      if ( 0 == v )   {
        except("Geant4Converter", "++ FAILED to create G4 material %s [Cannot convert property:%s]",
               material->GetName(), named->GetName());
      }
      if ( 0 == tab )  {
        tab = new G4MaterialPropertiesTable();
        mat->SetMaterialPropertiesTable(tab);
      }
      int idx = -1;
      try   {
        idx = tab->GetPropertyIndex(named->GetName());
      }
      catch(const std::exception& e)   {
        exc_str = e.what();
        idx = -1;
      }
      catch(...)   {
        idx = -1;
      }
      if ( idx < 0 )   {
        printout(ERROR, "Geant4Converter",
                 "++ UNKNOWN Geant4 Property: %-20s %s [IGNORED]",
                 exc_str.c_str(), named->GetName());
        continue;
      }
      // We need to convert the property from TGeo units to Geant4 units
      auto conv = g4PropertyConversion(idx);
      vector<double> bins(v->bins), vals(v->values);
      for(std::size_t i=0, count=bins.size(); i<count; ++i)
        bins[i] *= conv.first, vals[i] *= conv.second;

      G4MaterialPropertyVector* vec =
        new G4MaterialPropertyVector(&bins[0], &vals[0], bins.size());
      tab->AddProperty(named->GetName(), vec);
      printout(lvl, name, "++      Property: %-20s [%ld x %ld] -> %s ",
               named->GetName(), matrix->GetRows(), matrix->GetCols(), named->GetTitle());
      for(std::size_t i=0, count=v->bins.size(); i<count; ++i)
        printout(lvl, name, "  Geant4: %s %8.3g [MeV]  TGeo: %8.3g [GeV] Conversion: %8.3g",
                 named->GetName(), bins[i], v->bins[i], conv.first);
    }

    /// Attach the material properties if any
    TListIter cpropIt(&material->GetConstProperties());
    for(TObject* obj=cpropIt.Next(); obj; obj = cpropIt.Next())  {
      string  exc_str;
      Bool_t     err = kFALSE;
      TNamed*  named = (TNamed*)obj;

      const char*  cptr = ::strstr(named->GetName(), GEANT4_TAG_IGNORE);
      if ( 0 != cptr )   {
        printout(INFO, name, "++ Ignore CONST property %s [%s].",
                 named->GetName(), named->GetTitle());
        continue;
      }
      cptr = ::strstr(named->GetTitle(), GEANT4_TAG_IGNORE);
      if ( 0 != cptr )   {
        printout(INFO, name,"++ Ignore CONST property %s [%s].",
                 named->GetName(), named->GetTitle());
        continue;
      }
      cptr = ::strstr(named->GetName(), GEANT4_TAG_PLUGIN);
      if ( 0 != cptr )   {
        printout(INFO, name, "++ Ignore CONST property %s [%s]  --> Plugin.",
                 named->GetName(), named->GetTitle());
	plugin_name = named->GetTitle();
        continue;
      }

      double v = info.manager->GetProperty(named->GetTitle(),&err);
      if ( err != kFALSE )   {
        except(name,
               "++ FAILED to create G4 material %s "
               "[Cannot convert const property: %s]",
               material->GetName(), named->GetName());
      }
      if ( 0 == tab )  {
        tab = new G4MaterialPropertiesTable();
        mat->SetMaterialPropertiesTable(tab);
      }
      int idx = -1;
      try   {
        idx = tab->GetConstPropertyIndex(named->GetName());
      }
      catch(const std::exception& e)   {
        exc_str = e.what();
        idx = -1;
      }
      catch(...)   {
        idx = -1;
      }
      if ( idx < 0 )   {
        printout(ERROR, name,
                 "++ UNKNOWN Geant4 CONST Property: %-20s %s [IGNORED]",
                 exc_str.c_str(), named->GetName());
        continue;
      }
      // We need to convert the property from TGeo units to Geant4 units
      double conv = g4ConstPropertyConversion(idx);
      printout(lvl, name, "++      CONST Property: %-20s %g ", named->GetName(), v);
      tab->AddConstProperty(named->GetName(), v * conv);
    }
#endif
    auto* ionization = mat->GetIonisation();
    stringstream str;
    str << (*mat) << endl;
    if ( ionization )
      str << "              log(MEE): " << ionization->GetLogMeanExcEnergy();
    else
      str << "              log(MEE): UNKNOWN";
    printout(lvl, name, "++ Created G4 material %s", str.str().c_str());
    if ( !plugin_name.empty() )    {
      /// Call plugin to create extended material if requested
      Detector* det = const_cast<Detector*>(&m_detDesc);
      G4Material* extended_mat = PluginService::Create<G4Material*>(plugin_name, det, medium, mat);
      if ( !extended_mat )   {
	except("G4Cnv::material["+name+"]","++ FATAL Failed to call plugin to create material.");
      }
      mat = extended_mat;
    }
    info.g4Materials[medium] = mat;
  }
  return mat;
}

/// Dump solid in GDML format to output stream
void* Geant4Converter::handleSolid(const string& name, const TGeoShape* shape) const {
  G4VSolid* solid = 0;
  if ( shape ) {
    if ( 0 != (solid = data().g4Solids[shape]) )   {
      return solid;
    }
    TClass*    isa = shape->IsA();
    PrintLevel lvl = debugShapes ? ALWAYS : outputLevel;
    if (isa == TGeoShapeAssembly::Class()) {
      // Assemblies have no corresponding 'shape' in Geant4. Ignore the shape translation.
      // It does not harm, since this 'shape' is never accessed afterwards.
      data().g4Solids[shape] = solid = convertShape<TGeoShapeAssembly>(shape);
      return solid;
    }
    else if (isa == TGeoBBox::Class())
      solid = convertShape<TGeoBBox>(shape);
    else if (isa == TGeoTube::Class())
      solid = convertShape<TGeoTube>(shape);
    else if (isa == TGeoTubeSeg::Class())
      solid = convertShape<TGeoTubeSeg>(shape);
    else if (isa == TGeoCtub::Class())
      solid = convertShape<TGeoCtub>(shape);
    else if (isa == TGeoEltu::Class())
      solid = convertShape<TGeoEltu>(shape);
    else if (isa == TwistedTubeObject::Class())
      solid = convertShape<TwistedTubeObject>(shape);
    else if (isa == TGeoTrd1::Class())
      solid = convertShape<TGeoTrd1>(shape);
    else if (isa == TGeoTrd2::Class())
      solid = convertShape<TGeoTrd2>(shape);
    else if (isa == TGeoHype::Class())
      solid = convertShape<TGeoHype>(shape);
    else if (isa == TGeoXtru::Class())
      solid = convertShape<TGeoXtru>(shape);
    else if (isa == TGeoPgon::Class())
      solid = convertShape<TGeoPgon>(shape);
    else if (isa == TGeoPcon::Class())
      solid = convertShape<TGeoPcon>(shape);
    else if (isa == TGeoCone::Class())
      solid = convertShape<TGeoCone>(shape);
    else if (isa == TGeoConeSeg::Class())
      solid = convertShape<TGeoConeSeg>(shape);
    else if (isa == TGeoParaboloid::Class())
      solid = convertShape<TGeoParaboloid>(shape);
    else if (isa == TGeoSphere::Class())
      solid = convertShape<TGeoSphere>(shape);
    else if (isa == TGeoTorus::Class())
      solid = convertShape<TGeoTorus>(shape);
    else if (isa == TGeoTrap::Class())
      solid = convertShape<TGeoTrap>(shape);
    else if (isa == TGeoArb8::Class()) 
      solid = convertShape<TGeoArb8>(shape);
#if ROOT_VERSION_CODE > ROOT_VERSION(6,21,0)
    else if (isa == TGeoTessellated::Class()) 
      solid = convertShape<TGeoTessellated>(shape);
#endif
    else if (isa == TGeoScaledShape::Class())  {
      TGeoScaledShape* sh   = (TGeoScaledShape*) shape;
      TGeoShape*       sol  = sh->GetShape();
      const double*    vals = sh->GetScale()->GetScale();
      G4Scale3D        scal(vals[0], vals[1], vals[2]);
      G4VSolid* g4solid = (G4VSolid*)handleSolid(sol->GetName(), sol);
      if ( scal.xx()>0e0 && scal.yy()>0e0 && scal.zz()>0e0 )
        solid = new G4ScaledSolid(sh->GetName(), g4solid, scal);
      else
        solid = new G4ReflectedSolid(g4solid->GetName()+"_refl", g4solid, scal);
    }
    else if ( isa == TGeoCompositeShape::Class() )   {
      const TGeoCompositeShape* sh = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = sh->GetBoolNode();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      TGeoMatrix* matrix = boolean->GetRightMatrix();
      G4VSolid* left  = (G4VSolid*) handleSolid(name + "_left", boolean->GetLeftShape());
      G4VSolid* right = (G4VSolid*) handleSolid(name + "_right", boolean->GetRightShape());
      
      if (!left) {
        except("Geant4Converter","++ No left Geant4 Solid present for composite shape: %s",name.c_str());
      }
      if (!right) {
        except("Geant4Converter","++ No right Geant4 Solid present for composite shape: %s",name.c_str());
      }

      TGeoShape* ls = boolean->GetLeftShape();
      TGeoShape* rs = boolean->GetRightShape();
      if (strcmp(ls->ClassName(), "TGeoScaledShape") == 0 &&
          strcmp(rs->ClassName(), "TGeoBBox") == 0) {
        if (strcmp(((TGeoScaledShape *)ls)->GetShape()->ClassName(), "TGeoSphere") == 0) {
          if (oper == TGeoBoolNode::kGeoIntersection) {
            TGeoScaledShape* lls = (TGeoScaledShape *)ls;
            TGeoBBox* rrs = (TGeoBBox*)rs;
            double sx     = lls->GetScale()->GetScale()[0];
            double sy     = lls->GetScale()->GetScale()[1];
            double radius = ((TGeoSphere *)lls->GetShape())->GetRmax();
            double dz     = rrs->GetDZ();
            double zorig  = rrs->GetOrigin()[2];
            double zcut2  = dz + zorig;
            double zcut1  = 2 * zorig - zcut2;
            solid = new G4Ellipsoid(name,
                                    sx * radius * CM_2_MM,
                                    sy * radius * CM_2_MM,
                                    radius * CM_2_MM,
                                    zcut1 * CM_2_MM,
                                    zcut2 * CM_2_MM);
            data().g4Solids[shape] = solid;
            return solid;
          }
        }
      }

      if ( matrix->IsRotation() ) {
        G4Transform3D transform;
	g4Transform(matrix, transform);
        if (oper == TGeoBoolNode::kGeoSubtraction)
          solid = new G4SubtractionSolid(name, left, right, transform);
        else if (oper == TGeoBoolNode::kGeoUnion)
          solid = new G4UnionSolid(name, left, right, transform);
        else if (oper == TGeoBoolNode::kGeoIntersection)
          solid = new G4IntersectionSolid(name, left, right, transform);
      }
      else {
        const Double_t *t = matrix->GetTranslation();
        G4ThreeVector transform(t[0] * CM_2_MM, t[1] * CM_2_MM, t[2] * CM_2_MM);
        if (oper == TGeoBoolNode::kGeoSubtraction)
          solid = new G4SubtractionSolid(name, left, right, 0, transform);
        else if (oper == TGeoBoolNode::kGeoUnion)
          solid = new G4UnionSolid(name, left, right, 0, transform);
        else if (oper == TGeoBoolNode::kGeoIntersection)
          solid = new G4IntersectionSolid(name, left, right, 0, transform);
      }
    }

    if ( !solid )
      except("Geant4Converter","++ Failed to handle unknown solid shape: %s of type %s",
             name.c_str(), isa->GetName());
    printout(lvl,"Geant4Converter","++ Successessfully converted shape [%p] of type:%s to %s.",
             solid,isa->GetName(),typeName(typeid(*solid)).c_str());
    data().g4Solids[shape] = solid;
  }
  return solid;
}

/// Dump logical volume in GDML format to output stream
void* Geant4Converter::handleVolume(const string& name, const TGeoVolume* volume) const {
  Geant4GeometryInfo& info = data();
  PrintLevel lvl = debugVolumes ? ALWAYS : outputLevel;
  Geant4GeometryMaps::VolumeMap::const_iterator volIt = info.g4Volumes.find(volume);
  Volume     _v(volume);
  if ( _v.testFlagBit(Volume::VETO_SIMU) )  {
    printout(lvl, "Geant4Converter", "++ Volume %s not converted [Veto'ed for simulation]",volume->GetName());
    return nullptr;
  }
  else if (volIt == info.g4Volumes.end() ) {
    string      n       = volume->GetName();
    TGeoMedium* med     = volume->GetMedium();
    TGeoShape*  sh      = volume->GetShape();
    G4VSolid*   solid   = (G4VSolid*) handleSolid(sh->GetName(), sh);
    bool        is_assembly = sh->IsA() == TGeoShapeAssembly::Class() || volume->IsA() == TGeoVolumeAssembly::Class();

    printout(lvl, "Geant4Converter", "++ Convert Volume %-32s: %p %s/%s assembly:%s",
             n.c_str(), volume, sh->IsA()->GetName(), volume->IsA()->GetName(), yes_no(is_assembly));
    
    if ( is_assembly ) {
      return nullptr;
    }
    Region        reg    = _v.region();
    LimitSet      lim    = _v.limitSet();
    VisAttr       vis    = _v.visAttributes();
    G4Region*     region = reg.isValid() ? info.g4Regions[reg] : nullptr;
    G4UserLimits* limits = lim.isValid() ? info.g4Limits[lim]  : nullptr;
    G4Material*   medium = (G4Material*) handleMaterial(med->GetName(), Material(med));
    /// Check all pre-conditions
    if ( !solid )   {
      except("G4Converter","++ No Geant4 Solid present for volume:" + n);
    }
    else if ( !medium )   {
      except("G4Converter","++ No Geant4 material present for volume:" + n);
    }
    else if ( reg.isValid() && !region )  {
      except("G4Cnv::volume["+name+"]"," ++ Failed to access Geant4 region %s.",reg.name());
    }
    else if ( lim.isValid() && !limits )  {
      except("G4Cnv::volume["+name+"]","++ FATAL Failed to access Geant4 user limits %s.",lim.name());
    }
    else if ( limits )   {
      printout(lvl, "Geant4Converter", "++ Volume     + Apply LIMITS settings:%-24s to volume %s.",
               lim.name(), _v.name());
    }

    G4LogicalVolume* g4vol = nullptr;
    if ( _v.hasProperties() && !_v.getProperty(GEANT4_TAG_PLUGIN,"").empty() )   {
      Detector* det = const_cast<Detector*>(&m_detDesc); 
      string plugin = _v.getProperty(GEANT4_TAG_PLUGIN,"");
      g4vol = PluginService::Create<G4LogicalVolume*>(plugin, det, _v, solid, medium);
      if ( !g4vol )    {
	except("G4Cnv::volume["+name+"]","++ FATAL Failed to call plugin to create logical volume.");
      }
    }
    else  {
      g4vol = new G4LogicalVolume(solid, medium, n, nullptr, nullptr, nullptr);
    }

    if ( limits )   {
      g4vol->SetUserLimits(limits);
    }
    if ( region )   {
      printout(lvl, "Geant4Converter", "++ Volume     + Apply REGION settings: %s to volume %s.",
               reg.name(), _v.name());
      g4vol->SetRegion(region);
      region->AddRootLogicalVolume(g4vol);
    }
    G4VisAttributes* vattr = vis.isValid() ? (G4VisAttributes*)handleVis(vis.name(), vis) : nullptr;
    if ( vattr )   {
      g4vol->SetVisAttributes(vattr);
    }
    info.g4Volumes[volume] = g4vol;
    printout(lvl, "Geant4Converter", "++ Volume     + %s converted: %p ---> G4: %p", n.c_str(), volume, g4vol);
  }
  return nullptr;
}

/// Dump logical volume in GDML format to output stream
void* Geant4Converter::collectVolume(const string& /* name */, const TGeoVolume* volume) const {
  Geant4GeometryInfo& info = data();
  Volume              _v(volume);
  Region              reg = _v.region();
  LimitSet            lim = _v.limitSet();
  SensitiveDetector   det = _v.sensitiveDetector();

  if ( lim.isValid() )
    info.limits[lim].insert(volume);
  if ( reg.isValid() )
    info.regions[reg].insert(volume);
  if ( det.isValid() )
    info.sensitives[det].insert(volume);
  return (void*)volume;
}

/// Dump volume placement in GDML format to output stream
void* Geant4Converter::handleAssembly(const string& name, const TGeoNode* node) const {
  TGeoVolume* mot_vol = node->GetVolume();
  PrintLevel lvl = debugVolumes ? ALWAYS : outputLevel;
  if ( mot_vol->IsA() != TGeoVolumeAssembly::Class() )    {
    return nullptr;
  }
  Volume _v(mot_vol);
  if ( _v.testFlagBit(Volume::VETO_SIMU) )  {
    printout(lvl, "Geant4Converter", "++ AssemblyNode %s not converted [Veto'ed for simulation]",node->GetName());
    return nullptr;
  }
  Geant4GeometryInfo& info = data();
  Geant4AssemblyVolume* g4 = info.g4AssemblyVolumes[node];
  if ( g4 )  {
    printout(ALWAYS, "Geant4Converter", "+++ Assembly: **** : Re-using existing assembly: %s",node->GetName());
  }
  if ( !g4 )  {
    g4 = new Geant4AssemblyVolume();
    for(Int_t i=0; i < mot_vol->GetNdaughters(); ++i)   {
      TGeoNode*     dau     = mot_vol->GetNode(i);
      TGeoVolume*   dau_vol = dau->GetVolume();
      TGeoMatrix*   tr      = dau->GetMatrix();
      G4Transform3D transform;

      g4Transform(tr, transform);
      if ( is_left_handed(tr) )   {
        G4Scale3D     scale;
        G4Rotate3D    rot;
        G4Translate3D trans;
        transform.getDecomposition(scale, rot, trans);
        printout(debugReflections ? ALWAYS : lvl, "Geant4Converter",
                 "++ Placing reflected ASSEMBLY. dau:%s to mother %s "
                 "Tr:x=%8.1f y=%8.1f z=%8.1f   Scale:x=%4.2f y=%4.2f z=%4.2f",
                 dau_vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz(),
                 scale.xx(), scale.yy(), scale.zz());
      }

      if ( dau_vol->IsA() == TGeoVolumeAssembly::Class() )  {
        Geant4GeometryMaps::AssemblyMap::iterator ia = info.g4AssemblyVolumes.find(dau);
        if ( ia == info.g4AssemblyVolumes.end() )  {
          printout(FATAL, "Geant4Converter", "+++ Invalid child assembly at %s : %d  parent: %s child:%s",
                   __FILE__, __LINE__, name.c_str(), dau->GetName());
          delete g4;
          return nullptr;
        }
        g4->placeAssembly(dau, (*ia).second, transform);
        printout(lvl, "Geant4Converter", "+++ Assembly: AddPlacedAssembly : dau:%s "
                 "to mother %s Tr:x=%8.3f y=%8.3f z=%8.3f",
                 dau_vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
      }
      else   {
        Geant4GeometryMaps::VolumeMap::iterator iv = info.g4Volumes.find(dau_vol);
        if ( iv == info.g4Volumes.end() )  {
          printout(FATAL,"Geant4Converter", "+++ Invalid child volume at %s : %d  parent: %s child:%s",
                   __FILE__, __LINE__, name.c_str(), dau->GetName());
          except("Geant4Converter", "+++ Invalid child volume at %s : %d  parent: %s child:%s",
                 __FILE__, __LINE__, name.c_str(), dau->GetName());
        }
        g4->placeVolume(dau,(*iv).second, transform);
        printout(lvl, "Geant4Converter", "+++ Assembly: AddPlacedVolume : dau:%s "
                 "to mother %s Tr:x=%8.3f y=%8.3f z=%8.3f",
                 dau_vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
      }
    }
    info.g4AssemblyVolumes[node] = g4;
  }
  return g4;
}

/// Dump volume placement in GDML format to output stream
void* Geant4Converter::handlePlacement(const string& name, const TGeoNode* node) const {
  Geant4GeometryInfo& info = data();
  PrintLevel lvl = debugPlacements ? ALWAYS : outputLevel;
  Geant4GeometryMaps::PlacementMap::const_iterator g4it = info.g4Placements.find(node);
  G4VPhysicalVolume* g4 = (g4it == info.g4Placements.end()) ? 0 : (*g4it).second;
  TGeoVolume* vol = node->GetVolume();
  Volume _v(vol);

  if ( _v.testFlagBit(Volume::VETO_SIMU) )  {
    printout(lvl, "Geant4Converter", "++ Placement %s not converted [Veto'ed for simulation]",node->GetName());
    return nullptr;
  }
  //g4 = nullptr;
  if ( !g4 ) {
    TGeoVolume* mot_vol = node->GetMotherVolume();
    TGeoMatrix* tr = node->GetMatrix();
    if ( !tr ) {
      except("Geant4Converter",
             "++ Attempt to handle placement without transformation:%p %s of type %s vol:%p",
             node, node->GetName(), node->IsA()->GetName(), vol);
    }
    else if (0 == vol) {
      except("Geant4Converter", "++ Unknown G4 volume:%p %s of type %s ptr:%p",
             node, node->GetName(), node->IsA()->GetName(), vol);
    }
    else {
      int           copy               = node->GetNumber();
      bool          node_is_reflected  = is_left_handed(tr);
      bool          node_is_assembly   = vol->IsA() == TGeoVolumeAssembly::Class();
      bool          mother_is_assembly = mot_vol ? mot_vol->IsA() == TGeoVolumeAssembly::Class() : false;
      G4Transform3D transform;
      Geant4GeometryMaps::VolumeMap::const_iterator volIt = info.g4Volumes.find(mot_vol);

      g4Transform(tr, transform);
      if ( mother_is_assembly )   {
        //
        // Mother is an assembly:
        // Nothing to do here, because:
        // -- placed volumes were already added before in "handleAssembly"
        // -- imprint cannot be made, because this requires a logical volume as a mother
        //
        printout(lvl, "Geant4Converter", "+++ Assembly: **** : dau:%s "
                 "to mother %s Tr:x=%8.3f y=%8.3f z=%8.3f",
                 vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
        return nullptr;
      }
      G4Scale3D        scale;
      G4Rotate3D       rot;
      G4Translate3D    trans;
      transform.getDecomposition(scale, rot, trans);
      if ( node_is_assembly )   {
        //
        // Node is an assembly:
        // Imprint the assembly. The mother MUST already be transformed.
        //
        printout(lvl, "Geant4Converter", "++ Assembly: makeImprint: dau:%-12s %s in mother %-12s "
                 "Tr:x=%8.1f y=%8.1f z=%8.1f   Scale:x=%4.2f y=%4.2f z=%4.2f",
                 node->GetName(), node_is_reflected ? "(REFLECTED)" : "",
                 mot_vol ? mot_vol->GetName() : "<unknown>",
                 transform.dx(), transform.dy(), transform.dz(),
                 scale.xx(), scale.yy(), scale.zz());
        Geant4AssemblyVolume* ass = (Geant4AssemblyVolume*)info.g4AssemblyVolumes[node];
        Geant4AssemblyVolume::Chain chain;
        chain.emplace_back(node);
        ass->imprint(*this, node, chain, ass, (*volIt).second, transform, copy, checkOverlaps);
        return nullptr;
      }
      else if ( node != info.manager->GetTopNode() && volIt == info.g4Volumes.end() )  {
        throw logic_error("Geant4Converter: Invalid mother volume found!");
      }
      PlacedVolume pv(node);
      const auto*  pv_data = pv.data();
      G4LogicalVolume* g4vol = info.g4Volumes[vol];
      G4LogicalVolume* g4mot = info.g4Volumes[mot_vol];
      G4PhysicalVolumesPair pvPlaced  { nullptr, nullptr };

      if ( pv_data && pv_data->params && (pv_data->params->flags&Volume::REPLICATED) )   {
	EAxis  axis = kUndefined;
	double width = 0e0, offset = 0e0;
	auto flags = pv_data->params->flags;
	auto count = pv_data->params->trafo1D.second;
	const auto& start = pv_data->params->start.Translation().Vect();
	const auto& delta = pv_data->params->trafo1D.first.Translation().Vect();

	if ( flags&Volume::X_axis )
	  { axis = kXAxis; width = delta.X(); offset = start.X(); }
	else if ( flags&Volume::Y_axis )
	  { axis = kYAxis; width = delta.Y(); offset = start.Y(); }
	else if ( flags&Volume::Z_axis )
	  { axis = kZAxis; width = delta.Z(); offset = start.Z(); }
	else
	  except("Geant4Converter",
		 "++ Replication around unknown axis is not implemented. flags: %16X", flags);
	printout(INFO,"Geant4Converter","++ Replicate: Axis: %ld Count: %ld offset: %f width: %f",
		 axis, count, offset, width);
	auto* g4pv = new G4PVReplica(name,      // its name
				     g4vol,     // its logical volume
				     g4mot,     // its mother (logical) volume
				     axis,      // its replication axis
				     count,     // Number of replicas
				     width,     // Distance between 2 replicas
				     offset);   // Placement offset in axis direction
	pvPlaced = { g4pv, nullptr };
#if 0
	pvPlaced =
	  G4ReflectionFactory::Instance()->Replicate(name,      // its name
						     g4vol,     // its logical volume
						     g4mot,     // its mother (logical) volume
						     axis,      // its replication axis
						     count,     // Number of replicas
						     width,     // Distance between 2 replicas
						     offset);   // Placement offset in axis direction
	/// Update replica list to avoid additional conversions...
	auto* g4pv = pvPlaced.second ? pvPlaced.second : pvPlaced.first;
#endif
	for( auto& handle : pv_data->params->placements )
	  info.g4Placements[handle.ptr()] = g4pv;
      }
      else if ( pv_data && pv_data->params )   {
	auto*  g4par = new Geant4PlacementParameterisation(pv);
	auto*  g4pv  = new G4PVParameterised(name,              // its name
					     g4vol,             // its logical volume
					     g4mot,             // its mother (logical) volume
					     g4par->axis(),     // its replication axis
					     g4par->count(),    // Number of replicas
					     g4par);            // G4 parametrization
	pvPlaced = { g4pv, nullptr };
	/// Update replica list to avoid additional conversions...
	for( auto& handle : pv_data->params->placements )
	  info.g4Placements[handle.ptr()] = g4pv;
      }
      else    {
	pvPlaced =
	  G4ReflectionFactory::Instance()->Place(transform,     // no rotation
						 name,          // its name
						 g4vol,         // its logical volume
						 g4mot,         // its mother (logical) volume
						 false,         // no boolean operations
						 copy,          // its copy number
						 checkOverlaps);
      }
      printout(debugReflections||debugPlacements ? ALWAYS : lvl, "Geant4Converter",
               "++ Place %svolume %-12s in mother %-12s "
               "Tr:x=%8.1f y=%8.1f z=%8.1f   Scale:x=%4.2f y=%4.2f z=%4.2f",
               node_is_reflected ? "REFLECTED " : "", _v.name(),
               mot_vol ? mot_vol->GetName() : "<unknown>",
               transform.dx(), transform.dy(), transform.dz(),
               scale.xx(), scale.yy(), scale.zz());
      // First 2 cases can be combined.
      // Leave them separated for debugging G4ReflectionFactory for now...
      if ( node_is_reflected  && !pvPlaced.second )
        return info.g4Placements[node] = pvPlaced.first;
      else if ( !node_is_reflected && !pvPlaced.second )
        return info.g4Placements[node] = pvPlaced.first;
      // Now deal with valid pvPlaced.second ...
      if ( node_is_reflected )
        return info.g4Placements[node] = pvPlaced.first;
      else if ( !node_is_reflected )
        return info.g4Placements[node] = pvPlaced.first;
      g4 = pvPlaced.second ? pvPlaced.second : pvPlaced.first;
    }
    info.g4Placements[node] = g4;
    printout(ERROR, "Geant4Converter", "++ DEAD code. Should not end up here!");
  }
  return g4;
}

/// Convert the geometry type region into the corresponding Geant4 object(s).
void* Geant4Converter::handleRegion(Region region, const set<const TGeoVolume*>& /* volumes */) const {
  G4Region* g4 = data().g4Regions[region];
  if ( !g4 ) {
    PrintLevel lvl = debugRegions ? ALWAYS : outputLevel;
    Region     r   = region;
    g4 = new G4Region(r.name());

    // create region info with storeSecondaries flag
    if( not r.wasThresholdSet() and r.storeSecondaries() ) {
      throw runtime_error("G4Region: StoreSecondaries is True, but no explicit threshold set:");
    }
    printout(lvl, "Geant4Converter", "++ Setting up region: %s", r.name());
    G4UserRegionInformation* info = new G4UserRegionInformation();
    info->region = r;
    info->threshold = r.threshold()*CLHEP::MeV/units::MeV;
    info->storeSecondaries = r.storeSecondaries();
    g4->SetUserInformation(info);

    printout(lvl, "Geant4Converter", "++ Converted region settings of:%s.", r.name());
    vector < string > &limits = r.limits();
    G4ProductionCuts* cuts = 0;
    // set production cut
    if( not r.useDefaultCut() ) {
      cuts = new G4ProductionCuts();
      cuts->SetProductionCut(r.cut()*CLHEP::mm/units::mm);
      printout(lvl, "Geant4Converter", "++ %s: Using default cut: %f [mm]",
               r.name(), r.cut()*CLHEP::mm/units::mm);
    }
    for( const auto& nam : limits )  {
      LimitSet ls = m_detDesc.limitSet(nam);
      if (ls.isValid()) {
        const LimitSet::Set& cts = ls.cuts();
        for (const auto& c : cts )   {
          int pid = 0;
          if ( c.particles == "*" ) pid = -1;
          else if ( c.particles == "e-"     ) pid = idxG4ElectronCut;
          else if ( c.particles == "e+"     ) pid = idxG4PositronCut;
          else if ( c.particles == "e[+-]"  ) pid = -idxG4PositronCut-idxG4ElectronCut;
          else if ( c.particles == "e[-+]"  ) pid = -idxG4PositronCut-idxG4ElectronCut;
          else if ( c.particles == "gamma"  ) pid = idxG4GammaCut;
          else if ( c.particles == "proton" ) pid = idxG4ProtonCut;
          else throw runtime_error("G4Region: Invalid production cut particle-type:" + c.particles);
          if ( !cuts ) cuts = new G4ProductionCuts();
          if ( pid == -(idxG4PositronCut+idxG4ElectronCut) )  {
            cuts->SetProductionCut(c.value*CLHEP::mm/units::mm, idxG4PositronCut);
            cuts->SetProductionCut(c.value*CLHEP::mm/units::mm, idxG4ElectronCut);
          }
          else  {
            cuts->SetProductionCut(c.value*CLHEP::mm/units::mm, pid);
          }
          printout(lvl, "Geant4Converter", "++ %s: Set cut  [%s/%d] = %f [mm]",
                   r.name(), c.particles.c_str(), pid, c.value*CLHEP::mm/units::mm);
        }
        bool found = false;
        const auto& lm = data().g4Limits;
        for (const auto& j : lm )   {
          if (nam == j.first->GetName()) {
            g4->SetUserLimits(j.second);
            printout(lvl, "Geant4Converter", "++ %s: Set limits %s to region type %s",
                     r.name(), nam.c_str(), j.second->GetType().c_str());
            found = true;
            break;
          }
        }
        if ( found )   {
          continue;
        }
      }
      throw runtime_error("G4Region: Failed to resolve user limitset:" + nam);
    }
    /// Assign cuts to region if they were created
    if ( cuts ) g4->SetProductionCuts(cuts);
    data().g4Regions[region] = g4;
  }
  return g4;
}

/// Convert the geometry type LimitSet into the corresponding Geant4 object(s).
void* Geant4Converter::handleLimitSet(LimitSet limitset, const set<const TGeoVolume*>& /* volumes */) const {
  G4UserLimits* g4 = data().g4Limits[limitset];
  if ( !g4 ) {
    struct LimitPrint  {
      const LimitSet& ls;
      LimitPrint(const LimitSet& lset) : ls(lset) {}
      const LimitPrint& operator()(const std::string& pref, const Geant4UserLimits::Handler& h)  const {
        if ( !h.particleLimits.empty() )  {
          printout(ALWAYS,"Geant4Converter",
                   "+++ LimitSet: Limit %s.%s applied for particles:",ls.name(), pref.c_str());
          for(const auto& p : h.particleLimits)
            printout(ALWAYS,"Geant4Converter","+++ LimitSet:    Particle type: %-18s PDG: %-6d : %f",
                     p.first->GetParticleName().c_str(), p.first->GetPDGEncoding(), p.second);
        }
        else  {
          printout(ALWAYS,"Geant4Converter",
                   "+++ LimitSet: Limit %s.%s NOT APPLIED.",ls.name(), pref.c_str());
        }
        return *this;
      }
    };
    Geant4UserLimits* limits = new Geant4UserLimits(limitset);
    g4 = limits;
    if ( debugRegions )    {
      LimitPrint print(limitset);
      print("maxTime",    limits->maxTime)
        ("minEKine",      limits->minEKine)
        ("minRange",      limits->minRange)
        ("maxStepLength", limits->maxStepLength)
        ("maxTrackLength",limits->maxTrackLength);
    }
    data().g4Limits[limitset] = g4;
  }
  return g4;
}

/// Convert the geometry visualisation attributes to the corresponding Geant4 object(s).
void* Geant4Converter::handleVis(const string& /* name */, VisAttr attr) const {
  Geant4GeometryInfo& info = data();
  G4VisAttributes*    g4   = info.g4Vis[attr];
  if ( !g4 ) {
    float red = 0, green = 0, blue = 0;
    int   style = attr.lineStyle();
    attr.rgb(red, green, blue);
    g4 = new G4VisAttributes(attr.visible(), G4Colour(red, green, blue, attr.alpha()));
    //g4->SetLineWidth(attr->GetLineWidth());
    g4->SetDaughtersInvisible(!attr.showDaughters());
    if ( style == VisAttr::SOLID ) {
      g4->SetLineStyle(G4VisAttributes::unbroken);
      g4->SetForceWireframe(false);
      g4->SetForceSolid(true);
    }
    else if ( style == VisAttr::WIREFRAME || style == VisAttr::DASHED ) {
      g4->SetLineStyle(G4VisAttributes::dashed);
      g4->SetForceSolid(false);
      g4->SetForceWireframe(true);
    }
    info.g4Vis[attr] = g4;
  }
  return g4;
}

/// Handle the geant 4 specific properties
void Geant4Converter::handleProperties(Detector::Properties& prp) const {
  map < string, string > processors;
  static int s_idd = 9999999;
  for( const auto& [nam, vals] : prp ) {
    if ( nam.substr(0, 6) == "geant4" ) {
      auto id_it = vals.find("id");
      string id = (id_it == vals.end()) ? _toString(++s_idd,"%d") : (*id_it).second;
      processors.emplace(id, nam);
    }
  }
  for( const auto& p : processors ) {
    const GeoHandler* hdlr = this;
    const Detector::PropertyValues& vals = prp[p.second];
    string type = vals.find("type")->second;
    string tag  = type + "_Geant4_action";
    Detector* det = const_cast<Detector*>(&m_detDesc);
    long      res = PluginService::Create<long>(tag, det, hdlr, &vals);
    if ( 0 == res ) {
      throw runtime_error("Failed to locate plugin to interprete files of type"
                          " \"" + tag + "\" - no factory:" + type);
    }
    res = *(long*)res;
    if ( res != 1 ) {
      throw runtime_error("Failed to invoke the plugin " + tag + " of type " + type);
    }
    printout(outputLevel, "Geant4Converter", "+++++ Executed Successfully Geant4 setup module *%s*.", type.c_str());
  }
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
/// Convert the geometry type material into the corresponding Geant4 object(s).
void* Geant4Converter::handleMaterialProperties(TObject* mtx) const    {
  Geant4GeometryInfo& info   = data();
  TGDMLMatrix*        matrix = (TGDMLMatrix*)mtx;
  const char*         cptr   = ::strstr(matrix->GetName(), GEANT4_TAG_IGNORE);
  Geant4GeometryInfo::PropertyVector* g4 = info.g4OpticalProperties[matrix];

  if ( 0 != cptr )   {  // Check if the property should not be passed to Geant4
    printout(INFO,"Geant4MaterialProperties","++ Ignore property %s [%s].",
             matrix->GetName(), matrix->GetTitle());	     
    return nullptr;
  }
  cptr = ::strstr(matrix->GetTitle(), GEANT4_TAG_IGNORE);
  if ( 0 != cptr )   {  // Check if the property should not be passed to Geant4
    printout(INFO,"Geant4MaterialProperties","++ Ignore property %s [%s].",
             matrix->GetName(), matrix->GetTitle());
    return nullptr;
  }
  
  if ( !g4 )  {
    PrintLevel lvl = debugMaterials ? ALWAYS : outputLevel;
    g4 = new Geant4GeometryInfo::PropertyVector();
    std::size_t rows = matrix->GetRows();
    g4->name    = matrix->GetName();
    g4->title   = matrix->GetTitle();
    g4->bins.reserve(rows);
    g4->values.reserve(rows);
    for( std::size_t i=0; i<rows; ++i )   {
      g4->bins.emplace_back(matrix->Get(i,0)  /*   *CLHEP::eV/units::eV   */);
      g4->values.emplace_back(matrix->Get(i,1));
    }
    printout(lvl, "Geant4Converter",
             "++ Successfully converted material property:%s : %s [%ld rows]",
             matrix->GetName(), matrix->GetTitle(), rows);
    info.g4OpticalProperties[matrix] = g4;
  }
  return g4;
}

static G4OpticalSurfaceFinish geant4_surface_finish(TGeoOpticalSurface::ESurfaceFinish f)   {
#define TO_G4_FINISH(x)  case TGeoOpticalSurface::kF##x : return x;
  switch(f)   {
    TO_G4_FINISH(polished);              // smooth perfectly polished surface
    TO_G4_FINISH(polishedfrontpainted);  // smooth top-layer (front) paint
    TO_G4_FINISH(polishedbackpainted);   // same is 'polished' but with a back-paint
 
    TO_G4_FINISH(ground);                // rough surface
    TO_G4_FINISH(groundfrontpainted);    // rough top-layer (front) paint
    TO_G4_FINISH(groundbackpainted);     // same as 'ground' but with a back-paint

    TO_G4_FINISH(polishedlumirrorair);   // mechanically polished surface, with lumirror
    TO_G4_FINISH(polishedlumirrorglue);  // mechanically polished surface, with lumirror & meltmount
    TO_G4_FINISH(polishedair);           // mechanically polished surface
    TO_G4_FINISH(polishedteflonair);     // mechanically polished surface, with teflon
    TO_G4_FINISH(polishedtioair);        // mechanically polished surface, with tio paint
    TO_G4_FINISH(polishedtyvekair);      // mechanically polished surface, with tyvek
    TO_G4_FINISH(polishedvm2000air);     // mechanically polished surface, with esr film
    TO_G4_FINISH(polishedvm2000glue);    // mechanically polished surface, with esr film & meltmount

    TO_G4_FINISH(etchedlumirrorair);     // chemically etched surface, with lumirror
    TO_G4_FINISH(etchedlumirrorglue);    // chemically etched surface, with lumirror & meltmount
    TO_G4_FINISH(etchedair);             // chemically etched surface
    TO_G4_FINISH(etchedteflonair);       // chemically etched surface, with teflon
    TO_G4_FINISH(etchedtioair);          // chemically etched surface, with tio paint
    TO_G4_FINISH(etchedtyvekair);        // chemically etched surface, with tyvek
    TO_G4_FINISH(etchedvm2000air);       // chemically etched surface, with esr film
    TO_G4_FINISH(etchedvm2000glue);      // chemically etched surface, with esr film & meltmount

    TO_G4_FINISH(groundlumirrorair);     // rough-cut surface, with lumirror
    TO_G4_FINISH(groundlumirrorglue);    // rough-cut surface, with lumirror & meltmount
    TO_G4_FINISH(groundair);             // rough-cut surface
    TO_G4_FINISH(groundteflonair);       // rough-cut surface, with teflon
    TO_G4_FINISH(groundtioair);          // rough-cut surface, with tio paint
    TO_G4_FINISH(groundtyvekair);        // rough-cut surface, with tyvek
    TO_G4_FINISH(groundvm2000air);       // rough-cut surface, with esr film
    TO_G4_FINISH(groundvm2000glue);      // rough-cut surface, with esr film & meltmount

    // for DAVIS model
    TO_G4_FINISH(Rough_LUT);             // rough surface
    TO_G4_FINISH(RoughTeflon_LUT);       // rough surface wrapped in Teflon tape
    TO_G4_FINISH(RoughESR_LUT);          // rough surface wrapped with ESR
    TO_G4_FINISH(RoughESRGrease_LUT);    // rough surface wrapped with ESR and coupled with opical grease
    TO_G4_FINISH(Polished_LUT);          // polished surface
    TO_G4_FINISH(PolishedTeflon_LUT);    // polished surface wrapped in Teflon tape
    TO_G4_FINISH(PolishedESR_LUT);       // polished surface wrapped with ESR
    TO_G4_FINISH(PolishedESRGrease_LUT); // polished surface wrapped with ESR and coupled with opical grease
    TO_G4_FINISH(Detector_LUT);          // polished surface with optical grease
  default:
    printout(ERROR,"Geant4Surfaces","++ Unknown finish style: %d [%s]. Assume polished!",
             int(f), TGeoOpticalSurface::FinishToString(f));
    return polished;
  }
#undef TO_G4_FINISH
}

static G4SurfaceType geant4_surface_type(TGeoOpticalSurface::ESurfaceType t)   {
#define TO_G4_TYPE(x)  case TGeoOpticalSurface::kT##x : return x;
  switch(t)   {
    TO_G4_TYPE(dielectric_metal);      // dielectric-metal interface
    TO_G4_TYPE(dielectric_dielectric); // dielectric-dielectric interface
    TO_G4_TYPE(dielectric_LUT);        // dielectric-Look-Up-Table interface
    TO_G4_TYPE(dielectric_LUTDAVIS);   // dielectric-Look-Up-Table DAVIS interface
    TO_G4_TYPE(dielectric_dichroic);   // dichroic filter interface
    TO_G4_TYPE(firsov);                // for Firsov Process
    TO_G4_TYPE(x_ray);                  // for x-ray mirror process
  default:
    printout(ERROR,"Geant4Surfaces","++ Unknown surface type: %d [%s]. Assume dielectric_metal!",
             int(t), TGeoOpticalSurface::TypeToString(t));
    return dielectric_metal;
  }
#undef TO_G4_TYPE
}

static G4OpticalSurfaceModel geant4_surface_model(TGeoOpticalSurface::ESurfaceModel surfMod)   {
#define TO_G4_MODEL(x)  case TGeoOpticalSurface::kM##x : return x;
  switch(surfMod)   {
    TO_G4_MODEL(glisur);   // original GEANT3 model
    TO_G4_MODEL(unified);  // UNIFIED model
    TO_G4_MODEL(LUT);      // Look-Up-Table model
    TO_G4_MODEL(DAVIS);    // DAVIS model
    TO_G4_MODEL(dichroic); // dichroic filter
  default:
    printout(ERROR,"Geant4Surfaces","++ Unknown surface model: %d [%s]. Assume glisur!",
             int(surfMod), TGeoOpticalSurface::ModelToString(surfMod));
    return glisur;
  }
#undef TO_G4_MODEL
}

/// Convert the optical surface to Geant4
void* Geant4Converter::handleOpticalSurface(TObject* surface) const    {
  TGeoOpticalSurface* optSurf    = (TGeoOpticalSurface*)surface;
  Geant4GeometryInfo& info = data();
  G4OpticalSurface*   g4   = info.g4OpticalSurfaces[optSurf];
  if ( !g4 ) {
    G4SurfaceType          type   = geant4_surface_type(optSurf->GetType());
    G4OpticalSurfaceModel  model  = geant4_surface_model(optSurf->GetModel());
    G4OpticalSurfaceFinish finish = geant4_surface_finish(optSurf->GetFinish());
    g4 = new G4OpticalSurface(optSurf->GetName(), model, finish, type, optSurf->GetValue());
    g4->SetSigmaAlpha(optSurf->GetSigmaAlpha());
    // not implemented: g4->SetPolish(s->GetPolish());
    printout(debugSurfaces ? ALWAYS : DEBUG, "Geant4Converter",
             "++ Created OpticalSurface: %-18s type:%s model:%s finish:%s",
             optSurf->GetName(),
             TGeoOpticalSurface::TypeToString(optSurf->GetType()),
             TGeoOpticalSurface::ModelToString(optSurf->GetModel()),
             TGeoOpticalSurface::FinishToString(optSurf->GetFinish()));
    G4MaterialPropertiesTable* tab = 0;
    TListIter it(&optSurf->GetProperties());
    for(TObject* obj = it.Next(); obj; obj = it.Next())  {
      string exc_str;
      TNamed*      named  = (TNamed*)obj;
      TGDMLMatrix* matrix = info.manager->GetGDMLMatrix(named->GetTitle());
      const char*  cptr   = ::strstr(matrix->GetName(), GEANT4_TAG_IGNORE);
      if ( 0 != cptr )  // Check if the property should not be passed to Geant4
        continue;

      if ( 0 == tab )  {
        tab = new G4MaterialPropertiesTable();
        g4->SetMaterialPropertiesTable(tab);
      }

      Geant4GeometryInfo::PropertyVector* v =
        (Geant4GeometryInfo::PropertyVector*)handleMaterialProperties(matrix);
      if ( !v )  {  // Error!
        except("Geant4OpticalSurface","++ Failed to convert opt.surface %s. Property table %s is not defined!",
               optSurf->GetName(), named->GetTitle());
      }
      int idx = -1;
      try   {
        idx = tab->GetPropertyIndex(named->GetName());
      }
      catch(const std::exception& e)   {
        exc_str = e.what();
        idx = -1;
      }
      catch(...)   {
        idx = -1;
      }
      if ( idx < 0 )   {
        printout(ERROR, "Geant4Converter",
                 "++ UNKNOWN Geant4 Property: %-20s %s [IGNORED]",
                 exc_str.c_str(), named->GetName());
        continue;
      }
      // We need to convert the property from TGeo units to Geant4 units
      auto conv = g4PropertyConversion(idx);
      vector<double> bins(v->bins), vals(v->values);
      for(std::size_t i=0, count=v->bins.size(); i<count; ++i)
        bins[i] *= conv.first, vals[i] *= conv.second;
      G4MaterialPropertyVector* vec = new G4MaterialPropertyVector(&bins[0], &vals[0], bins.size());
      tab->AddProperty(named->GetName(), vec);
      
      printout(debugSurfaces ? ALWAYS : DEBUG, "Geant4Converter",
               "++       Property: %-20s [%ld x %ld] -->  %s",
               named->GetName(), matrix->GetRows(), matrix->GetCols(), named->GetTitle());
      for(std::size_t i=0, count=v->bins.size(); i<count; ++i)
        printout(debugSurfaces ? ALWAYS : DEBUG, named->GetName(),
                 "  Geant4: %8.3g [MeV]  TGeo: %8.3g [GeV] Conversion: %8.3g",
                 bins[i], v->bins[i], conv.first);
    }
    info.g4OpticalSurfaces[optSurf] = g4;
  }
  return g4;
}

/// Convert the skin surface to Geant4
void* Geant4Converter::handleSkinSurface(TObject* surface) const   {
  TGeoSkinSurface*    surf = (TGeoSkinSurface*)surface;
  Geant4GeometryInfo& info = data();
  G4LogicalSkinSurface* g4 = info.g4SkinSurfaces[surf];
  if ( !g4 ) {
    G4OpticalSurface* optSurf  = info.g4OpticalSurfaces[OpticalSurface(surf->GetSurface())];
    G4LogicalVolume*  v = info.g4Volumes[surf->GetVolume()];
    g4 = new G4LogicalSkinSurface(surf->GetName(), v, optSurf);
    printout(debugSurfaces ? ALWAYS : DEBUG, "Geant4Converter",
             "++ Created SkinSurface: %-18s  optical:%s",
             surf->GetName(), surf->GetSurface()->GetName());
    info.g4SkinSurfaces[surf] = g4;
  }
  return g4;
}

/// Convert the border surface to Geant4
void* Geant4Converter::handleBorderSurface(TObject* surface) const   {
  TGeoBorderSurface*    surf = (TGeoBorderSurface*)surface;
  Geant4GeometryInfo&   info = data();
  G4LogicalBorderSurface* g4 = info.g4BorderSurfaces[surf];
  if ( !g4 ) {
    G4OpticalSurface*  optSurf = info.g4OpticalSurfaces[OpticalSurface(surf->GetSurface())];
    G4VPhysicalVolume* n1 = info.g4Placements[surf->GetNode1()];
    G4VPhysicalVolume* n2 = info.g4Placements[surf->GetNode2()];
    g4 = new G4LogicalBorderSurface(surf->GetName(), n1, n2, optSurf);
    printout(debugSurfaces ? ALWAYS : DEBUG, "Geant4Converter",
             "++ Created BorderSurface: %-18s  optical:%s",
             surf->GetName(), surf->GetSurface()->GetName());
    info.g4BorderSurfaces[surf] = g4;
  }
  return g4;
}
#endif

/// Convert the geometry type SensitiveDetector into the corresponding Geant4 object(s).
void Geant4Converter::printSensitive(SensitiveDetector sens_det, const set<const TGeoVolume*>& /* volumes */) const {
  Geant4GeometryInfo&     info = data();
  set<const TGeoVolume*>& volset = info.sensitives[sens_det];
  SensitiveDetector       sd = sens_det;
  stringstream str;

  printout(INFO, "Geant4Converter", "++ SensitiveDetector: %-18s %-20s Hits:%-16s", sd.name(), ("[" + sd.type() + "]").c_str(),
           sd.hitsCollection().c_str());
  str << "                    | " << "Cutoff:" << setw(6) << left << sd.energyCutoff() << setw(5) << right << volset.size()
      << " volumes ";
  if (sd.region().isValid())
    str << " Region:" << setw(12) << left << sd.region().name();
  if (sd.limits().isValid())
    str << " Limits:" << setw(12) << left << sd.limits().name();
  str << ".";
  printout(INFO, "Geant4Converter", str.str().c_str());

  for (const auto i : volset )  {
    map<Volume, G4LogicalVolume*>::iterator v = info.g4Volumes.find(i);
    G4LogicalVolume* vol = (*v).second;
    str.str("");
    str << "                                   | " << "Volume:" << setw(24) << left << vol->GetName() << " "
        << vol->GetNoDaughters() << " daughters.";
    printout(INFO, "Geant4Converter", str.str().c_str());
  }
}

string printSolid(G4VSolid* sol) {
  stringstream str;
  if (typeid(*sol) == typeid(G4Box)) {
    const G4Box* b = (G4Box*) sol;
    str << "++ Box: x=" << b->GetXHalfLength() << " y=" << b->GetYHalfLength() << " z=" << b->GetZHalfLength();
  }
  else if (typeid(*sol) == typeid(G4Tubs)) {
    const G4Tubs* t = (const G4Tubs*) sol;
    str << " Tubs: Ri=" << t->GetInnerRadius() << " Ra=" << t->GetOuterRadius() << " z/2=" << t->GetZHalfLength() << " Phi="
        << t->GetStartPhiAngle() << "..." << t->GetDeltaPhiAngle();
  }
  return str.str();
}

/// Print G4 placement
void* Geant4Converter::printPlacement(const string& name, const TGeoNode* node) const {
  Geant4GeometryInfo& info = data();
  G4VPhysicalVolume*  g4   = info.g4Placements[node];
  G4LogicalVolume*    vol  = info.g4Volumes[node->GetVolume()];
  G4LogicalVolume*    mot  = info.g4Volumes[node->GetMotherVolume()];
  G4VSolid*           sol  = vol->GetSolid();
  G4ThreeVector       tr   = g4->GetObjectTranslation();
  G4VSensitiveDetector* sd = vol->GetSensitiveDetector();
  if ( !sd )  {
    return g4;
  }
  stringstream str;
  str << "G4Cnv::placement: + " << name << " No:" << node->GetNumber() << " Vol:" << vol->GetName() << " Solid:"
      << sol->GetName();
  printout(outputLevel, "G4Placement", str.str().c_str());
  str.str("");
  str << "                  |" << " Loc: x=" << tr.x() << " y=" << tr.y() << " z=" << tr.z();
  printout(outputLevel, "G4Placement", str.str().c_str());
  printout(outputLevel, "G4Placement", printSolid(sol).c_str());
  str.str("");
  str << "                  |" << " Ndau:" << vol->GetNoDaughters() << " physvols." << " Mat:" << vol->GetMaterial()->GetName()
      << " Mother:" << (char*) (mot ? mot->GetName().c_str() : "---");
  printout(outputLevel, "G4Placement", str.str().c_str());
  str.str("");
  str << "                  |" << " SD:" << sd->GetName();
  printout(outputLevel, "G4Placement", str.str().c_str());
  return g4;
}

namespace  {
  template <typename O, typename C, typename F> void handleRefs(const O* o, const C& c, F pmf) {
    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
      //(o->*pmf)((*i)->GetName(), *i);
      (o->*pmf)("", *i);
    }
  }

  template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf) {
    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
      (o->*pmf)((*i)->GetName(), *i);
    }
  }

  template <typename O, typename F> void handleArray(const O* o, const TObjArray* c, F pmf) {
    TObjArrayIter arr(c);
    for(TObject* i = arr.Next(); i; i=arr.Next())
      (o->*pmf)(i);
  }

  template <typename O, typename C, typename F> void handleMap(const O* o, const C& c, F pmf) {
    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i)
      (o->*pmf)((*i).first, (*i).second);
  }

  template <typename O, typename C, typename F> void handleRMap(const O* o, const C& c, F pmf) {
    for (typename C::const_reverse_iterator i = c.rbegin(); i != c.rend(); ++i)  {
      //cout << "Handle RMAP [ " << (*i).first << " ]" << endl;
      handle(o, (*i).second, pmf);
    }
  }
  template <typename O, typename C, typename F> void handleRMap_(const O* o, const C& c, F pmf) {
    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i)  {
      const auto& cc = (*i).second;
      for (const auto& j : cc)   {
        (o->*pmf)(j);
      }
    }
  }
}

/// Create geometry conversion
Geant4Converter& Geant4Converter::create(DetElement top) {
  Geant4GeometryInfo& geo = this->init();
  World wrld = top.world();
  m_data->clear();
  geo.manager = &wrld.detectorDescription().manager();
  collect(top, geo);
  checkOverlaps = false;
  // We do not have to handle defines etc.
  // All positions and the like are not really named.
  // Hence, start creating the G4 objects for materials, solids and log volumes.
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
  handleArray(this, geo.manager->GetListOfGDMLMatrices(), &Geant4Converter::handleMaterialProperties);
  handleArray(this, geo.manager->GetListOfOpticalSurfaces(), &Geant4Converter::handleOpticalSurface);
#endif
  
  handle(this,     geo.volumes, &Geant4Converter::collectVolume);
  handle(this,     geo.solids,  &Geant4Converter::handleSolid);
  printout(outputLevel, "Geant4Converter", "++ Handled %ld solids.", geo.solids.size());
  handleRefs(this, geo.vis,     &Geant4Converter::handleVis);
  printout(outputLevel, "Geant4Converter", "++ Handled %ld visualization attributes.", geo.vis.size());
  handleMap(this,  geo.limits,  &Geant4Converter::handleLimitSet);
  printout(outputLevel, "Geant4Converter", "++ Handled %ld limit sets.", geo.limits.size());
  handleMap(this,  geo.regions, &Geant4Converter::handleRegion);
  printout(outputLevel, "Geant4Converter", "++ Handled %ld regions.", geo.regions.size());
  handle(this,     geo.volumes, &Geant4Converter::handleVolume);
  printout(outputLevel, "Geant4Converter", "++ Handled %ld volumes.", geo.volumes.size());
  handleRMap(this, *m_data,     &Geant4Converter::handleAssembly);
  // Now place all this stuff appropriately
  handleRMap(this, *m_data,     &Geant4Converter::handlePlacement);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
  /// Handle concrete surfaces
  handleArray(this, geo.manager->GetListOfSkinSurfaces(),   &Geant4Converter::handleSkinSurface);
  handleArray(this, geo.manager->GetListOfBorderSurfaces(), &Geant4Converter::handleBorderSurface);
#endif
  //==================== Fields
  handleProperties(m_detDesc.properties());
  if ( printSensitives )  {
    handleMap(this, geo.sensitives, &Geant4Converter::printSensitive);
  }
  if ( printPlacements )  {
    handleRMap(this, *m_data, &Geant4Converter::printPlacement);
  }

  geo.setWorld(top.placement().ptr());
  geo.valid = true;
  printout(INFO, "Geant4Converter", "+++  Successfully converted geometry to Geant4.");
  return *this;
}
