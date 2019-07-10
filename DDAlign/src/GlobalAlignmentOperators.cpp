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
#include "DD4hep/detail/DetectorInterna.h"
#include "DDAlign/GlobalAlignmentOperators.h"
#include "DDAlign/GlobalDetectorAlignment.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::align;

void GlobalAlignmentOperator::insert(GlobalAlignment alignment)  const   {
  if ( !cache.insert(alignment) )     {
    // Error
  }
}

void GlobalAlignmentSelector::operator()(Entries::value_type e)  const {
  TGeoPhysicalNode* pn = 0;
  nodes.emplace(e->path,make_pair(pn,e));
}

void GlobalAlignmentSelector::operator()(const Cache::value_type& entry)  const {
  TGeoPhysicalNode* pn = entry.second;
  for(Entries::const_iterator j=entries.begin(); j != entries.end(); ++j)   {
    Entries::value_type e = (*j);
    if ( GlobalAlignmentStack::needsReset(*e) || GlobalAlignmentStack::hasMatrix(*e) )  {
      const char* p = pn->GetName();
      bool reset_children = GlobalAlignmentStack::resetChildren(*e);
      if ( reset_children && ::strstr(p,e->path.c_str()) == p )   {
        nodes.emplace(p,make_pair(pn,e));
        break;
      }
      else if ( e->path == p )  {
        nodes.emplace(p,make_pair(pn,e));
        break;
      }
    }
  }
}

template <> void GlobalAlignmentActor<DDAlign_standard_operations::node_print>::init() {
  printout(ALWAYS,"GlobalAlignmentCache","++++++++++++++++++++++++ Summary ++++++++++++++++++++++++");
}

template <> void GlobalAlignmentActor<DDAlign_standard_operations::node_print>::operator()(Nodes::value_type& n)  const {
  TGeoPhysicalNode* p = n.second.first;
  const Entry& e = *n.second.second;
  printout(ALWAYS,"GlobalAlignmentCache","Need to reset entry:%s - %s [needsReset:%s, hasMatrix:%s]",
           p->GetName(), e.path.c_str(),
           yes_no(GlobalAlignmentStack::needsReset(e)),
           yes_no(GlobalAlignmentStack::hasMatrix(e)));
}

template <> void GlobalAlignmentActor<DDAlign_standard_operations::node_delete>::operator()(Nodes::value_type& n)  const {
  delete n.second.second;
  n.second.second = 0;
}

template <> void GlobalAlignmentActor<DDAlign_standard_operations::node_reset>::operator()(Nodes::value_type& n) const  {
  TGeoPhysicalNode* p = n.second.first;
  string np;
  if ( p->IsAligned() )   {
    for (Int_t i=0, nLvl=p->GetLevel(); i<=nLvl; i++) {
      TGeoNode* node = p->GetNode(i);
      TGeoMatrix* mm = node->GetMatrix();  // Node's relative matrix
      np += string("/")+node->GetName();
      if ( !mm->IsIdentity() && i > 0 )  {    // Ignore the 'world', is identity anyhow
        GlobalAlignment a = cache.get(np);
        if ( a.isValid() )  {
          printout(ALWAYS,"GlobalAlignmentActor<reset>","Correct path:%s leaf:%s",p->GetName(),np.c_str());
          TGeoHMatrix* glob = p->GetMatrix(i-1);
          if ( a.isValid() && i!=nLvl )   {
            *mm = *(a->GetOriginalMatrix());
          }
          else if ( i==nLvl ) {
            TGeoHMatrix* hm = dynamic_cast<TGeoHMatrix*>(mm);
            TGeoMatrix*  org = p->GetOriginalMatrix();
            if ( hm && org )  {
              hm->SetTranslation(org->GetTranslation());
              hm->SetRotation(org->GetRotationMatrix());
            }
            else  {
              printout(ALWAYS,"GlobalAlignmentActor<reset>",
                       "Invalid operation: %p %p", (void*)hm, (void*)org);
            }
          }
          *glob *= *mm;
        }
      }
    }
  }
}

