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
//  LinkDef.h
//
//
//  Created by Pere Mato on 22/1/12.
//
//==========================================================================
//

#ifndef DD4HEP_GEOMETRY_DICTIONARY_H
#define DD4HEP_GEOMETRY_DICTIONARY_H

#include "XML/Evaluator.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DD4hep/objects/VolumeManagerInterna.h"

#include "DD4hep/World.h"
#include "DD4hep/Callback.h"
#include "DD4hep/LCDDData.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/FieldTypes.h"

#include <vector>
#include <map>
#include <string>

typedef DD4hep::Geometry::PlacedVolumeExtension::VolID VolID;

#include "TRint.h"
namespace DD4hep {
  void run_interpreter(const std::string& name)  {
    pair<int, char**> a(0,0);
    TRint app(name.c_str(), &a.first, a.second);
    app.Run();
  }
  XmlTools::Evaluator& evaluator();
  XmlTools::Evaluator& g4Evaluator();
}

// -------------------------------------------------------------------------
// Regular DD4hep dictionaries
// -------------------------------------------------------------------------
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

#pragma link C++ namespace XmlTools;
#pragma link C++ class XmlTools::Evaluator;
#pragma link C++ function DD4hep::evaluator;
#pragma link C++ function DD4hep::g4Evaluator;

#pragma link C++ namespace DD4hep;
#pragma link C++ namespace DD4hep::Geometry;
#pragma link C++ namespace DD4hep::Conditions;
#pragma link C++ namespace DD4hep::DDSegmentation;

#ifndef __ROOTCLING__
template pair<unsigned int, string>;
template class DD4hep::Handle<DD4hep::NamedObject>;
template class pair< string, DD4hep::Handle<DD4hep::NamedObject> >;
template class map< string, DD4hep::Handle<DD4hep::NamedObject> >;
template class pair<DD4hep::Callback,unsigned long>;
#endif

#pragma link C++ class pair<unsigned int,string>+;
//#pragma link C++ class DD4hep::Callback+;
#pragma link C++ class pair<DD4hep::Callback,unsigned long>+;
#pragma link C++ class DD4hep::NamedObject+;
#pragma link C++ class DD4hep::Geometry::Ref_t+;
#pragma link C++ class DD4hep::Handle<DD4hep::NamedObject>+;
#pragma link C++ class pair<string, DD4hep::Handle<DD4hep::NamedObject> >+;
#pragma link C++ class map<string, DD4hep::Handle<DD4hep::NamedObject> >+;
#pragma link C++ class map<string, DD4hep::Handle<DD4hep::NamedObject> >::iterator;
#pragma link C++ class map<string, DD4hep::Handle<DD4hep::NamedObject> >::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<string, DD4hep::Handle<DD4hep::NamedObject> >::iterator&,const map<string, DD4hep::Handle<DD4hep::NamedObject> >::iterator& ); 
#pragma link C++ function operator!=( const map<string, DD4hep::Handle<DD4hep::NamedObject> >::iterator&,const map<string, DD4hep::Handle<DD4hep::NamedObject> >::iterator& ); 
#endif

#pragma link C++ class DD4hep::ObjectExtensions+;
template class DD4hep::Handle<TNamed>;
#pragma link C++ class DD4hep::Handle<TNamed>+;

#pragma link C++ class DD4hep::IOV+;
#pragma link C++ class DD4hep::IOVType+;
#pragma link C++ class DD4hep::OpaqueData+;
#pragma link C++ class DD4hep::OpaqueDataBlock+;

#pragma link C++ class DD4hep::Geometry::LCDD+;
#pragma link C++ class DD4hep::Geometry::LCDDData+;
#pragma link C++ class DD4hep::Geometry::LCDDData::ObjectHandleMap+;
#pragma link C++ class DD4hep::Geometry::LCDD::PropertyValues+;
#pragma link C++ class DD4hep::Geometry::LCDD::Properties+;
#pragma link C++ class DD4hepRootPersistency+;

// These below are the Namedobject instances to be generated ....
//#pragma link C++ class DD4hep::Geometry::LCDD::HandleMap+;
//#pragma link C++ class DD4hep::Geometry::LCDD::HandleMap::iterator;
//#pragma link C++ class DD4hep::Geometry::LCDD::HandleMap::const_iterator;
//#pragma link C++ class DD4hep::Geometry::LCDD::HandleMap::key_type; // == string
//#pragma link C++ class DD4hep::Geometry::LCDD::HandleMap::value_type+;

