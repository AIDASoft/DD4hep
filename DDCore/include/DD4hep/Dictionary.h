//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  LinkDef.h
//
//
//  Created by Pere Mato on 22/1/12.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_DICTIONARY_H
#define DD4HEP_DDCORE_DICTIONARY_H

// Framework include files
#include "DDParsers/Evaluator.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/VolumeManagerInterna.h"

#include "DD4hep/World.h"
#include "DD4hep/DD4hepUI.h"
#include "DD4hep/Callback.h"
#include "DD4hep/DetectorData.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/FieldTypes.h"

// C/C++ include files
#include <vector>
#include <map>
#include <string>

#include "TRint.h"
namespace dd4hep {
  namespace cond {}
  namespace align {}
  namespace detail {}
  void run_interpreter(const std::string& name)  {
    pair<int, char**> a(0,0);
    TRint app(name.c_str(), &a.first, a.second);
    app.Run();
  }
  XmlTools::Evaluator& evaluator();
  XmlTools::Evaluator& g4Evaluator();
}

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

#pragma link C++ namespace XmlTools;
#pragma link C++ class XmlTools::Evaluator;
#pragma link C++ function dd4hep::evaluator;
#pragma link C++ function dd4hep::g4Evaluator;

#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::detail;
#pragma link C++ namespace dd4hep::cond;
#pragma link C++ namespace dd4hep::align;
#pragma link C++ namespace dd4hep::DDSegmentation;

#ifndef __ROOTCLING__
template pair<unsigned int, string>;
template class dd4hep::Handle<dd4hep::NamedObject>;
template class pair< string, dd4hep::Handle<dd4hep::NamedObject> >;
template class map< string, dd4hep::Handle<dd4hep::NamedObject> >;
template class pair<dd4hep::Callback,unsigned long>;
#endif

#pragma link C++ class pair<unsigned int,string>+;
//#pragma link C++ class dd4hep::Callback+;
#pragma link C++ class pair<dd4hep::Callback,unsigned long>+;
#pragma link C++ class dd4hep::NamedObject+;
#pragma link C++ class dd4hep::Ref_t+;
#pragma link C++ class dd4hep::Handle<dd4hep::NamedObject>+;
#pragma link C++ class pair<string, dd4hep::Handle<dd4hep::NamedObject> >+;
#pragma link C++ class map<string, dd4hep::Handle<dd4hep::NamedObject> >+;
#pragma link C++ class map<string, dd4hep::Handle<dd4hep::NamedObject> >::iterator;
#pragma link C++ class map<string, dd4hep::Handle<dd4hep::NamedObject> >::const_iterator;
#pragma link C++ class dd4hep::dd4hepUI;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<string, dd4hep::Handle<dd4hep::NamedObject> >::iterator&,const map<string, dd4hep::Handle<dd4hep::NamedObject> >::iterator& );
#pragma link C++ function operator!=( const map<string, dd4hep::Handle<dd4hep::NamedObject> >::iterator&,const map<string, dd4hep::Handle<dd4hep::NamedObject> >::iterator& );
#endif

#pragma link C++ class dd4hep::ObjectExtensions+;
template class dd4hep::Handle<TNamed>;
#pragma link C++ class dd4hep::Handle<TNamed>+;

#pragma link C++ class dd4hep::IOV+;
#pragma link C++ class dd4hep::IOVType+;
#pragma link C++ class dd4hep::OpaqueData+;
#pragma link C++ class dd4hep::OpaqueDataBlock+;

#pragma link C++ class dd4hep::Detector+;
#pragma link C++ class dd4hep::DetectorData+;
#pragma link C++ class dd4hep::DetectorData::ObjectHandleMap+;
#pragma link C++ class dd4hep::Detector::PropertyValues+;
#pragma link C++ class dd4hep::Detector::Properties+;
#pragma link C++ class DD4hepRootPersistency+;

// These below are the Namedobject instances to be generated ....
//#pragma link C++ class dd4hep::Detector::HandleMap+;
//#pragma link C++ class dd4hep::Detector::HandleMap::iterator;
//#pragma link C++ class dd4hep::Detector::HandleMap::const_iterator;
//#pragma link C++ class dd4hep::Detector::HandleMap::key_type; // == string
//#pragma link C++ class dd4hep::Detector::HandleMap::value_type+;

#pragma link C++ class dd4hep::VolumeManager+;
#pragma link C++ class dd4hep::VolumeManagerObject+;
#pragma link C++ class dd4hep::VolumeManagerContext+;
#pragma link C++ class dd4hep::Handle<dd4hep::VolumeManagerObject>+;