template <> void GlobalAlignmentActor<DDAlign_standard_operations::node_align>::operator()(Nodes::value_type& n) const  {
  Entry&     e       = *n.second.second;
  bool       overlap = GlobalAlignmentStack::overlapDefined(e);
  DetElement det     = e.detector;

  if ( !det->global_alignment.isValid() && !GlobalAlignmentStack::hasMatrix(e) )  {
    printout(WARNING,"GlobalAlignmentActor","++++ SKIP Alignment %s DE:%s Valid:%s Matrix:%s",
             e.path.c_str(),det.placementPath().c_str(),
             yes_no(det->global_alignment.isValid()), yes_no(GlobalAlignmentStack::hasMatrix(e)));
    return;
  }
  if ( GlobalDetectorAlignment::debug() )  {
    printout(INFO,"GlobalAlignmentActor","++++ %s DE:%s Matrix:%s",
             e.path.c_str(),det.placementPath().c_str(),yes_no(GlobalAlignmentStack::hasMatrix(e)));
  }
  // Need to care about optional arguments 'check_overlaps' and 'overlap'
  GlobalDetectorAlignment ad(det);
  GlobalAlignment   align;
  Transform3D       trafo;
  const Delta&      delta  = e.delta;
  bool              no_vol = e.path == det.placementPath();
  double            ovl_precision = e.overlap;

  if ( delta.checkFlag(Delta::HAVE_ROTATION|Delta::HAVE_PIVOT|Delta::HAVE_TRANSLATION) )
    trafo = Transform3D(Translation3D(delta.translation)*delta.pivot*delta.rotation*(delta.pivot.Inverse()));
  else if ( delta.checkFlag(Delta::HAVE_ROTATION|Delta::HAVE_TRANSLATION) )
    trafo = Transform3D(delta.rotation,delta.translation);
  else if ( delta.checkFlag(Delta::HAVE_ROTATION|Delta::HAVE_PIVOT) )
    trafo = Transform3D(delta.pivot*delta.rotation*(delta.pivot.Inverse()));
  else if ( delta.checkFlag(Delta::HAVE_ROTATION) )
    trafo = Transform3D(delta.rotation);
  else if ( delta.checkFlag(Delta::HAVE_TRANSLATION) )
    trafo = Transform3D(delta.translation);

  if ( GlobalAlignmentStack::checkOverlap(e) && overlap )
    align = no_vol ? ad.align(trafo,ovl_precision,e.overlap) : ad.align(e.path,trafo,ovl_precision,e.overlap);
  else if ( GlobalAlignmentStack::checkOverlap(e) )
    align = no_vol ? ad.align(trafo,ovl_precision) : ad.align(e.path,trafo,ovl_precision);
  else
    align = no_vol ? ad.align(trafo) : ad.align(e.path,trafo);

  if ( align.isValid() )  {
    insert(align);
    return;
  }
  except("GlobalAlignmentActor","Failed to apply alignment for "+e.path);
}

#if 0
void alignment_reset_dbg(const string& path, const GlobalAlignment& a)   {
  TGeoPhysicalNode* n = a.ptr();
  cout << " +++++++++++++++++++++++++++++++ " << path << endl;
  cout << "      +++++ Misaligned physical node: " << endl;
  n->Print();
  string np;
  if ( n->IsAligned() ) {
    for (Int_t i=0; i<=n->GetLevel(); i++) {
      TGeoMatrix* mm = n->GetNode(i)->GetMatrix();
      np += "/";
      np += n->GetNode(i)->GetName();
      if ( mm->IsIdentity() ) continue;
      if ( i == 0 ) continue;

      TGeoHMatrix* glob = n->GetMatrix(i-1);
      NodeMap::const_iterator j=original_matrices.find(np);
      if ( j != original_matrices.end() && i!=n->GetLevel() )   {
        cout << "      +++++ Patch Level: " << i << np << endl;
        *mm = *((*j).second);
      }
      else  {
        if ( i==n->GetLevel() ) {
          cout << "      +++++ Level: " << i << np << " --- Original matrix: " << endl;
          n->GetOriginalMatrix()->Print();
          cout << "      +++++ Level: " << i << np << " --- Local matrix: " << endl;
          mm->Print();
          TGeoHMatrix* hm = dynamic_cast<TGeoHMatrix*>(mm);
          hm->SetTranslation(n->GetOriginalMatrix()->GetTranslation());
          hm->SetRotation(n->GetOriginalMatrix()->GetRotationMatrix());
          cout << "      +++++ Level: " << i << np << " --- New local matrix" << endl;
          mm->Print();
        }
        else          {
          cout << "      +++++ Level: " << i << np << " --- Keep matrix " << endl;
          mm->Print();
        }
      }
      cout << "      +++++ Level: " << i << np << " --- Global matrix: " << endl;
      glob->Print();
      *glob *= *mm;
      cout << "      +++++ Level: " << i << np << " --- New global matrix: " << endl;
      glob->Print();
    }
  }
  cout << "\n\n\n      +++++ physical node (full): " << np <<  endl;
  n->Print();
  cout << "      +++++ physical node (global): " << np <<  endl;
  n->GetMatrix()->Print();
}
#endif
