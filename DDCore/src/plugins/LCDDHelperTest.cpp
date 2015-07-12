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
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/LCDDHelper.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


namespace  {

  /** @class LCDDHelperTest
   *
   * Test LCDDHelper handle object to easily access the sensitive detector object of a detector
   * using either the subdetector name or the detector element (or one of its children).
   *
   * This helper recusively searches for all children of a subdetector the sensitive detector.
   * 
   * See: DD4hep/LCDDHelper.h
   * Test: geoPluginRun -input file:../DD4hep.trunk/examples/CLICSiD/compact/compact.xml \
   *                    -plugin CLICSiD_LCDDHelperTest			\
   *         optional:  -<detector-name (default:SiVertexEndcap)>  [Note the '-'!!!]
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct LCDDHelperTest  {
    /// Initializing constructor
    LCDDHelperTest(LCDD& lcdd, int argc, char** argv)   {
      LCDDHelper h(&lcdd);
      const char* nam = argc>1 ? argv[1]+1 : "SiVertexEndcap";
      printSD(h,nam);
      walkSD(h,lcdd.detector(nam));
    }
    /// Default destructor
    virtual ~LCDDHelperTest() {}

    void walkSD(LCDDHelper h, DetElement de)  const {
      printSD(h,de);
      for(DetElement::Children::const_iterator i=de.children().begin(); i!=de.children().end(); ++i)  {
        DetElement child = (*i).second;
        printSD(h,child);
        if ( child.children().size() > 0 ) walkSD(h,child);
      }
    }
    void printSD(LCDDHelper h, DetElement de)  const {
      SensitiveDetector sd = h.sensitiveDetector(de);
      printout(INFO,"LCDDHelperTest","Sensitive detector[%s]: %p  --> %s",de.path().c_str(),(void*)sd.ptr(),
               sd.ptr() ? sd.name() : "????");

    }
    void printSD(LCDDHelper h, const char* nam)  const {
      SensitiveDetector sd = h.sensitiveDetector(nam);
      printout(INFO,"LCDDHelperTest","Sensitive detector[%s]: %p  --> %s",nam,(void*)sd.ptr(),
               sd.ptr() ? sd.name() : "????");

    }
    /// Action routine to execute the test
    static long run(LCDD& lcdd,int argc,char** argv)   {
      LCDDHelperTest test(lcdd,argc,argv);
      return 1;
    }
  };
}

namespace DD4hep {
  using ::LCDDHelperTest;
}
DECLARE_APPLY(CLICSiD_LCDDHelperTest,LCDDHelperTest::run)