#pragma link C++ class DD4hep::Geometry::VolumeManager+;
#pragma link C++ class DD4hep::Geometry::VolumeManagerObject+;
#pragma link C++ class DD4hep::Geometry::VolumeManagerContext+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::VolumeManagerObject>+;

#pragma link C++ class DD4hep::Geometry::CartesianField+;
#pragma link C++ class DD4hep::Geometry::CartesianField::Object+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::CartesianField::Object>+;
#pragma link C++ class DD4hep::Geometry::OverlayedField+;
#pragma link C++ class DD4hep::Geometry::OverlayedField::Object+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::OverlayedField::Object>+;

// FieldTypes.h
#pragma link C++ class DD4hep::Geometry::ConstantField+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::ConstantField>+;
#pragma link C++ class DD4hep::Geometry::SolenoidField+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::SolenoidField>+;
#pragma link C++ class DD4hep::Geometry::DipoleField+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::DipoleField>+;

#pragma link C++ class DD4hep::Geometry::IDDescriptor+;
#pragma link C++ class DD4hep::Geometry::IDDescriptorObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::IDDescriptorObject>+;

// Objects.h
#pragma link C++ class DD4hep::Geometry::Author+;
#pragma link C++ class vector<DD4hep::Geometry::Author>+;

#pragma link C++ class DD4hep::Geometry::Header+;
#pragma link C++ class DD4hep::Geometry::HeaderObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::HeaderObject>+;

#pragma link C++ class DD4hep::Geometry::Constant+;
#pragma link C++ class DD4hep::Geometry::ConstantObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::ConstantObject>+;
#pragma link C++ class vector<DD4hep::Geometry::Constant>+;

#pragma link C++ class DD4hep::Geometry::Atom+;
#pragma link C++ class vector<DD4hep::Geometry::Atom>+;
#pragma link C++ class DD4hep::Handle<TGeoElement>+;

#pragma link C++ class DD4hep::Geometry::Material+;
#pragma link C++ class vector<DD4hep::Geometry::Material>+;
#pragma link C++ class DD4hep::Handle<TGeoMedium>+;

#pragma link C++ class DD4hep::Geometry::VisAttr+;
#pragma link C++ class vector<DD4hep::Geometry::VisAttr>+;
#pragma link C++ class DD4hep::Geometry::VisAttrObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::VisAttrObject>+;

#pragma link C++ class DD4hep::Geometry::AlignmentEntry+;
#pragma link C++ class DD4hep::Geometry::Limit+;
#pragma link C++ class set<DD4hep::Geometry::Limit>+;
#pragma link C++ class vector<DD4hep::Geometry::Limit>+;
#pragma link C++ class DD4hep::Geometry::LimitSet+;
#pragma link C++ class vector<DD4hep::Geometry::LimitSet>+;
#pragma link C++ class DD4hep::Geometry::LimitSetObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::LimitSetObject>+;
#pragma link C++ class DD4hep::Geometry::Region+;
#pragma link C++ class DD4hep::Geometry::RegionObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::RegionObject>+;
#pragma link C++ class vector<DD4hep::Geometry::Region>+;

// Readout.h
#pragma link C++ class vector<pair<size_t,string> >+;
#pragma link C++ class DD4hep::Geometry::Segmentation+;
#pragma link C++ class DD4hep::Geometry::SegmentationObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::SegmentationObject>+;
#pragma link C++ class DD4hep::Geometry::HitCollection+;
#pragma link C++ class DD4hep::Geometry::Readout+;
#pragma link C++ class DD4hep::Geometry::HitCollection+;
#pragma link C++ class DD4hep::Geometry::ReadoutObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::ReadoutObject>+;
#pragma link C++ class vector<DD4hep::Geometry::HitCollection>+;
#pragma link C++ class vector<DD4hep::Geometry::Readout>+;
#pragma link C++ class vector<DD4hep::Geometry::HitCollection*>+;
#pragma link C++ class vector<const DD4hep::Geometry::HitCollection*>+;
#pragma link C++ class vector<DD4hep::Geometry::IDDescriptor>+;

// Alignment stuff
#pragma link C++ class DD4hep::Alignments::Delta+;
#pragma link C++ class DD4hep::Alignments::Alignment+;
#pragma link C++ class DD4hep::Alignments::Container+;
#pragma link C++ class DD4hep::Alignments::AlignmentData+;