#pragma link C++ class dd4hep::CartesianField+;
#pragma link C++ class dd4hep::CartesianField::Object+;
#pragma link C++ class dd4hep::Handle<dd4hep::CartesianField::Object>+;
#pragma link C++ class dd4hep::OverlayedField+;
#pragma link C++ class dd4hep::OverlayedField::Object+;
#pragma link C++ class dd4hep::Handle<dd4hep::OverlayedField::Object>+;

// FieldTypes.h
#pragma link C++ class dd4hep::ConstantField+;
#pragma link C++ class dd4hep::Handle<dd4hep::ConstantField>+;
#pragma link C++ class dd4hep::SolenoidField+;
#pragma link C++ class dd4hep::Handle<dd4hep::SolenoidField>+;
#pragma link C++ class dd4hep::DipoleField+;
#pragma link C++ class dd4hep::Handle<dd4hep::DipoleField>+;

#pragma link C++ class dd4hep::IDDescriptor+;
#pragma link C++ class dd4hep::IDDescriptorObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::IDDescriptorObject>+;

// Objects.h
#pragma link C++ class dd4hep::Author+;
#pragma link C++ class vector<dd4hep::Author>+;

#pragma link C++ class dd4hep::Header+;
#pragma link C++ class dd4hep::HeaderObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::HeaderObject>+;

#pragma link C++ class dd4hep::Constant+;
#pragma link C++ class dd4hep::ConstantObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::ConstantObject>+;
#pragma link C++ class vector<dd4hep::Constant>+;

#pragma link C++ class dd4hep::Atom+;
#pragma link C++ class vector<dd4hep::Atom>+;
#pragma link C++ class dd4hep::Handle<TGeoElement>+;

#pragma link C++ class dd4hep::Material+;
#pragma link C++ class vector<dd4hep::Material>+;
#pragma link C++ class dd4hep::Handle<TGeoMedium>+;

#pragma link C++ class dd4hep::VisAttr+;
#pragma link C++ class vector<dd4hep::VisAttr>+;
#pragma link C++ class dd4hep::VisAttrObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::VisAttrObject>+;

#pragma link C++ class dd4hep::Limit+;
#pragma link C++ class set<dd4hep::Limit>+;
#pragma link C++ class vector<dd4hep::Limit>+;
#pragma link C++ class dd4hep::LimitSet+;
#pragma link C++ class vector<dd4hep::LimitSet>+;
#pragma link C++ class dd4hep::LimitSetObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::LimitSetObject>+;
#pragma link C++ class dd4hep::Region+;
#pragma link C++ class dd4hep::RegionObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::RegionObject>+;
#pragma link C++ class vector<dd4hep::Region>+;

// Readout.h
#pragma link C++ class vector<pair<size_t,string> >+;
#pragma link C++ class dd4hep::Segmentation+;
#pragma link C++ class dd4hep::SegmentationObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::SegmentationObject>+;
#pragma link C++ class dd4hep::HitCollection+;
#pragma link C++ class dd4hep::Readout+;
#pragma link C++ class dd4hep::HitCollection+;
#pragma link C++ class dd4hep::ReadoutObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::ReadoutObject>+;
#pragma link C++ class vector<dd4hep::HitCollection>+;
#pragma link C++ class vector<dd4hep::Readout>+;
#pragma link C++ class vector<dd4hep::HitCollection*>+;
#pragma link C++ class vector<const dd4hep::HitCollection*>+;
#pragma link C++ class vector<dd4hep::IDDescriptor>+;

// Alignment stuff
#pragma link C++ class dd4hep::Delta+;
#pragma link C++ class dd4hep::Alignment+;
#pragma link C++ class dd4hep::AlignmentData+;

#pragma link C++ class dd4hep::detail::AlignmentConditionObject+;
#pragma link C++ class dd4hep::align::GlobalAlignment+;
#pragma link C++ class dd4hep::AlignmentDecorator<AlignmentData>+;
#pragma link C++ class dd4hep::Handle<dd4hep::AlignmentData>+;
#pragma link C++ class dd4hep::Handle<TGeoPhysicalNode>+;



#pragma link C++ class dd4hep::Condition+;
#pragma link C++ class vector<dd4hep::Condition>+;
#pragma link C++ class dd4hep::ConditionKey+;
#pragma link C++ class vector<dd4hep::ConditionKey>+;
#pragma link C++ class dd4hep::detail::ConditionObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::detail::ConditionObject>+;

