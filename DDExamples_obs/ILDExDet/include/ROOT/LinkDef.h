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

//---Specific detector----------------------------
//#pragma link C++ class DD4hep::ILDExTPC;
#pragma link C++ class DD4hep::TPCData;
#pragma link C++ class DD4hep::Geometry::Value<TNamed,DD4hep::TPCData>;
#pragma link C++ function DD4hep::Geometry::DetElement::DetElement(const std::string&, const std::string&, int, const DD4hep::TPCData&);


#endif