#pragma link C++ class DD4hep::Alignments::DetAlign+;
#pragma link C++ class DD4hep::Alignments::AlignmentsLoader+;
#pragma link C++ class DD4hep::Alignments::Interna::AlignmentConditionObject+;
#pragma link C++ class DD4hep::Alignments::Interna::AlignmentContainer+;
#pragma link C++ class DD4hep::Alignments::GlobalAlignment+;
#pragma link C++ class DD4hep::Alignments::AlignmentDecorator<AlignmentData>+;
#pragma link C++ class DD4hep::Handle<DD4hep::Alignments::AlignmentData>+;
#pragma link C++ class DD4hep::Handle<TGeoPhysicalNode>+;



#pragma link C++ class DD4hep::Conditions::Condition+;
#pragma link C++ class vector<DD4hep::Conditions::Condition>+;
#pragma link C++ class DD4hep::Conditions::Interna::ConditionObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Conditions::Interna::ConditionObject>+;

#pragma link C++ class DD4hep::Conditions::Container+;
#pragma link C++ class DD4hep::Conditions::Interna::ConditionContainer+;
#pragma link C++ class DD4hep::Handle<DD4hep::Conditions::Interna::ConditionContainer>+;
#pragma link C++ class DD4hep::Conditions::ConditionsPool+;

#pragma link C++ class DD4hep::Alignments::DetConditions+;

// DetElement.h
#pragma link C++ class DD4hep::Geometry::World+;
#pragma link C++ class DD4hep::Geometry::WorldObject+;
#pragma link C++ class DD4hep::Geometry::DetElement+;
#pragma link C++ class DD4hep::Geometry::DetElementObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::DetElementObject>+;
#pragma link C++ class vector<DD4hep::Geometry::DetElement>+;
#pragma link C++ class pair<string,DD4hep::Geometry::DetElement>+;
#pragma link C++ class map<string,DD4hep::Geometry::DetElement>+;
#pragma link C++ class map<string,DD4hep::Geometry::DetElement>::iterator;
#pragma link C++ class map<string,DD4hep::Geometry::DetElement>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const map<string, DD4hep::Geometry::DetElement >::iterator&,const map<string, DD4hep::Geometry::DetElement >::iterator& ); 
#pragma link C++ function operator!=( const map<string, DD4hep::Geometry::DetElement >::iterator&,const map<string, DD4hep::Geometry::DetElement >::iterator& ); 
#endif

#pragma link C++ class DD4hep::Geometry::SensitiveDetector+;
#pragma link C++ class DD4hep::Geometry::SensitiveDetectorObject+;
#pragma link C++ class DD4hep::Handle<DD4hep::Geometry::SensitiveDetectorObject>+;
#pragma link C++ class vector<DD4hep::Geometry::SensitiveDetector>+;

// Volume.h
#pragma link C++ class DD4hep::Geometry::Volume+;
#pragma link C++ class DD4hep::Geometry::VolumeExtension+;
#pragma link C++ class vector<DD4hep::Geometry::Volume>+;
#pragma link C++ class DD4hep::Handle<TGeoVolume>+;

#pragma link C++ class DD4hep::Geometry::PlacedVolume+;
#ifndef __ROOTCLING__
template vector<pair<string, int> >;
template vector<pair<string, int> >::iterator;
#endif
#pragma link C++ class vector<pair<string, int> >+;
#pragma link C++ class vector<pair<string, int> >::iterator;
#pragma link C++ class DD4hep::Geometry::PlacedVolumeExtension::VolIDs+;
#pragma link C++ class DD4hep::Geometry::PlacedVolumeExtension+;
#pragma link C++ class vector<DD4hep::Geometry::PlacedVolume>+;
#pragma link C++ class DD4hep::Handle<TGeoNode>+;
#pragma link C++ class vector<TGeoNode*>+;
#pragma link C++ class vector<TGeoVolume*>+;


// Shapes.h
#pragma link C++ class DD4hep::Handle<TGeoShape>+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoShape>+;

#pragma link C++ class DD4hep::Geometry::Polycone+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoPcon>+;
#pragma link C++ class DD4hep::Handle<TGeoPcon>+;

#pragma link C++ class DD4hep::Geometry::ConeSegment+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoConeSeg>+;
#pragma link C++ class DD4hep::Handle<TGeoConeSeg>+;

#pragma link C++ class DD4hep::Geometry::Box+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoBBox>+;
#pragma link C++ class DD4hep::Handle<TGeoBBox>+;

#pragma link C++ class DD4hep::Geometry::Torus+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoTorus>+;
#pragma link C++ class DD4hep::Handle<TGeoTorus>+;

