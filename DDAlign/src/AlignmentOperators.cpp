// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
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
#include "DDAlign/AlignmentOperators.h"
#include "DDAlign/DetectorAlignment.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

void AlignmentOperator::insert(Alignment alignment)  const   {
  if ( !cache.insert(alignment) )     {
    // Error
  }
}

void AlignmentSelector::operator()(Entries::value_type e)  const {
  TGeoPhysicalNode* pn = 0;
  nodes.insert(make_pair(e->path,make_pair(pn,e)));
}

void AlignmentSelector::operator()(const Cache::value_type& e)  const {
  TGeoPhysicalNode* pn = e.second;
  for(Entries::const_iterator j=entries.begin(); j != entries.end(); ++j)   {
    Entries::value_type e = (*j);
    if ( e->needsReset() || e->hasMatrix() )  {
      const char* p = pn->GetName();
      bool reset_children = e->resetChildren();
      if ( reset_children && ::strstr(p,e->path.c_str()) == p )   {
	nodes.insert(make_pair(p,make_pair(pn,e)));
	break;
      }
      else if ( e->path == p )  {
	nodes.insert(make_pair(p,make_pair(pn,e)));
	break;
      }
    }
  } 
}

template <> void AlignmentActor<DDAlign_standard_operations::node_print>::init() {
  printout(ALWAYS,"AlignmentCache","++++++++++++++++++++++++ Summary ++++++++++++++++++++++++");
}

template <> void AlignmentActor<DDAlign_standard_operations::node_print>::operator()(Nodes::value_type& n)  const {
  TGeoPhysicalNode* p = n.second.first;
  Entry* e = n.second.second;
  printout(ALWAYS,"AlignmentCache","Need to reset entry:%s - %s [needsReset:%s, hasMatrix:%s]",
	   p->GetName(),e->path.c_str(),yes_no(e->needsReset()),yes_no(e->hasMatrix()));
}

template <> void AlignmentActor<DDAlign_standard_operations::node_delete>::operator()(Nodes::value_type& n)  const {
  delete n.second.second;
  n.second.second = 0;
}

template <> void AlignmentActor<DDAlign_standard_operations::node_reset>::operator()(Nodes::value_type& n) const  {
  TGeoPhysicalNode* p = n.second.first;
  //Entry*            e = n.second.second;
  string np;
  if ( p->IsAligned() )   {
    for (Int_t i=0, nLvl=p->GetLevel(); i<=nLvl; i++) {
      TGeoNode* node = p->GetNode(i);
      TGeoMatrix* mm = node->GetMatrix();  // Node's relative matrix
      np += string("/")+node->GetName();
      if ( !mm->IsIdentity() && i > 0 )  {    // Ignore the 'world', is identity anyhow
	Alignment a = cache.get(np);
	if ( a.isValid() )  {
	  printout(ALWAYS,"AlignmentActor<reset>","Correct path:%s leaf:%s",p->GetName(),np.c_str());
	  TGeoHMatrix* glob = p->GetMatrix(i-1);
	  if ( a.isValid() && i!=nLvl )   {
	    *mm = *(a->GetOriginalMatrix());
	  }
	  else if ( i==nLvl ) {
	    TGeoHMatrix* hm = dynamic_cast<TGeoHMatrix*>(mm);
	    TGeoMatrix*  org = p->GetOriginalMatrix();
	    hm->SetTranslation(org->GetTranslation());
	    hm->SetRotation(org->GetRotationMatrix());
	  }
	  *glob *= *mm;
	}
      }
    }
  }
}

template <> void AlignmentActor<DDAlign_standard_operations::node_align>::operator()(Nodes::value_type& n) const  {
  Entry& e = *n.second.second;
  bool check = e.checkOverflow();
  bool overlap = e.checkOverlap();
  bool has_matrix  = e.hasMatrix();
  DetElement det = e.detector;
  bool valid = det.alignment().isValid();
  string det_placement = det.placementPath();

  if ( !valid && !has_matrix )  {
    cout << "++++ SKIP ALIGNMENT: ++++ " << e.path 
	 << " DE:" << det_placement 
	 << " Valid:" << yes_no(valid) 
	 << " Matrix:" << yes_no(has_matrix) << endl;
    /*    */
    return;
  }

  cout << "++++ " << e.path 
       << " DE:" << det_placement 
       << " Valid:" << yes_no(valid) 
       << " Matrix:" << yes_no(has_matrix)
       << endl;
  /*  */
  // Need to care about optional arguments 'check_overlaps' and 'overlap'
  DetectorAlignment ad(det);
  Alignment alignment;
  bool is_not_volume = e.path == det_placement;
  if ( check && overlap )     {
    alignment = is_not_volume 
      ? ad.align(e.transform, e.overflowValue(), e.overlap)
      : ad.align(e.path, e.transform, e.overflowValue(), e.overlap);
  }
  else if ( check )    {
    alignment = is_not_volume 
      ? ad.align(e.transform, e.overflowValue())
      : ad.align(e.path, e.transform, e.overflowValue());
  }
  else     {
    alignment = is_not_volume ? ad.align(e.transform) : ad.align(e.path, e.transform);
   }
  if ( alignment.isValid() )  {
    insert(alignment);
    return;
  }
  throw runtime_error("Failed to apply alignment for "+e.path);
}

#if 0
    void alignment_reset_dbg(const string& path, const Alignment& a)   {      
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
	    else	      {
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
