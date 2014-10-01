#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"

#include "DDRec/DetectorData.h"

#include "DDRec/DDGear.h"
#include "gearimpl/TPCParametersImpl.h"
#include "gearimpl/FixedPadSizeDiskLayout.h"


#include <iostream>

namespace DD4hep{
  namespace DDRec{
    
    using namespace Geometry ;
    //    using namespace gear ;

    /** Plugin that creates Gear objects for DetElements and attaches them 
     *  as extensions. Called from DDGear::createGearMgr().
     *  NB: this code is for backward compatibility to run 
     *  the old reconstruction that expects Gear information
     *  and should eventually be phased out.
     * 
     *  @author  F.Gaede, CERN/DESY
     *  @date Oct 2014
     *  @version $Id: $
     */
    
    static long createGearForILD(LCDD& lcdd, int argc, char** argv) {
      
      std::cout << " **** running plugin createGearForILD ! " <<  std::endl ;
      

      //***** TPC ********
      DetElement tpcDE = lcdd.detector("TPC") ;

      FixedPadSizeTPCData* tpc = tpcDE.extension<FixedPadSizeTPCData>() ;

      gear::TPCParametersImpl* gearTPC = new gear::TPCParametersImpl( tpc->driftLength , gear::PadRowLayout2D::POLAR ) ;
  
      gearTPC->setPadLayout( new gear::FixedPadSizeDiskLayout( tpc->rMinReadout, tpc->rMaxReadout, tpc->padHeight, tpc->padWidth, tpc->maxRow, tpc->padGap  ) ) ;
      
      gearTPC->setDoubleVal("tpcInnerRadius", tpc->rMin )  ; // inner r of support tube
      gearTPC->setDoubleVal("tpcOuterRadius", tpc->rMax )  ; // outer radius of TPC
      gearTPC->setDoubleVal("tpcInnerWallThickness", tpc->innerWallThickness )  ;   // thickness of inner shell
      gearTPC->setDoubleVal("tpcOuterWallThickness", tpc->outerWallThickness )  ;   // thickness of outer shell
      
      tpcDE.addExtension< GearHandle >( new GearHandle( gearTPC, "TPCParameters" ) ) ;
      
      //********************






      // --- LCDD::apply() expects return code 1 if all went well ! ----
      return 1;
    }
  }
}
DECLARE_APPLY( GearForILD, DD4hep::DDRec::createGearForILD )


