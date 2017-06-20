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
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DetectorHelper.h"

// C/C++ include files
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;


namespace  {

  /** @class DetectorHelperTest
   *
   * Test DetectorHelper handle object to easily access the sensitive detector object of a detector
   * using either the subdetector name or the detector element (or one of its children).
   *
   * This helper recusively searches for all children of a subdetector the sensitive detector.
   * 
   * See: dd4hep/DetectorHelper.h
   * Test: geoPluginRun -input file:../DD4hep.trunk/examples/CLICSiD/compact/compact.xml \
   *                    -plugin CLICSiD_DetectorHelperTest			\
   *         optional:  -<detector-name (default:SiVertexEndcap)>  [Note the '-'!!!]
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct DetectorHelperTest  {
    /// Initializing constructor
    DetectorHelperTest(Detector& description, int argc, char** argv)   {
      DetectorHelper h(&description);
      const char* nam = argc>1 ? argv[1]+1 : "SiVertexEndcap";
      printSD(h,nam);
      walkSD(h,description.detector(nam));
    }
    /// Default destructor
    virtual ~DetectorHelperTest() {}

    void walkSD(DetectorHelper h, DetElement de)  const {
      printSD(h,de);
      for(DetElement::Children::const_iterator i=de.children().begin(); i!=de.children().end(); ++i)  {
        DetElement child = (*i).second;
        printSD(h,child);
        if ( child.children().size() > 0 ) walkSD(h,child);
      }
    }
    void printSD(DetectorHelper h, DetElement de)  const {
      SensitiveDetector sd = h.sensitiveDetector(de);
      printout(INFO,"DetectorHelperTest","Sensitive detector[%s]: %p  --> %s",de.path().c_str(),(void*)sd.ptr(),
               sd.ptr() ? sd.name() : "????");

    }
    void printSD(DetectorHelper h, const char* nam)  const {
      SensitiveDetector sd = h.sensitiveDetector(nam);
      printout(INFO,"DetectorHelperTest","Sensitive detector[%s]: %p  --> %s",nam,(void*)sd.ptr(),
               sd.ptr() ? sd.name() : "????");

    }
    /// Action routine to execute the test
    static long run(Detector& description,int argc,char** argv)   {
      DetectorHelperTest test(description,argc,argv);
      return 1;
    }
  };
}

namespace dd4hep {
  using ::DetectorHelperTest;
}
DECLARE_APPLY(CLICSiD_DetectorHelperTest,DetectorHelperTest::run)