#pragma link C++ class DD4hep::Geometry::Cone+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoCone>+;
#pragma link C++ class DD4hep::Handle<TGeoCone>+;

#pragma link C++ class DD4hep::Geometry::Tube+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoTubeSeg>+;
#pragma link C++ class DD4hep::Handle<TGeoTubeSeg>+;

#pragma link C++ class DD4hep::Geometry::EllipticalTube+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoEltu>+;
#pragma link C++ class DD4hep::Handle<TGeoEltu>+;

#pragma link C++ class DD4hep::Geometry::Trap+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoTrap>+;
#pragma link C++ class DD4hep::Handle<TGeoTrap>+;

#pragma link C++ class DD4hep::Geometry::Trapezoid+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoTrd2>+;
#pragma link C++ class DD4hep::Handle<TGeoTrd2>+;

#pragma link C++ class DD4hep::Geometry::Sphere+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoSphere>+;
#pragma link C++ class DD4hep::Handle<TGeoSphere>+;

#pragma link C++ class DD4hep::Geometry::Paraboloid+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoParaboloid>+;
#pragma link C++ class DD4hep::Handle<TGeoParaboloid>+;

#pragma link C++ class DD4hep::Geometry::Hyperboloid+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoHype>+;
#pragma link C++ class DD4hep::Handle<TGeoHype>+;

#pragma link C++ class DD4hep::Geometry::PolyhedraRegular+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoPgon>+;
#pragma link C++ class DD4hep::Handle<TGeoPgon>+;

#pragma link C++ class DD4hep::Geometry::BooleanSolid+;
#pragma link C++ class DD4hep::Geometry::Solid_type<TGeoCompositeShape>+;
#pragma link C++ class DD4hep::Handle<TGeoCompositeShape>+;

#pragma link C++ class DD4hep::Geometry::SubtractionSolid+;
#pragma link C++ class DD4hep::Geometry::UnionSolid+;
#pragma link C++ class DD4hep::Geometry::IntersectionSolid+;

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

#pragma link C++ class DD4hep::Geometry::LCDD+;

#pragma link C++ function DD4hep::run_interpreter(const string& name);
#pragma link C++ function DD4hep::Geometry::_toDictionary(const string&, const string&);

#endif  // __CINT__


// -------------------------------------------------------------------------
// DDSegmentation dictionaries
// -------------------------------------------------------------------------
#ifdef __HAVE_DDSEGMENTATION__
#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/CartesianGrid.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/CartesianGridXZ.h"
#include "DDSegmentation/CartesianGridYZ.h"
#include "DDSegmentation/CylindricalSegmentation.h"
#include "DDSegmentation/ProjectiveCylinder.h"
#include "DDSegmentation/SegmentationParameter.h"
#include "DDSegmentation/TiledLayerSegmentation.h"
typedef DD4hep::DDSegmentation::VolumeID VolumeID;
typedef DD4hep::DDSegmentation::CellID CellID;

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLANG__) || defined(__ROOTCLING__)
#pragma link C++ class DD4hep::DDSegmentation::SegmentationParameter+;
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<int>+;

#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<float>+;
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<double>+;
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<string>+;
#if 0
/// Severe problem due to template specialization!
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<vector<int> >+;
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<vector<float> >+;
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<vector<double> >+;
#pragma link C++ class DD4hep::DDSegmentation::TypedSegmentationParameter<vector<string> >+;
#endif

#pragma link C++ class DD4hep::DDSegmentation::Segmentation+;
#pragma link C++ class DD4hep::DDSegmentation::CartesianGrid+;
#pragma link C++ class DD4hep::DDSegmentation::CartesianGridXY+;
#pragma link C++ class DD4hep::DDSegmentation::CartesianGridXYZ+;
#pragma link C++ class DD4hep::DDSegmentation::CartesianGridXZ+;
#pragma link C++ class DD4hep::DDSegmentation::CartesianGridYZ+;
#pragma link C++ class DD4hep::DDSegmentation::CylindricalSegmentation+;
#pragma link C++ class DD4hep::DDSegmentation::ProjectiveCylinder+;
#pragma link C++ class DD4hep::DDSegmentation::TiledLayerSegmentation+;

#pragma link C++ class DD4hep::DDSegmentation::BitFieldValue+;
#pragma link C++ class DD4hep::DDSegmentation::BitField64+;

#endif  // __CINT__
#endif  // __HAVE_DDSEGMENTATION__

#endif // DD4HEP_GEOMETRY_DICTIONARY_H
