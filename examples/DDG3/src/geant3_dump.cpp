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
#define DD4HEP_MUST_USE_DETECTORIMP_H
#include <DD4hep/Detector.h>
#include <DD4hep/DetectorImp.h>
#include <DD4hep/Memory.h>
#include <DD4hep/DD4hepUI.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Printout.h>
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/PluginCreators.h>
#include <DD4hep/VolumeProcessor.h>
#include <DD4hep/DetectorProcessor.h>
#include <DD4hep/DD4hepRootPersistency.h>
#include <XML/DocumentHandler.h>
#include <XML/XMLElements.h>
#include <XML/XMLTags.h>

// ROOT includes
#include <TInterpreter.h>
#include <TGeoElement.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TSystem.h>
#include <TClass.h>
#include <TRint.h>
#include <TGDMLMatrix.h>

// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <sstream>

using namespace dd4hep;
using namespace dd4hep::detail;

namespace myext  {
  typedef std::vector<DetElement>   ElementPath;
  typedef std::vector<PlacedVolume> PlacementPath;

  /// Determine top level element (=world) for any element walking up the detector element tree
  DetElement topElement(DetElement child)   {
    if ( child.isValid() )   {
      if ( child.parent().isValid() )
        return topElement(child.parent());
      return child;
    }
    throw std::runtime_error("dd4hep: DetElement cannot determine top parent (world) [invalid handle]");
  }

  void elementPath(DetElement element, PlacementPath& det_nodes) {
    for(DetElement par = element; par.isValid(); par = par.parent())  {
      PlacedVolume pv = par.placement();
      if ( pv.isValid() )  {
        det_nodes.emplace_back(pv);
      }
    }
  }
  
  /// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
  bool findChildByName(PlacedVolume parent, PlacedVolume child, detail::tools::PlacementPath& path) {
    if ( parent.isValid() && child.isValid() ) {
      // Check self
      if ( 0 == ::strcmp(parent.ptr()->GetName(),child.ptr()->GetName()) ) {
        path.emplace_back(child);
        return true;
      }
      TIter next(parent->GetVolume()->GetNodes());
      // Now check next layer children
      for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
        if ( 0 == ::strcmp(daughter->GetName(),child.ptr()->GetName()) ) {
          path.emplace_back(daughter);
          return true;
        }
      }
      next.Reset();
      // Finally crawl down the tree
      for (TGeoNode *daughter = (TGeoNode*) next(); daughter; daughter = (TGeoNode*) next()) {
        detail::tools::PlacementPath sub_path;
        bool res = findChildByName(daughter, child, sub_path);
        if (res) {
          path.insert(path.end(), sub_path.begin(), sub_path.end());
          path.emplace_back(daughter);
          return res;
        }
      }
    }
    return false;
  }

  void makePlacementPath(PlacementPath det_nodes, PlacementPath& all_nodes)   {
    for (size_t i = 0, n = det_nodes.size(); n > 0 && i < n-1; ++i)   {
      PlacedVolume e1 = det_nodes[i + 1];
      PlacedVolume e2 = det_nodes[i];
      if (!findChildByName(e1, e2, all_nodes))   {
        throw std::runtime_error("dd4hep: DetElement cannot determine placement path of "
                                 + std::string(det_nodes[i].name()) + " [internal error]");
      }
    }
    if ( det_nodes.size() > 0 )   {
      all_nodes.emplace_back(det_nodes.back());
    }
  }

  void placementPath(DetElement element, PlacementPath& all_nodes)  {
    PlacementPath det_nodes;
    elementPath(element,det_nodes);
    makePlacementPath(std::move(det_nodes), all_nodes); 
  }
  /// Assemble the path of the PlacedVolume selection
  std::string placementPath(const PlacementPath& nodes, bool reverse=false)  {
    std::string path = "";
    if ( reverse )  {
      for(PlacementPath::const_reverse_iterator i=nodes.rbegin();i!=nodes.rend();++i)
        path += "/" + std::string((*i)->GetName());
    }
    else  {
      for(PlacementPath::const_iterator i=nodes.begin();i!=nodes.end();++i)
        path += "/" + std::string((*i)->GetName());
    }
    return path;
  }
  /// Assemble the path of the PlacedVolume selection
  std::string placementPath(DetElement element)  {
    PlacementPath path;
    placementPath(element,path);
    return placementPath(std::move(path));
  }
  /// Find DetElement as child of a parent by its relative or absolute path
  DetElement findDaughterElement(DetElement parent, const std::string& subpath)  {
    if ( parent.isValid() )   {
      size_t idx = subpath.find('/',1);
      if ( subpath[0] == '/' )   {
        DetElement top = topElement(parent);
        if ( idx == std::string::npos ) return top;
        return findDaughterElement(top,subpath.substr(idx+1));
      }
      if ( idx == std::string::npos )
        return parent.child(subpath);
      std::string name = subpath.substr(0,idx);
      DetElement node = parent.child(name);
      if ( node.isValid() )   {
        return findDaughterElement(node,subpath.substr(idx+1));
      }
      throw std::runtime_error("dd4hep: DetElement "+parent.path()+" has no child named:"+name+" [No such child]");
    }
    throw std::runtime_error("dd4hep: Cannot determine child with path "+subpath+" from invalid parent [invalid handle]");
  }
  /// Find DetElement as child of the top level volume by its absolute path
  DetElement findElement(const Detector& description, const std::string& path)   {
    const auto children = description.world().children();
    for( const auto& c : children )
      std::cout << c.first << " == " << c.second.name() << std::endl;
    return findDaughterElement(description.world(),path);
  }
}


