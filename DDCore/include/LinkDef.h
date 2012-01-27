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

#pragma link C++ class DetDesc::Geometry::LCDD;
#pragma link C++ class DetDesc::Geometry::Document;
#pragma link C++ class DetDesc::Geometry::Constant;
#pragma link C++ class DetDesc::Geometry::Material;
#pragma link C++ class DetDesc::Geometry::Volume;
#pragma link C++ class DetDesc::Geometry::VisAttr;
#pragma link C++ class DetDesc::Geometry::Subdetector;
#pragma link C++ class DetDesc::Geometry::Box;
#pragma link C++ class DetDesc::Geometry::Tube;
#pragma link C++ class DetDesc::Geometry::Volume;
#pragma link C++ class DetDesc::Geometry::PhysVol;
#pragma link C++ class DetDesc::Geometry::Position;
#pragma link C++ class DetDesc::Geometry::Rotation;
#pragma link C++ class DetDesc::Geometry::Handle_t;
#pragma link C++ namespace DetDesc::Geometry;
#pragma link C++ function DetDesc::Geometry::_toDictionary(const std::string&, const std::string&);

#endif