// DetElement.h
#pragma link C++ class dd4hep::World+;
#pragma link C++ class dd4hep::WorldObject+;
#pragma link C++ class dd4hep::DetElement+;
#pragma link C++ class dd4hep::DetElementObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::DetElementObject>+;
#pragma link C++ class vector<dd4hep::DetElement>+;
#pragma link C++ class pair<string,dd4hep::DetElement>+;
#pragma link C++ class map<string,dd4hep::DetElement>+;
#pragma link C++ class map<string,dd4hep::DetElement>::iterator;
#pragma link C++ class map<string,dd4hep::DetElement>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<string, dd4hep::DetElement >::iterator&,const map<string, dd4hep::DetElement >::iterator& );
#pragma link C++ function operator!=( const map<string, dd4hep::DetElement >::iterator&,const map<string, dd4hep::DetElement >::iterator& );
#endif

#pragma link C++ class dd4hep::SensitiveDetector+;
#pragma link C++ class dd4hep::SensitiveDetectorObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::SensitiveDetectorObject>+;
#pragma link C++ class vector<dd4hep::SensitiveDetector>+;

// Volume.h
#pragma link C++ class dd4hep::Volume+;
#pragma link C++ class dd4hep::VolumeExtension+;
#pragma link C++ class vector<dd4hep::Volume>+;
#pragma link C++ class dd4hep::Handle<TGeoVolume>+;

#pragma link C++ class dd4hep::PlacedVolume+;
#ifndef __ROOTCLING__
template vector<pair<string, int> >;
template vector<pair<string, int> >::iterator;
#endif
#pragma link C++ class vector<pair<string, int> >+;
#pragma link C++ class vector<pair<string, int> >::iterator;
#pragma link C++ class dd4hep::PlacedVolumeExtension::VolIDs+;
#pragma link C++ class dd4hep::PlacedVolumeExtension+;
#pragma link C++ class vector<dd4hep::PlacedVolume>+;
#pragma link C++ class dd4hep::Handle<TGeoNode>+;
#pragma link C++ class vector<TGeoNode*>+;
#pragma link C++ class vector<TGeoVolume*>+;


// Shapes.h
#pragma link C++ class dd4hep::Handle<TGeoShape>+;
#pragma link C++ class dd4hep::Solid_type<TGeoShape>+;

#pragma link C++ class dd4hep::Polycone+;
#pragma link C++ class dd4hep::Solid_type<TGeoPcon>+;
#pragma link C++ class dd4hep::Handle<TGeoPcon>+;

#pragma link C++ class dd4hep::ConeSegment+;
#pragma link C++ class dd4hep::Solid_type<TGeoConeSeg>+;
#pragma link C++ class dd4hep::Handle<TGeoConeSeg>+;

#pragma link C++ class dd4hep::Box+;
#pragma link C++ class dd4hep::Solid_type<TGeoBBox>+;
#pragma link C++ class dd4hep::Handle<TGeoBBox>+;

#pragma link C++ class dd4hep::Torus+;
#pragma link C++ class dd4hep::Solid_type<TGeoTorus>+;
#pragma link C++ class dd4hep::Handle<TGeoTorus>+;

#pragma link C++ class dd4hep::Cone+;
#pragma link C++ class dd4hep::Solid_type<TGeoCone>+;
#pragma link C++ class dd4hep::Handle<TGeoCone>+;

#pragma link C++ class dd4hep::Tube+;
#pragma link C++ class dd4hep::Solid_type<TGeoTubeSeg>+;
#pragma link C++ class dd4hep::Handle<TGeoTubeSeg>+;

#pragma link C++ class dd4hep::EllipticalTube+;
#pragma link C++ class dd4hep::Solid_type<TGeoEltu>+;
#pragma link C++ class dd4hep::Handle<TGeoEltu>+;

#pragma link C++ class dd4hep::Trap+;
#pragma link C++ class dd4hep::Solid_type<TGeoTrap>+;
#pragma link C++ class dd4hep::Handle<TGeoTrap>+;

#pragma link C++ class dd4hep::Trapezoid+;
#pragma link C++ class dd4hep::Solid_type<TGeoTrd2>+;
#pragma link C++ class dd4hep::Handle<TGeoTrd2>+;

#pragma link C++ class dd4hep::Sphere+;
#pragma link C++ class dd4hep::Solid_type<TGeoSphere>+;
#pragma link C++ class dd4hep::Handle<TGeoSphere>+;

