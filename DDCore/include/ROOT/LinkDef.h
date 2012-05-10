//
//  LinkDef.h
//  
//
//  Created by Pere Mato on 22/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

template class DD4hep::Geometry::Handle<TNamed>;

#pragma link C++ class DD4hep::Geometry::LCDD;
#pragma link C++ class DD4hep::Geometry::Handle<TNamed>;
#pragma link C++ class DD4hep::Geometry::Constant;
#pragma link C++ class DD4hep::Geometry::Material;
#pragma link C++ class DD4hep::Geometry::Volume;
#pragma link C++ class DD4hep::Geometry::VisAttr;
#pragma link C++ class DD4hep::Geometry::Limit;
#pragma link C++ class DD4hep::Geometry::AlignmentEntry;
#pragma link C++ class DD4hep::Geometry::DetElement;
#pragma link C++ class DD4hep::Geometry::Box;
#pragma link C++ class DD4hep::Geometry::Tube;
#pragma link C++ class DD4hep::Geometry::Trapezoid;
#pragma link C++ class DD4hep::Geometry::Volume;
#pragma link C++ class DD4hep::Geometry::PlacedVolume;
#pragma link C++ class DD4hep::Geometry::Position;
#pragma link C++ class DD4hep::Geometry::Rotation;
#pragma link C++ namespace DD4hep::Geometry;
#pragma link C++ function DD4hep::Geometry::_toDictionary(const std::string&, const std::string&);

#endif
