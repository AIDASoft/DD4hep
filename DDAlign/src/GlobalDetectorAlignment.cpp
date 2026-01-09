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
#include <DDAlign/GlobalDetectorAlignment.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/InstanceCount.h>
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/Printout.h>
#include <DD4hep/detail/Handle.inl>
#include <DD4hep/detail/DetectorInterna.h>

// ROOT include files
#include <TGeoMatrix.h>
#include <TGeoManager.h>

#ifdef __GNUC__    // Disable some diagnostics.
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

using LevelElements = std::vector<std::pair<int,dd4hep::DetElement> >;
using namespace dd4hep::align;
using dd4hep::printout;
using dd4hep::INFO;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace align {

    /// Global alignment data container
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_DDALIGN
     */
    class GlobalAlignmentData : public NamedObject  {
    public:
      GlobalAlignment              global;
      std::vector<GlobalAlignment> volume_alignments;
      
    public:
      GlobalAlignmentData(const std::string& path)
        : NamedObject(path, "global-alignment")
      {
        this->global = GlobalAlignment( path );
      }
      virtual ~GlobalAlignmentData()  {
        detail::destroyHandle( this->global );
      }
    };
  }   /* End namespace Aligments               */
}     /* End namespace dd4hep                  */

DD4HEP_INSTANTIATE_HANDLE_NAMED(GlobalAlignmentData);

namespace {

  static bool s_GlobalDetectorAlignment_debug = true;

  GlobalAlignment _align(const GlobalAlignment& a, TGeoHMatrix* transform, bool check, double overlap) {
    TGeoPhysicalNode* node = a.ptr();
    if ( node )  {
      TGeoMatrix* mm = node->GetNode()->GetMatrix();
      bool dbg = GlobalDetectorAlignment::debug();
      if ( dbg )  {
        printout( INFO, "Alignment", "DELTA matrix of %s", node->GetName() );
        transform->Print();
        printout( INFO, "Alignment", "OLD matrix of %s", node->GetName() );
        mm->Print();
      }
      std::vector<dd4hep::PlacedVolume> places;
      for( int i = 0; i < node->GetLevel(); ++i )
        places.emplace_back( node->GetNode(i+1) );

      transform->MultiplyLeft(mm); // orig * delta
      node->Align( transform, 0, check, overlap );
      if ( dbg )  {
        printout( INFO, "Alignment", "NEW matrix of %s", node->GetName() );
        node->GetNode()->GetMatrix()->Print();
      }
      TGeoVolume* oldm;
      dd4hep::PlacedVolume pv = node->GetNode(0);
      dd4hep::Volume       v  = pv->GetVolume();
      std::string          path = "/";


      
      dbg = false;
      // dbg = true;
      path += pv->GetName();
      for( int i = 0; i < node->GetLevel(); ++i )  {
        /// Attach user extension to placed volume
        pv = node->GetNode( i+1 );
        if ( nullptr == pv->GetUserExtension() )  {
          pv->SetUserExtension( places[i]->GetUserExtension() );
        }
        oldm = pv->GetMotherVolume();
        if( dbg )  {
          path += "/";
          path += pv->GetName();
          if( oldm != v.ptr() )  {
            printout(dd4hep::ALWAYS, "GlobalAlignment",
                     // "+++ Mother relationship broken after alignment: %s  place: %p volume: %p mother volume: old: %p new: %p",
                     "+++ Fix mother relationship after alignment: %s  place: %p volume: %p mother volume: old: %p new: %p",
                     path.c_str(), (void*)pv.ptr(), (void*)pv->GetVolume(), (void*)oldm, (void*)v.ptr() );
          }
        }
        pv->SetMotherVolume( v.ptr() );
        /// Fix node volumes: attach extensions etc.
        v = pv->GetVolume();
        /// Fix daughter mother relationship for the nodes in the branches
        for( int idau = 0; idau < v->GetNdaughters(); ++idau )  {
          TGeoNode* dau = v->GetNode( idau );
          if( v.ptr() != dau->GetMotherVolume() )  {
            dau->SetMotherVolume( v );
            if( dbg )  {
              oldm = dau->GetMotherVolume();
              if( oldm != v.ptr() )  {
                printout(dd4hep::ALWAYS, "GlobalAlignment",
                         // "+++ Mother relationship broken after alignment: %s/%s  place: %p volume: %p mother volume: old: %p --> new: %p",
                         "+++ Fix mother relationship after alignment: %s/%s  place: %p volume: %p "
                         "mother volume: old: %p --> new: %p",
                         path.c_str(), dau->GetName(), (void*)dau, (void*)dau->GetVolume(),
                         (void*)oldm, (void*)v.ptr());
              }
            }
          }
        }
        /// Attach user extension to volume
        if ( nullptr == v->GetUserExtension() )  {
          v->SetUserExtension( places[i].volume()->GetUserExtension() );
        }
      }
      return GlobalAlignment(node);
    }
    dd4hep::except("GlobalDetectorAlignment", "Cannot align non existing physical node. [Invalid Handle]");
    return { };
  }
  
  GlobalAlignment _alignment(const GlobalDetectorAlignment& det)  {
    dd4hep::DetElement::Object& e = det._data();
    if ( !e.global_alignment.isValid() )  {
      std::string path   = dd4hep::detail::tools::placementPath(det);
      e.global_alignment = dd4hep::Ref_t(new GlobalAlignmentData(path));
    }
    dd4hep::Handle<GlobalAlignmentData> h(e.global_alignment);
    if ( h.isValid() && h->global.isValid() )  {
      return h->global;
    }
    dd4hep::except("GlobalDetectorAlignment", "Cannot access global alignment data. [Invalid Handle]");
    return { };
  }