#pragma link C++ class dd4hep::Paraboloid+;
#pragma link C++ class dd4hep::Solid_type<TGeoParaboloid>+;
#pragma link C++ class dd4hep::Handle<TGeoParaboloid>+;

#pragma link C++ class dd4hep::Hyperboloid+;
#pragma link C++ class dd4hep::Solid_type<TGeoHype>+;
#pragma link C++ class dd4hep::Handle<TGeoHype>+;

#pragma link C++ class dd4hep::PolyhedraRegular+;
#pragma link C++ class dd4hep::Solid_type<TGeoPgon>+;
#pragma link C++ class dd4hep::Handle<TGeoPgon>+;

#pragma link C++ class dd4hep::BooleanSolid+;
#pragma link C++ class dd4hep::Solid_type<TGeoCompositeShape>+;
#pragma link C++ class dd4hep::Handle<TGeoCompositeShape>+;

#pragma link C++ class dd4hep::SubtractionSolid+;
#pragma link C++ class dd4hep::UnionSolid+;
#pragma link C++ class dd4hep::IntersectionSolid+;

#pragma link C++ class pair<string, string>+;
#pragma link C++ class map<string, string>+;
#pragma link C++ class map<string, string>::iterator;
#pragma link C++ class map<string, string>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<string, string>::iterator&, const map<string, string>::iterator& );
#pragma link C++ function operator!=( const map<string, string>::iterator&, const map<string, string>::iterator& );
#endif

#pragma link C++ class pair<string, map<string, string> >+;
#pragma link C++ class map<string, map<string, string> >+;
#pragma link C++ class map<string, map<string,string>>::iterator;
#pragma link C++ class map<string, map<string,string>>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<string, map<string,string>>::iterator&, const map<string, map<string,string>>::iterator& );
#pragma link C++ function operator!=( const map<string, map<string,string>>::iterator&, const map<string, map<string,string>>::iterator& );
#endif

#pragma link C++ class dd4hep::Detector+;

#pragma link C++ function dd4hep::run_interpreter(const string& name);
#pragma link C++ function dd4hep::_toDictionary(const string&, const string&);

#include "DD4hep/ConditionsData.h"
#pragma link C++ class dd4hep::cond::ClientData+;
#pragma link C++ class dd4hep::cond::AbstractMap+;
#pragma link C++ class dd4hep::cond::AbstractMap::Params+;


#endif  // __CINT__


// -------------------------------------------------------------------------
// DDSegmentation dictionaries
// -------------------------------------------------------------------------
#ifdef __HAVE_DDSEGMENTATION__
#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/NoSegmentation.h"
#include "DDSegmentation/GridPhiEta.h"
#include "DDSegmentation/GridRPhiEta.h"
#include "DDSegmentation/CartesianGrid.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/CartesianGridXZ.h"
#include "DDSegmentation/CartesianGridYZ.h"
#include "DDSegmentation/CylindricalSegmentation.h"
#include "DDSegmentation/ProjectiveCylinder.h"
#include "DDSegmentation/SegmentationParameter.h"
#include "DDSegmentation/TiledLayerSegmentation.h"
typedef dd4hep::DDSegmentation::VolumeID VolumeID;
typedef dd4hep::DDSegmentation::CellID CellID;

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLANG__) || defined(__ROOTCLING__)
#pragma link C++ class dd4hep::DDSegmentation::SegmentationParameter+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<int>+;

#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<float>+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<double>+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<string>+;
#if 0
/// Severe problem due to template specialization!
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<int> >+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<float> >+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<double> >+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<string> >+;
#endif

#pragma link C++ class dd4hep::DDSegmentation::Segmentation+;
#pragma link C++ class dd4hep::DDSegmentation::NoSegmentation+;
#pragma link C++ class dd4hep::DDSegmentation::GridPhiEta+;
#pragma link C++ class dd4hep::DDSegmentation::GridRPhiEta+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGrid+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridXY+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridXYZ+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridXZ+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridYZ+;
#pragma link C++ class dd4hep::DDSegmentation::CylindricalSegmentation+;
#pragma link C++ class dd4hep::DDSegmentation::ProjectiveCylinder+;
#pragma link C++ class dd4hep::DDSegmentation::TiledLayerSegmentation+;

#pragma link C++ class dd4hep::DDSegmentation::BitFieldValue+;
#pragma link C++ class dd4hep::DDSegmentation::BitField64+;

#endif  // __CINT__
#endif  // __HAVE_DDSEGMENTATION__

#endif  /* DD4HEP_DDCORE_DICTIONARY_H  */
