// $Id: TubeSegment_geo.cpp 633 2013-06-21 13:50:50Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/VolumeManager.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static long test(LCDD& lcdd,int argc,char** argv)    {
  cout << "Processing plugin...." << endl;
  typedef vector<PlacedVolume::VolID> VolIDs;
  VolumeManager     mgr    = lcdd.volumeManager();
  DetElement        sdet   = lcdd.detector("HcalBarrel");
  SensitiveDetector sd     = lcdd.sensitiveDetector("HcalBarrel");
  IDDescriptor      iddesc = sd.readout().idSpec();

  size_t ii, jj, kk;
  VolIDs sysIds;
  sysIds.push_back(PlacedVolume::VolID("system",sdet.id()));
  sysIds.push_back(PlacedVolume::VolID("barrel",0));
  DetElement::Children::const_iterator i, j, k;
  const DetElement::Children& staves = sdet.children();  
  for (i=staves.begin(), ii=1; i!=staves.end(); ++i, ++ii)  {
    VolIDs staveIds(sysIds);
    DetElement stave = (*i).second;
    const DetElement::Children& layers = stave.children();
    PlacedVolume stave_pv = stave.placement();
    staveIds.insert(staveIds.end(),stave_pv.volIDs().begin(),stave_pv.volIDs().end());
    for (j=layers.begin(), jj=1; j!=layers.end(); ++j, ++jj)  {
      VolIDs layerIds(staveIds);
      DetElement layer = (*j).second;
      PlacedVolume layer_pv = layer.placement();
      const DetElement::Children& slices = layer.children();
      layerIds.insert(layerIds.end(),layer_pv.volIDs().begin(),layer_pv.volIDs().end());
      for (k=slices.begin(), kk=1; k!=slices.end(); ++k, ++kk)  {
	VolIDs sliceIds(layerIds);
	DetElement slice = (*k).second;
	PlacedVolume pv = slice.placement();
	sliceIds.insert(sliceIds.end(),pv.volIDs().begin(),pv.volIDs().end());
	if ( pv.volume().isSensitive() )  {
	  stringstream err, log;
	  VolumeManager::VolumeID vid = iddesc.encode(sliceIds);
	  DetElement    det       = mgr.lookupDetector(vid);
	  DetElement    det_elem  = mgr.lookupDetElement(vid);
	  PlacedVolume  det_place = mgr.lookupPlacement(vid);
	  if ( pv.ptr() != det_place.ptr() )   {
	    err << "Wrong placement ";
	  }
	  else if ( det_elem.ptr() != slice.ptr() )   {
	    err << "Wrong associated detector element vid=" << (void*)vid 
		<< " got " << det_elem.path() << " (" << (void*)det_elem.ptr() << ") "
		<< " instead of " << slice.path() << " (" << (void*)slice.ptr() << ")";
	  }
	  else if ( det.ptr() != sdet.ptr() )   {
	    err << "Wrong associated detector ";
	  }

	  const IDDescriptor::FieldMap& m = iddesc.fields();
	  log << "IDS(" << pv.name() << "): ";
	  for(IDDescriptor::FieldMap::const_iterator l=m.begin(); l != m.end(); ++l)   {
	    log << (*l).first << "=" << (*l).second->value(vid) << "  ";
	  }
	  if ( !err.str().empty() )   {
	    printout(ERROR,"CLICSid_PolyhedraBarrel_VolMgrTest",(err.str()+" "+log.str()).c_str());
	    throw runtime_error(err.str());
	  }
	  printout(INFO,"CLICSid_PolyhedraBarrel_VolMgrTest",("Found Volume: "+log.str()).c_str());	  
	}
      }
    }
  }
  return 1;
}
DECLARE_APPLY(CLICSid_PolyhedraBarrel_VolMgrTest,test);