  void _dumpParentElements(GlobalDetectorAlignment& det, LevelElements& elements)   {
    int level = 0;
    dd4hep::detail::tools::PlacementPath nodes;
    dd4hep::detail::tools::ElementPath   det_nodes;
    dd4hep::detail::tools::placementPath(det, nodes);
    dd4hep::detail::tools::elementPath(det, det_nodes);
    ///    std::cout << "Placement path:";
    dd4hep::detail::tools::PlacementPath::const_reverse_iterator j=nodes.rbegin();
    dd4hep::detail::tools::ElementPath::const_reverse_iterator   k=det_nodes.rbegin();
    for(; j!=nodes.rend(); ++j, ++level)  {
      //cout << "(" << level << ") " << (void*)((*j).ptr())
      //           << " " << string((*j)->GetName()) << " ";
      if ( ::strcmp((*j).ptr()->GetName(), (*k).placement().ptr()->GetName()) )  {
        //cout << "[DE]";
        elements.emplace_back(level, *k);
        ++k;
      }
      else  {
        //elements.emplace_back(level,DetElement());
      }
      //std::cout << " ";
    }
    //std::cout << std::endl;
  }
}

/// Initializing constructor
GlobalDetectorAlignment::GlobalDetectorAlignment(DetElement e)
  : DetElement(std::move(e))
{
}

/// Initializing constructor
GlobalDetectorAlignment::GlobalDetectorAlignment(DetElement&& e)
  : DetElement(std::move(e))
{
}

/// Access debugging flag
bool GlobalDetectorAlignment::debug()   {
  return s_GlobalDetectorAlignment_debug;
}

/// Set debugging flag
bool GlobalDetectorAlignment::debug(bool value)   {
  bool tmp = s_GlobalDetectorAlignment_debug;
  s_GlobalDetectorAlignment_debug = value;
  return tmp;
}

/// Collect all placements from the detector element up to the world volume
void GlobalDetectorAlignment::collectNodes(std::vector<PlacedVolume>& nodes)   {
  detail::tools::placementPath(*this, nodes);
}

/// Access to the alignment block
GlobalAlignment GlobalDetectorAlignment::alignment() const   {
  return _alignment(*this);
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
std::vector<GlobalAlignment>& GlobalDetectorAlignment::volumeAlignments()  {
  Handle<GlobalAlignmentData> h(_data().global_alignment);
  return h->volume_alignments;
}

/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
const std::vector<GlobalAlignment>& GlobalDetectorAlignment::volumeAlignments() const   {
  Handle<GlobalAlignmentData> h(_data().global_alignment);
  return h->volume_alignments;
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
GlobalAlignment GlobalDetectorAlignment::align(const Position& pos, bool chk, double overlap) {
  return align(detail::matrix::_transform(pos), chk, overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
GlobalAlignment GlobalDetectorAlignment::align(const RotationZYX& rot, bool chk, double overlap) {
  return align(detail::matrix::_transform(rot), chk, overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
GlobalAlignment GlobalDetectorAlignment::align(const Position& pos, const RotationZYX& rot, bool chk, double overlap) {
  return align(detail::matrix::_transform(pos, rot), chk, overlap);
}

/// Align the physical node according to a generic Transform3D
GlobalAlignment GlobalDetectorAlignment::align(const Transform3D& transform, bool chk, double overlap)  {
  return align(detail::matrix::_transform(transform), chk, overlap);
}

/// Align the physical node according to a generic TGeo matrix
GlobalAlignment GlobalDetectorAlignment::align(TGeoHMatrix* matrix, bool chk, double overlap)  {
  return _align(_alignment(*this), matrix, chk, overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation only)
GlobalAlignment GlobalDetectorAlignment::align(const std::string& elt_path, const Position& pos, bool chk, double overlap) {
  return align(elt_path,detail::matrix::_transform(pos), chk, overlap);
}

/// Align the PhysicalNode of the placement of the detector element (rotation only)
GlobalAlignment GlobalDetectorAlignment::align(const std::string& elt_path, const RotationZYX& rot, bool chk, double overlap) {
  return align(elt_path,detail::matrix::_transform(rot), chk, overlap);
}

/// Align the PhysicalNode of the placement of the detector element (translation + rotation)
GlobalAlignment 
GlobalDetectorAlignment::align(const std::string& elt_path, const Position& pos, const RotationZYX& rot, bool chk, double overlap) {
  return align(elt_path,detail::matrix::_transform(pos, rot), chk, overlap);
}

/// Align the physical node according to a generic Transform3D
GlobalAlignment GlobalDetectorAlignment::align(const std::string& elt_path, const Transform3D& transform, bool chk, double overlap)  {
  return align(elt_path,detail::matrix::_transform(transform), chk, overlap);
}

/// Align the physical node according to a generic TGeo matrix
GlobalAlignment GlobalDetectorAlignment::align(const std::string& elt_path, TGeoHMatrix* matrix, bool chk, double overlap)  {
  if ( elt_path.empty() )
    return _align(_alignment(*this), matrix, chk, overlap);
  else if ( elt_path == placementPath() )
    return _align(_alignment(*this), matrix, chk, overlap);
  else if ( elt_path[0] == '/' )   {
    GlobalAlignment a(elt_path);
    volumeAlignments().emplace_back(a);
    return _align(a, matrix, chk, overlap);
  }
  GlobalAlignment a(placementPath() + '/' + elt_path);
  volumeAlignments().emplace_back(a);
  return _align(a, matrix, chk, overlap);
}
