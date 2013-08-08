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


namespace DD4hep {
  struct VolIDTest  {
    typedef vector<PlacedVolume::VolID> VolIDs;
    std::string   m_name;
    IDDescriptor  m_iddesc;
    VolumeManager m_mgr;
    DetElement    m_det;
    VolIDTest(LCDD& lcdd, const std::string& name, size_t depth);
    virtual ~VolIDTest() {}
    void walk(DetElement de, VolIDs ids, size_t depth, size_t mx_depth)  const;
  };
}

VolIDTest::VolIDTest(LCDD& lcdd, const std::string& name, size_t depth) : m_name(name)  {
  DetElement        sdet   = lcdd.detector(name);
  SensitiveDetector sd     = lcdd.sensitiveDetector(name);
  stringstream err, log;

  if ( !sdet.isValid() )   {
    err << "The subdetector " << name << " is not known to the geometry.";
    printout(INFO,"VolIDTest",err.str().c_str());
    throw runtime_error(err.str());
  }
  if ( !sdet.isValid() )   {
    err << "The sensitive detector of subdetector " << name << " is not known to the geometry.";
    printout(INFO,"VolIDTest",err.str().c_str());
    throw runtime_error(err.str());
  }
  m_det    = sdet;
  m_mgr    = lcdd.volumeManager();
  m_iddesc = sd.readout().idSpec();
  VolIDs sysIds;
  walk(sdet,sysIds,0,depth);
}

void VolIDTest::walk(DetElement e, VolIDs ids, size_t depth, size_t mx_depth)  const   {
  if ( depth <= mx_depth )  {
    DetElement::Children::const_iterator i;
    const DetElement::Children& children = e.children();  
    PlacedVolume pv = e.placement();
    VolIDs child_ids(ids);
    child_ids.insert(child_ids.end(),pv.volIDs().begin(),pv.volIDs().end());
    for (i=children.begin(); i!=children.end(); ++i)  {
      walk((*i).second,child_ids,depth+1,mx_depth);
    }
    if ( pv.volume().isSensitive() )  {
      stringstream err, log;
      VolumeID vid = m_iddesc.encode(child_ids);
      try {
	DetElement det       = m_mgr.lookupDetector(vid);
        DetElement det_elem  = m_mgr.lookupDetElement(vid);
        PlacedVolume det_place = m_mgr.lookupPlacement(vid);
	if ( pv.ptr() != det_place.ptr() )   {
	  err << "Wrong placement "
	      << " got "        << det_place.name() << " (" << (void*)det_place.ptr() << ")"
	      << " instead of " << pv.name()        << " (" << (void*)pv.ptr()        << ") "
	      << " vid:" << (void*)vid;
	}
	else if ( det_elem.ptr() != e.ptr() )   {
	  err << "Wrong associated detector element vid="  << (void*)vid 
	      << " got "        << det_elem.path() << " (" << (void*)det_elem.ptr() << ") "
	      << " instead of " << e.path()        << " (" << (void*)e.ptr()        << ")"
	      << " vid:" << (void*)vid;
	}
	else if ( det.ptr() != m_det.ptr() )   {
	  err << "Wrong associated detector "
	      << " vid:" << (void*)vid;
	}
      }
      catch(const exception& ex) {
	err << "Lookup " << pv.name() << " id:" << (void*)vid << " path:" << e.path() << " error:" << ex.what();
      }
      const IDDescriptor::FieldMap& m = m_iddesc.fields();
      log << "IDS(" << pv.name() << "): ";
      for(size_t fi=0; fi<mx_depth; ++fi)  {
	IDDescriptor::Field fld = m_iddesc.field(fi);
	log << fld->name() << "=" << fld->value(vid) << "  ";
      }
      log << " vid:" << (void*)vid;
      if ( !err.str().empty() )   {
	printout(ERROR,m_name+"_VolMgrTest",err.str()+" "+log.str());
	throw runtime_error(err.str());
      }
      printout(INFO,m_name+"_VolMgrTest","Found Volume: "+log.str());
    }
  }
}


static long test(LCDD& lcdd,int argc,char** argv)    {
  cout << "Processing plugin...CLICSid_VolMgrTest..." << endl;
  //VolIDTest test(lcdd,"HcalBarrel",5);
  for(int i=1; i<argc;++i)  {
    string name = argv[i]+1;
    DetElement sdet = lcdd.detector(name);
    VolIDTest test(lcdd,name,5);
  }
#if 0
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
#endif
  return 1;
}
DECLARE_APPLY(CLICSid_PolyhedraBarrel_VolMgrTest,test);
