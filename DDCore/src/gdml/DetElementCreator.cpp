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
//
// Specialized generic detector constructor
// 
//==========================================================================
#ifndef DD4HEP_DETELEMENTCREATOR_H
#define DD4HEP_DETELEMENTCREATOR_H

// Framework include files
#include "DD4hep/VolumeProcessor.h"
#include "DD4hep/Printout.h"

namespace dd4hep {
  
  /// DD4hep DetElement creator for the CMS geometry.
  /*  Heuristically assign DetElement structures to the sensitive volume pathes.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class DetElementCreator : public PlacedVolumeProcessor  {
    struct Data {
      PlacedVolume pv {0};
      DetElement   element {};
      bool         sensitive = false;
      bool         has_sensitive = false;
      int          level = 0;
      int          vol_count = 0;
      int          daughter_count = 0;
      int          sensitive_count = 0;

      Data() = default;
      Data(PlacedVolume v) : pv(v) {}
      Data(const Data& d) = default;
      Data& operator=(const Data& d) = default;
    };
    struct Count {
      int elements = 0;
      int volumes = 0;
      int sensitives = 0;
      Count() = default;
      Count(const Count&) = default;
      Count& operator=(const Count&) = default;
    };
    typedef std::vector<Data> VolumeStack;
    typedef std::map<std::string,DetElement> Detectors;
    typedef std::map<DetElement,Count>       Counters;
    typedef std::map<std::pair<DetElement,int>, std::pair<int,int> > LeafCount;
    typedef std::map<PlacedVolume, std::pair<int,int> > AllPlacements;
    Detector&         description;
    Material          sensitive_material;
    Counters          counters;
    LeafCount         leafCount;
    VolumeStack       stack;
    Detectors         subdetectors;
    DetElement        current_detector;
    std::string       detector;
    std::string       sensitive_material_name;
    std::string       sensitive_type;
    std::string       detector_volume_match;
    std::string       detector_volume_veto;
    size_t            detector_volume_level = 0;
    int               max_volume_level = 9999;
    PrintLevel        printLevel = INFO;
    SensitiveDetector current_sensitive;
    AllPlacements     all_placements;
    
    /// Add new subdetector to the detector description
    DetElement addSubdetector(const std::string& nam, PlacedVolume pv, bool volid);
    /// Create a new detector element
    DetElement createElement(const char* debug_tag, PlacedVolume pv, int id);
    /// Create the top level detectors
    void createTopLevelDetectors(PlacedVolume pv);
    /// Generate the name of the DetElement object from the placed volume
    std::string detElementName(PlacedVolume pv)  const;
  public:
    /// Initializing constructor
    DetElementCreator(Detector& desc,
                      const std::string& detector,
                      const std::string& sd_type,
                      const std::string& sd_match, 
                      const std::string& sd_veto,
                      const std::string& sd_mat,
                      int sd_lvl,
                      PrintLevel p);
    /// Default destructor
    virtual ~DetElementCreator()  throw();
    /// Callback to output PlacedVolume information of an single Placement
    virtual int operator()(PlacedVolume pv, int level);
    /// Callback to output PlacedVolume information of an entire Placement
    virtual int process(PlacedVolume pv, int level, bool recursive);
  };
}
#endif   /* DD4HEP_DETELEMENTCREATOR_H  */

// Framework include files
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace dd4hep;

/// Initializing constructor
DetElementCreator::DetElementCreator(Detector& desc,
                                     const std::string& det,
                                     const string& sd_match,
                                     const string& sd_veto,
                                     const string& sd_type,
                                     const string& sd_mat, int sd_lvl,
                                     PrintLevel p)
  : description(desc), detector(det), sensitive_material_name(sd_mat),
    sensitive_type(sd_type), detector_volume_match(sd_match),
    detector_volume_veto(sd_veto), max_volume_level(sd_lvl), printLevel(p)
{
  DetectorHelper helper(description);
  sensitive_material = desc.material(sensitive_material_name);
  if ( !sensitive_material.isValid() )   {
    except("DetElementCreator",
           "++ Failed to extract MATERIAL from the element table.");
  }
  stack.reserve(32);
  detector_volume_level = 0;
}

/// Default destructor
DetElementCreator::~DetElementCreator()  throw()
{
  Count total;
  stringstream str, id_str;
  const char* pref = detector_volume_match.c_str();
  printout(INFO,pref,"DetElementCreator: +++++++++++++++ Summary of sensitve elements  ++++++++++++++++++++++++");
  for ( const auto& c : counters )  {
    printout(INFO,pref,"DetElementCreator: ++ Summary: SD: %-24s %7d DetElements %7d sensitives out of %7d volumes",
             (c.first.name()+string(":")).c_str(), c.second.elements, c.second.sensitives, c.second.volumes);
    total.elements   += c.second.elements;
    total.sensitives += c.second.sensitives;
    total.volumes    += c.second.volumes;
  }
  printout(INFO,pref,"DetElementCreator: ++ Summary:     %-24s %7d DetElements %7d sensitives out of %7d volumes",
           "Grand Total:",total.elements,total.sensitives,total.volumes);
  printout(INFO,pref,"DetElementCreator: +++++++++++++++ Summary of geometry depth analysis  ++++++++++++++++++");
  int total_cnt = 0, total_depth = 0;
  map<DetElement, vector<pair<int,int> > > fields;
  for ( const auto& l : leafCount )  {
    DetElement de = l.first.first;
    printout(INFO,pref,"DetElementCreator: ++ Summary: SD: %-24s system:%04X Lvl:%3d Sensitives: %6d [Max: %6d].",
             (de.name()+string(":")).c_str(), de.id(),
             l.first.second, l.second.second, l.second.first);
    fields[de].emplace_back(l.first.second,l.second.first);
    total_depth += l.second.second;
    ++total_cnt;
  }
  printout(INFO,pref,"DetElementCreator: ++ Summary:     %-24s  %d.","Total DetElements:",total_cnt);
  printout(INFO,pref,"DetElementCreator: +++++++++++++++ Readout structure generation  ++++++++++++++++++++++++");
  str << endl;
  for( const auto& f : fields )   {
    string ro_name = f.first.name() + string("Hits");
    int num_bits = 8;
    id_str.str("");
    id_str << "system:" << num_bits;
    for( const auto& q : f.second )   {
      int bits = 0;
      if      ( q.second < 1<<0  ) bits = 1;
      else if ( q.second < 1<<1  ) bits = 1;
      else if ( q.second < 1<<2  ) bits = 2;
      else if ( q.second < 1<<3  ) bits = 3;
      else if ( q.second < 1<<4  ) bits = 4;
      else if ( q.second < 1<<5  ) bits = 5;
      else if ( q.second < 1<<6  ) bits = 6;
      else if ( q.second < 1<<7  ) bits = 7;
      else if ( q.second < 1<<8  ) bits = 8;
      else if ( q.second < 1<<9  ) bits = 9;
      else if ( q.second < 1<<10 ) bits = 10;
      else if ( q.second < 1<<11 ) bits = 11;
      else if ( q.second < 1<<12 ) bits = 12;
      else if ( q.second < 1<<13 ) bits = 13;
      else if ( q.second < 1<<14 ) bits = 14;
      else if ( q.second < 1<<15 ) bits = 15;
      bits += 1;
      id_str << ",Lv" << q.first << ":" << bits;
      num_bits += bits;
    }
    string idspec = id_str.str();
    str << "<readout name=\"" << ro_name << "\">" << endl
        << "\t<id>"
        << idspec
        << "</id>  <!-- Number of bits: " << num_bits << " -->" << endl
        << "</readout>" << endl;

    /// Create ID Descriptors and readout configurations
    try   {
      IDDescriptor dsc(ro_name,idspec);
      description.addIDSpecification(dsc);
      Readout ro(ro_name);
      ro.setIDDescriptor(dsc);
      description.addReadout(ro);
      SensitiveDetector sd = description.sensitiveDetector(f.first.name());
      sd.setHitsCollection(ro.name());
      sd.setReadout(ro);
      printout(INFO,pref,"DetElementCreator: ++ Setting up readout for subdetector:%-24s id:%04X",
               f.first.name(), f.first.id());
    }
    catch(std::exception& e)    {
      printout(ERROR,pref,"DetElementCreator: ++ FAILED to setup readout for subdetector:%-24s id:%04X [%s]",
               f.first.name(), f.first.id(), e.what());
    }
  }
  printout(INFO,pref,"DetElementCreator: "
           "+++++++++++++++ ID Descriptor generation  ++++++++++++++++++++++++++++");
  printout(INFO,"",str.str().c_str());
  char volid[32];
  for(auto& p : all_placements )  {
    PlacedVolume place = p.first;
    Volume vol = place.volume();
    ::snprintf(volid,sizeof(volid),"Lv%d", p.second.first);
    printout(DEBUG,pref, "DetElementCreator: ++ Set volid (%-24s): %-6s = %3d  -> %s  (%p)",
             vol.isSensitive() ? vol.sensitiveDetector().name() : "Not Sensitive",
             volid, p.second.second, place.name(), place.ptr());
    place.addPhysVolID(volid, p.second.second);
  }
  printout(ALWAYS, pref, "DetElementCreator: ++ Instrumented %ld subdetectors with %d "
           "DetElements %d sensitives out of %d volumes and %ld sensitive placements.",
           fields.size(),total.elements,total.sensitives,total.volumes,all_placements.size());
}

/// Generate the name of the DetElement object from the placed volume
string DetElementCreator::detElementName(PlacedVolume pv)  const    {
  if ( pv.isValid() )  {
    string nam = pv.name();
    size_t idx = string::npos; // nam.rfind('_');
    string nnam = nam.substr(0, idx);
    return nnam;
  }
  except("DetElementCreator","++ Cannot deduce name from invalid PlacedVolume handle!");
  return string();
}

/// Create a new detector element
DetElement DetElementCreator::createElement(const char* /* debug_tag */, PlacedVolume pv, int id) {
  string     name = detElementName(pv);
  DetElement det(name, id);
  det.setPlacement(pv);
  /*
    printout(INFO,"DetElementCreator","++ Created detector element [%s]: %s (%s)  %p",
    debug_tag, det.name(), name.c_str(), det.ptr());
  */
  return det;
}

/// Create the top level detectors
void DetElementCreator::createTopLevelDetectors(PlacedVolume pv)   {
  auto& data = stack.back();
  data.element = current_detector = addSubdetector(detElementName(pv), pv, true);
}

/// Add new subdetector to the detector description
DetElement DetElementCreator::addSubdetector(const std::string& nam, PlacedVolume pv, bool volid)  {
  Detectors::iterator idet = subdetectors.find(nam);
  if ( idet == subdetectors.end() )   {
    DetElement det(nam, description.detectors().size()+1);
    det.setPlacement(pv);
    if ( volid )  {
      det.placement().addPhysVolID("system",det.id());
    }
    idet = subdetectors.emplace(nam,det).first;
    description.add(det);
    printout(printLevel,"DetElementCreator","++ Added sub-detector element: %s",det.path().c_str());
  }
  return idet->second;
}

/// Callback to output PlacedVolume information of an single Placement
int DetElementCreator::operator()(PlacedVolume pv, int vol_level)   {
  if ( detector_volume_level > 0 )   {
    Material mat = pv.volume().material();
    if ( mat == sensitive_material )  {
      Data& data = stack.back();
      data.sensitive     = true;
      data.has_sensitive = true;
      ++data.vol_count;
      int   idx   = pv->GetMotherVolume()->GetIndex(pv.ptr())+1;
      auto& cnt   = leafCount[make_pair(current_detector,vol_level)];
      cnt.first   = std::max(cnt.first,idx);
      ++cnt.second;
      all_placements[pv] = make_pair(vol_level,idx);
      return 1;
    }
  }
  return 0;
}

/// Callback to output PlacedVolume information of an entire Placement
int DetElementCreator::process(PlacedVolume pv, int lvl, bool recursive)   {
  int ret = 0;
  string pv_nam = pv.name();
  if ( detector_volume_level > 0 ||
       ( (!detector_volume_match.empty() &&
          pv_nam.find(detector_volume_match) != string::npos) &&
         (detector_volume_veto.empty() ||
          pv_nam.find(detector_volume_veto)  == string::npos)  )  )
  {
    stack.emplace_back(Data(pv));
    if ( 0 == detector_volume_level )   {
      detector_volume_level = stack.size();
      createTopLevelDetectors(pv);
    }
    ret = PlacedVolumeProcessor::process(pv,lvl,recursive);
    /// Complete structures if the stack size is > 3!
    if ( stack.size() > detector_volume_level )   {
      // Note: short-cuts to entries in the stack MUST be local and
      // initialized AFTER the call to "process"! The vector may be resized!
      auto& data   = stack.back();
      auto& parent = stack[stack.size()-2];
      auto& counts = counters[current_detector];
      if ( data.sensitive )   {
        /// If this volume is sensitve, we must attach a sensitive detector handle
        if ( !current_sensitive.isValid() )  {
          SensitiveDetector sd = description.sensitiveDetector(current_detector.name());
          if ( !sd.isValid() )  {
            sd = SensitiveDetector(current_detector.name(), sensitive_type);
            current_detector->flag |= DetElement::Object::HAVE_SENSITIVE_DETECTOR;
            description.add(sd);
            
          }
          current_sensitive = sd;
        }
        pv.volume().setSensitiveDetector(current_sensitive);
        ++counts.sensitives;
      }
      ++counts.volumes;
      bool added = false;
      if ( data.vol_count > 0 )    {
        parent.daughter_count  += data.vol_count;
        parent.daughter_count  += data.daughter_count;
        data.has_sensitive      = true;
      }
      else   {
        parent.daughter_count  += data.daughter_count;
        data.has_sensitive      = (data.daughter_count>0);
      }

      if ( data.has_sensitive )    {
        // If we have sensitive elements at this level or below,
        // we must complete the DetElement hierarchy
        if ( data.pv.volIDs().empty() )   {
          char text[32];
          ::snprintf(text, sizeof(text), "Lv%d", lvl);
          data.pv.addPhysVolID(text, data.pv->GetMotherVolume()->GetIndex(data.pv.ptr())+1);
        }
        else   {
          AllPlacements::const_iterator e = all_placements.find(data.pv);
          if ( e != all_placements.end() && (*e).second.first != lvl)  {
            printout(ERROR,"DetElementCreator","PLacement VOLID error: %d <> %d",lvl,(*e).second.first);
          }
        }

        for(size_t i=1; i<stack.size(); ++i)   {
          auto& d = stack[i];
          auto& p = stack[i-1];
          if ( !d.element.isValid() )    {
            d.element = createElement("Element", d.pv, current_detector.id());
            (i==1 ? current_detector : p.element).add(d.element);
            ++counts.elements;
          }
          p.has_sensitive = true;
        }
        printout(printLevel,"DetElementCreator",
                 "++ Assign detector element: %s (%p, %ld children) to %s (%p) with %ld vols",
                 data.element.name(), data.element.ptr(), data.element.children().size(),
                 parent.element.name(), parent.element.ptr(), data.vol_count);
        added = true;
        // It is simpler to collect the volumes and later assign the volids
        // rather than checking if the volid already exists.
        int vol_level = lvl;
        int idx = data.pv->GetMotherVolume()->GetIndex(data.pv.ptr())+1;
        all_placements[data.pv] = make_pair(vol_level,idx); // 1...n
        // Update counters
        auto& cnt_det   = leafCount[make_pair(current_detector,vol_level)];
        cnt_det.first   = std::max(cnt_det.first,idx);
        cnt_det.second += 1;
        printout(printLevel,"DetElementCreator","++ [%ld] Added element: %s",
                 stack.size(), data.element.path().c_str());
      }
      if ( !added && data.element.isValid() )  {
        printout(WARNING,"MEMORY-LEAK","Level:%3d Orpahaned DetElement:%s Daugthers:%d Parent:%s",
                 int(stack.size()), data.element.name(), data.vol_count, parent.pv.name());
      }
    }
    /// Now the cleanup kicks in....
    if ( stack.size() == detector_volume_level )  {
      current_sensitive = SensitiveDetector();
      current_detector = DetElement();
      detector_volume_level = 0;
      ret = 0;
    }
    stack.pop_back();
  }
  else if ( lvl < max_volume_level )  {
    //printout(printLevel, "", "+++ Skip volume %s", pv_nam.c_str());
    ret = PlacedVolumeProcessor::process(pv,lvl,recursive);
  }
  return ret;
}

static void* create_object(Detector& description, int argc, char** argv)   {
  PrintLevel prt  = DEBUG;
  size_t sd_level = 99999;
  string sd_mat, sd_match, sd_veto, sd_type, detector;
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-material",argv[i],5) )
      sd_mat   = argv[++i];
    else if ( 0 == ::strncmp("-match",argv[i],5) )
      sd_match = argv[++i];
    else if ( 0 == ::strncmp("-detector",argv[i],5) )
      detector = argv[++i];
    else if ( 0 == ::strncmp("-veto",argv[i],5) )
      sd_veto  = argv[++i];
    else if ( 0 == ::strncmp("-type",argv[i],5) )
      sd_type  = argv[++i];
    else if ( 0 == ::strncmp("-level",argv[i],5) )
      sd_level = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-print",argv[i],5) )
      prt = decodePrintLevel(argv[++i]);
    else
      break;
  }
  if ( sd_mat.empty() || sd_match.empty() || sd_type.empty() )   {
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                            \n"
      "     name:  factory name DD4hep_ROOTGDMLParse                           \n"
      "     -material <string>  Sensitive material name (identifier)           \n"
      "     -match    <string>  Matching string for subdetector identification \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  PlacedVolumeProcessor* proc = new DetElementCreator(description, detector, sd_match, sd_veto, sd_type, sd_mat, sd_level, prt);
  return (void*)proc;
}

// first argument is the type from the xml file
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_DetElementCreator,create_object)