/// Basic entry point to print out the detector element hierarchy
/**
 *  Factory: DD4hep_DetectorDump, DD4hep_DetectorVolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
template <int flag>
static long dump_detelement_tree(Detector& description, int argc, char** argv) {
  using VolIDs = PlacedVolume::VolIDs;
  /// Local helper class
  struct Actor {
    Detector& descr;
    std::string component { "g3_dump" };
    std::string path     { };
    DetElement det_world { };
    long count          = 0;
    int  have_match     = -1;
    int  analysis_level = 999999;
    bool dump_materials = false;
    bool dump_shapes    = false;
    bool dump_positions = false;
    bool dump_volids    = false;
    bool sensitive_only = false;

    /// Initializing constructor
    Actor(Detector& det) : descr(det) {
      det_world = descr.world();
    }
    /// Default destructor
    ~Actor() {
      printout(ALWAYS,component, "+++ Scanned a total of %ld elements.",count);
    }
    void parse_args(int ac, char** av)   {
      for(int i=0; i<ac; ++i)  {
        if      ( ::strncmp(av[i],"--sensitive",     5)==0 ) { sensitive_only = true;  }
        else if ( ::strncmp(av[i], "-sensitive",     5)==0 ) { sensitive_only = true;  }
        else if ( ::strncmp(av[i], "--no-sensitive", 8)==0 ) { sensitive_only = false; }
        else if ( ::strncmp(av[i], "-materials",     4)==0 ) { dump_materials = true;  }
        else if ( ::strncmp(av[i], "--materials",    5)==0 ) { dump_materials = true;  }
        else if ( ::strncmp(av[i], "-shapes",        4)==0 ) { dump_shapes    = true;  }
        else if ( ::strncmp(av[i], "--shapes",       5)==0 ) { dump_shapes    = true;  }
        else if ( ::strncmp(av[i], "-positions",     4)==0 ) { dump_positions = true;  }
        else if ( ::strncmp(av[i], "--positions",    5)==0 ) { dump_positions = true;  }
        else if ( ::strncmp(av[i], "-no-sensitive",  7)==0 ) { sensitive_only = false; }
        else if ( ::strncmp(av[i], "--volids",       5)==0 ) { dump_volids    = true;  }
        else if ( ::strncmp(av[i], "-volids",        5)==0 ) { dump_volids    = true;  }
        else if ( ::strncmp(av[i], "--detector",     5)==0 ) { path = av[++i];       }
        else if ( ::strncmp(av[i], "-detector",      5)==0 ) { path = av[++i];       }
        else if ( ::strncmp(av[i], "--level",        5)==0 ) { analysis_level = ::atol(av[++i]); }
        else if ( ::strncmp(av[i], "-level",         5)==0 ) { analysis_level = ::atol(av[++i]); }
        else   {
          std::cout << "  "
                    << (flag==0 ? "DD4hep_DetectorDump" : "DD4hep_DetectorVolumeDump") << " -arg [-arg]   \n\n"
                    << "         Dump " << (flag==0 ? "DetElement" : "Detector volume") << " tree.          \n"
                    << "         Configure produced output information using the following options:       \n\n"
            "   --sensitive             Process only sensitive volumes.                                \n"
            "    -sensitive             dto.                                                           \n"
            "   --no-sensitive          Invert sensitive only flag.                                    \n"
            "    -no-sensitive          dto.                                                           \n"
            "   --shapes                Print shape information.                                       \n"
            "    -shapes                dto.                                                           \n"
            "   --positions             Print position information.                                    \n"
            "    -positions             dto.                                                           \n"
            "   --materials             Print material information.                                    \n"
            "    -materials             dto.                                                           \n"
            "   --detector    <path>    Process elements only if <path> is part of the DetElement path.\n"
            "    -detector    <path>    dto.                                                           \n"
            "    -level       <number>  Maximal depth to be explored by the scan                       \n"
            "   --level       <number>  dto.                                                           \n"
            "    -volids                Print volume identifiers of placements.                        \n"
            "   --volids                dto.                                                           \n"
            "\tArguments given: " << arguments(ac,av) << std::endl << std::flush;        
          ::exit(EINVAL);
        }
      }
    }
    IDDescriptor get_id_descriptor(PlacedVolume pv)   {
      if ( pv.isValid() )  {
        Volume v = pv.volume();
        SensitiveDetector sd = v.sensitiveDetector();
        if ( sd.isValid() )   {
          IDDescriptor dsc = sd.readout().idSpec();
          if ( dsc.isValid() ) return dsc;
        }
        for (Int_t idau = 0, ndau = v->GetNdaughters(); idau < ndau; ++idau)  {
          IDDescriptor dsc = get_id_descriptor(v->GetNode(idau));
          if ( dsc.isValid() ) return dsc;
        }
      }
      return IDDescriptor();
    }
    void validate_id_descriptor(DetElement de, IDDescriptor& desc)   {
      desc = (!de.parent() || de.parent() == det_world)
        ? IDDescriptor() : get_id_descriptor(de.placement());
    }

    long dump(DetElement de, int level, IDDescriptor& id_desc, VolIDs chain)   {
      char fmt[256];
      PlacedVolume place = de.placement();
      const DetElement::Children& children = de.children();
      bool use_elt = path.empty() || de.path().find(path) != std::string::npos;

      if ( have_match < 0 && use_elt )  {
        have_match = level;
      }

      use_elt &= ((level-have_match) <= analysis_level);
      if ( !place.isValid() )    {
        std::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s DetElement with INVALID PLACEMENT!", level+1, 2*level+1);
        printout(ERROR,component, fmt, "", de.path().c_str());
        use_elt = false;
      }

      if ( place.isValid() && de != det_world )    {
        chain.insert(chain.end(), place.volIDs().begin(), place.volIDs().end());
      }
      if ( use_elt )   {
        if ( !sensitive_only || 0 != de.volumeID() )  {
          char sens = place.isValid() && place.volume().isSensitive() ? 'S' : ' ';
          switch( flag )  {
          case 0:
            ++count;
            if ( de.placement() == de.idealPlacement() )  {
              std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p  %%c", level+1, 2*level+1);
              printout(INFO, component, fmt, "", de.path().c_str(), int(children.size()),
                       (unsigned long)de.volumeID(), (void*)place.ptr(), sens);
              break;
            }
            std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds %%s NumDau:%%d VolID:%%08X Place:%%p [ideal:%%p aligned:%%p]  %%c",
                          level+1, 2*level+1);
            printout(INFO, component, fmt, "", de.path().c_str(), int(children.size()),
                     (unsigned long)de.volumeID(), (void*)de.idealPlacement().ptr(),
                     (void*)place.ptr(), sens);
            break;
          case 1:
            ++count;
            std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds Detector: %%s NumDau:%%d VolID:%%p", level+1, 2*level+1);
            printout(INFO, component, fmt, "", de.path().c_str(), int(children.size()), (void*)de.volumeID());
            if ( de.placement() == de.idealPlacement() )  {
              std::snprintf(fmt, sizeof(fmt), "%03d %%-%ds Placement: %%s  %%c", level+1, 2*level+3);

              printout(INFO,component,fmt,"", myext::placementPath(de).c_str(), sens);
              printout(INFO,component,fmt,"", de.placementPath().c_str(), sens);
              break;
            }
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Placement: %%s  [ideal:%%p aligned:%%p] %%c",
                          level+1,2*level+3);
            printout(INFO,component,fmt,"", de.placementPath().c_str(),
                     (void*)de.idealPlacement().ptr(), (void*)place.ptr(), sens);          
            break;
          default:
            break;
          }
          if ( (dump_materials || dump_shapes) && place.isValid() )  {
            Volume vol = place.volume();
            Material mat = vol.material();
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Material: %%-12s Shape: %%s", level+1,2*level+3);
            printout(INFO,component,fmt,"", mat.name(), toStringSolid(vol->GetShape()).c_str());
          }
          if ( dump_positions && place.isValid() )  {
            Position pos = place.position();
            Box      box = place.volume().solid();
            double   loc[3] = {0,0,0}, world[3] = {0,0,0};
            TGeoHMatrix   tr = de.nominal().worldTransformation();
            tr.LocalToMaster(loc, world);
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds BBox:     (%%9.4f,%%9.4f,%%9.4f) [cm]", level+1,2*level+3);
            printout(INFO,component,fmt,"", box.x(), box.y(), box.z());
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Position: (%%9.4f,%%9.4f,%%9.4f) [cm] w/r to mother", level+1,2*level+3);
            printout(INFO,component,fmt,"", pos.X(), pos.Y(), pos.Z());
            std::snprintf(fmt,sizeof(fmt), "%03d %%-%ds Position: (%%9.4f,%%9.4f,%%9.4f) [cm] w/r to world",  level+1,2*level+3);
            printout(INFO,component,fmt,"", world[0], world[1], world[2]);
          }
          if ( dump_volids && !place.volIDs().empty() )    {
            std::stringstream log;
            log << " VID:";
            for( const auto& i : chain )
              log << " " << i.first << ':' << std::dec << std::setw(2) << i.second;
            if ( id_desc.isValid() )  {
              log << " (encoded:0x" << std::setfill('0') << std::setw(8) << std::hex
                  << id_desc.encode(chain)
                  << std::setfill(' ') << std::dec << ") ";
            }
            std::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s", level+1, 2*level+1);
            printout(INFO,component, fmt, "", log.str().c_str());
          }
        }
      }
      for ( const auto& c : children )   {
        validate_id_descriptor(c.second, id_desc);
        dump(c.second, level+1, id_desc, chain);
      }
      return 1;
    }
  };
  VolIDs       chain;
  IDDescriptor id_descriptor;
  Actor        actor(description);
  actor.parse_args(argc, argv);
  return actor.dump(description.world(), 0, id_descriptor, std::move(chain));
}
DECLARE_APPLY(geant3_DetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(geant3_DetectorVolumeDump,dump_detelement_tree<1>)

/// Basic entry point to print out the volume hierarchy
/**
 *  Factory: DD4hep_VolumeDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dump_volume_tree(Detector& description, int argc, char** argv) {
  struct Actor {
    Detector&        description;
    bool             m_printPathes         = false;
    bool             m_printVolIDs         = false;
    bool             m_printShapes         = false;
    bool             m_printPointers       = false;
    bool             m_printPositions      = false;
    bool             m_printVis            = false;
    bool             m_printMaterials      = false;
    bool             m_printSensitivesOnly = false;
    long             m_printMaxLevel       = 999999;
    long             m_numNodes            = 0;
    long             m_numShapes           = 0;
    long             m_numSensitive        = 0;
    long             m_numMaterial         = 0;
    long             m_numMaterialERR      = 0;
    bool             m_topStat             = false;
    std::string      m_detector;
    std::string                currTop;
    std::map<std::string,long> top_counts;
    
    Actor(Detector& desc, int ac, char** av) : description(desc)  {
      m_detector = "/world";
      for(int i=0; i<ac; ++i)  {
        char c = ::tolower(av[i][0]);
        char* p = av[i];
        if ( c == '-' ) { ++p; c = ::tolower(av[i][1]); }
        if ( c == '-' ) { ++p; c = ::tolower(av[i][1]); }
        if      ( ::strncmp(p,"volume_ids",3) == 0  ) m_printVolIDs         = true;
        else if ( ::strncmp(p,"level",3)      == 0  ) m_printMaxLevel       = ::atol(av[++i]);
        else if ( ::strncmp(p,"materials",3)  == 0  ) m_printMaterials      = true;
        else if ( ::strncmp(p,"pathes",3)     == 0  ) m_printPathes         = true;
        else if ( ::strncmp(p,"positions",3)  == 0  ) m_printPositions      = true;
        else if ( ::strncmp(p,"pointers",3)   == 0  ) m_printPointers       = true;
        else if ( ::strncmp(p,"shapes",3)     == 0  ) m_printShapes         = true;
        else if ( ::strncmp(p,"sensitive",3)  == 0  ) m_printSensitivesOnly = true;
        else if ( ::strncmp(p,"topstats",3)   == 0  ) m_topStat             = true;
        else if ( ::strncmp(p,"vis",3)        == 0  ) m_printVis            = true;
        else if ( ::strncmp(p,"detector",3)   == 0  ) m_detector            = av[++i];
        else if ( ::strncmp(p,"help",3)       == 0  )   {
          std::cout <<
            "Usage: -plugin <name> -arg [-arg]                                                   \n"
            "     -detector <string> Top level DetElement path. Default: '/world'                \n"
            "     -pathes            Print DetElement pathes                                     \n"
            "     -level    <number> Maximal depth to be explored by the scan                    \n"
            "     -positions         Print placement positions                                   \n"
            "     -volume_ids        Print placement volume IDs                                  \n"
            "     -materials         Print volume materials                                      \n"       
            "     -pointers          Debug: Print pointer values                                 \n"       
            "     -shapes            Print shape information                                     \n"
            "     -vis               Print visualisation attribute name(s) if present            \n"
            "     -sensitive         Only print information for sensitive volumes                \n"
            "     -topstats          Print statistics about top level node                       \n"       
            "\tArguments given: " << arguments(ac,av) << std::endl << std::flush;
          _exit(0);
        }
      }
      if ( m_printMaxLevel < 999999 )
        printout(ALWAYS,"VolumeDump","+++ Maximal print level:   %ld",m_printMaxLevel);
      if ( !m_detector.empty() )
        printout(ALWAYS,"VolumeDump","+++ Subdetector path:      %s",m_detector.c_str());
      printout(ALWAYS,"VolumeDump","+++ Printing positions:    %s",yes_no(m_printPositions));
      printout(ALWAYS,"VolumeDump","+++ Printing shapes:       %s",yes_no(m_printShapes));
      printout(ALWAYS,"VolumeDump","+++ Printing materials:    %s",yes_no(m_printMaterials));
      printout(ALWAYS,"VolumeDump","+++ Printing volume ids:   %s",yes_no(m_printVolIDs));
      printout(ALWAYS,"VolumeDump","+++ Printing visattrs:     %s",yes_no(m_printVis));
      printout(ALWAYS,"VolumeDump","+++ Print only sensitives: %s",yes_no(m_printSensitivesOnly));
    }
    ~Actor()  {
      printout(ALWAYS,"VolumeDump",
               "+++ Checked %ld physical volume placements.     %3ld are sensitive.",
               m_numNodes, m_numSensitive);
      if ( m_printMaterials )  {
        printout(ALWAYS,"VolumeDump",
                 "+++ Checked %ld materials in volume placements. %3ld are BAD.",
                 m_numMaterial, m_numMaterialERR);
      }
      if ( m_printShapes )   {
        printout(ALWAYS,"VolumeDump","+++ Checked %ld shapes.", m_numShapes);
      }
      if ( m_topStat )  {
        for(const auto& t : top_counts)  {
          if ( t.second > 1 )
            printout(ALWAYS,"VolumeDump",
                     "+++     Top node: %-32s     %8ld placements.",t.first.c_str(),t.second);
        }
      }
    }

    long dump(std::string prefix, TGeoNode* ideal, TGeoNode* aligned, int level, PlacedVolume::VolIDs volids)  {
      char fmt[128];
      std::stringstream log;
      PlacedVolume pv(ideal);
      bool sensitive = false;
      std::string opt_info, pref = std::move(prefix);

      if ( level >= m_printMaxLevel )    {
        return 1;
      }
      ++m_numNodes;
      if ( level == 0 )
        currTop = "";
      else if ( level == 1 )   {
        currTop = ideal->GetVolume()->GetName();
        ++top_counts[currTop];
      }
      else if ( level > 1 )   {
        ++top_counts[currTop];
      }

      if ( m_printPathes )   {
        pref += "/";
        pref += aligned->GetName();
      }
      /// 
      TGeoVolume* mother = ideal ? ideal->GetMotherVolume() : nullptr;
      if ( m_printPositions || m_printVolIDs )  {
        if ( m_printPointers )    {
          if ( ideal != aligned )
            std::snprintf(fmt,sizeof(fmt),"Ideal:%p Aligned:%p ",
                          (void*)ideal,(void*)aligned);
          else
            std::snprintf(fmt,sizeof(fmt),"Ideal:%p MotherVol:%p",
                          (void*)ideal, (void*)mother);
          log << fmt;
        }
        // Top level volume! have no volume ids
        if ( m_printVolIDs && ideal && ideal->GetMotherVolume() )  {
          PlacedVolume::VolIDs vid = pv.volIDs();
          if ( !vid.empty() )  {
            sensitive = true;
            log << " VolID: ";
            volids.insert(volids.end(),vid.begin(),vid.end());
            for( const auto& i : volids )  {
              std::snprintf(fmt, sizeof(fmt), "%s:%2d ",i.first.c_str(),i.second);
              log << fmt;
            }
            if ( !vid.empty() || pv.volume().isSensitive() )  {
              SensitiveDetector sd = pv.volume().sensitiveDetector();
              if ( sd.isValid() )  {
                IDDescriptor dsc = sd.readout().idSpec();
                if ( dsc.isValid() )  {
                  log << std::hex << " (0x" << std::setfill('0') << std::setw(8)
                      << dsc.encode(volids)
                      << std::setfill(' ') << std::dec << ") ";
                }
              }
            }
          }
        }
        opt_info = log.str();
        log.str("");
      }
      ///  
      if ( m_printVis && pv.volume().visAttributes().isValid() )   {
        log << " Vis:" << pv.volume().visAttributes().name();
        opt_info += log.str();
        log.str("");
      }
      TGeoVolume* volume = ideal ? ideal->GetVolume() : 0;
      if ( !m_printSensitivesOnly || (m_printSensitivesOnly && sensitive) )  {
        Volume vol = pv.volume();
        char  sens = vol.isSensitive() ? 'S' : ' ';
        if ( m_printPointers )    {
          if ( ideal == aligned )  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s [Ideal:%p Vol:%p MotherVol:%p] %%-%ds %%-16s Vol:%%s shape:%%s \t %c %%s",
                          level+1,(void*)ideal,(void*)vol, (void*)mother, 2*level+1, sens);
          }
          else  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s Ideal:%p Aligned:%p %%-%ds %%-16s Vol:%%s shape:%%s %c %%s",
                          level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        else  {
          if ( ideal == aligned )  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds %%-16s Vol:%%s shape:%%s \t %c %%s",
                          level+1,2*level+1,sens);
          }
          else  {
            std::snprintf(fmt,sizeof(fmt),"%03d %%s Ideal:%p Aligned:%p %%-%ds %%-16s Vol:%%s shape:%%s %c %%s",
                          level+1,(void*)ideal,(void*)aligned,2*level+1,sens);
          }
        }
        const auto* sh = volume ? volume->GetShape() : nullptr;
        printout(INFO,"VolumeDump",fmt,pref.c_str(),"",
                 aligned->GetName(),
                 vol.name(),
                 sh ? sh->IsA()->GetName() : "[Invalid Shape]",
                 opt_info.c_str());
        if ( sens == 'S' ) ++m_numSensitive;
      }
      if ( m_printMaterials )   {
        Volume   vol = pv.volume();
        Material mat = vol.material();
        TGeoMaterial* mptr = mat->GetMaterial();
        bool ok = mat.A() == mptr->GetA() && mat.Z() == mptr->GetZ();
        std::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds Material: %%-16s A:%%6.2f %%6.2f  Z:%%6.2f %%6.2f",
                      level+1,2*level+1);
        ++m_numMaterial;
        if ( !ok ) ++m_numMaterialERR;
        printout(ok ? INFO : ERROR, "VolumeDump", fmt,
                 "  ->", "", mat.name(), mat.A(), mptr->GetA(), mat.Z(), mptr->GetZ());
      }
      log.str("");
      if ( m_printShapes )   {
        log << "Shape: " << toStringSolid(pv.volume().solid()) << " \t";
      }
      if ( m_printPositions )  {
        if ( ideal )  {
          const double* trans = ideal->GetMatrix()->GetTranslation();
          std::snprintf(fmt, sizeof(fmt), "Pos: (%f,%f,%f) ",trans[0],trans[1],trans[2]);
        }
        else  {
          std::snprintf(fmt, sizeof(fmt), " <ERROR: INVALID Translation matrix> ");
        }
        log << fmt << " \t";
      }
      if ( !log.str().empty() )  {
        std::snprintf(fmt,sizeof(fmt),"%03d %%s %%-%ds %%s",level+1,2*level+1);
        printout(INFO, "VolumeDump", fmt, "  ->", "", log.str().c_str());
      }
      for (Int_t idau = 0, ndau = aligned->GetNdaughters(); idau < ndau; ++idau)  {
        if ( ideal )   {
          TGeoNode*   ideal_daughter   = ideal->GetDaughter(idau);
          const char* daughter_name    = ideal_daughter->GetName();
          TGeoNode*   aligned_daughter = volume->GetNode(daughter_name);
          dump(pref, ideal_daughter, aligned_daughter, level+1, volids);
        }
        else  {
          printout(ERROR,"VolumeDump"," <ERROR: INVALID IDEAL Translation matrix>: %s",aligned->GetName());
        }
      }
      return 1;
    }
    ///
    int operator()()   {
      PlacedVolume pv;
      DetElement   top = description.world();
      detail::tools::PlacementPath path;

      if ( m_detector != "/world" )   {
        top = myext::findElement(description,m_detector);
        if ( !top.isValid() )  {
          except("VolumeDump","+++ Invalid DetElement path: %s",m_detector.c_str());
        }
      }
      if ( !top.placement().isValid() )   {
        except("VolumeDump","+++ Invalid DetElement placement: %s",m_detector.c_str());
      }
      std::string place = top.placementPath();
      detail::tools::placementPath(top, path);
      pv = detail::tools::findNode(description.world().placement(),place);
      if ( !pv.isValid() )   {
        except("VolumeDump","+++ Invalid placement verification for placement:%s",place.c_str());
      }
      return this->dump("", top.placement().ptr(), pv.ptr(), 0, PlacedVolume::VolIDs());
    }
  };
  Actor actor(description, argc, argv);
  return actor();
}
DECLARE_APPLY(geant3_VolumeDump,dump_volume_tree)
