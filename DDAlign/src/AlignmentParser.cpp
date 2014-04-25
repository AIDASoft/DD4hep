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
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DDAlign/AlignmentTags.h"
#include "DDAlign/AlignmentStack.h"
#include "DDAlign/AlignmentCache.h"
#include "DDAlign/AlignmentTransaction.h"

// C/C++ include files
#include <stdexcept>

namespace DD4hep  {

  namespace   {

    /// Some utility class to specialize the convetrers:
    class alignment;
    class detelement;
    class include_file;
    class volume;
    class rotation;
    class position;
    class pivot;
    class transform3d;
  }

  /// Forward declarations for all specialized converters
  template <> void Converter<pivot>::operator()(xml_h seq)  const;
  template <> void Converter<position>::operator()(xml_h seq)  const;
  template <> void Converter<rotation>::operator()(xml_h seq)  const;
  template <> void Converter<transform3d>::operator()(xml_h seq)  const;

  template <> void Converter<volume>::operator()(xml_h seq)  const;
  template <> void Converter<alignment>::operator()(xml_h seq)  const;
  template <> void Converter<detelement>::operator()(xml_h seq)  const;
  template <> void Converter<include_file>::operator()(xml_h seq)  const;
}  

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/** Convert rotation objects
 *
 *    <rotation x="0.5" y="0"  z="0"/>     
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<rotation>::operator()(xml_h e) const {
  xml_comp_t r(e);
  RotationZYX* v = (RotationZYX*)param;
  v->SetComponents(r.z(), r.y(), r.x());
  printout(INFO,"Alignment<rotation>",
	   "  Rotation:   x=%9.3f y=%9.3f   z=%9.3f  phi=%7.4f psi=%7.4f theta=%7.4f",
	   r.x(), r.y(), r.z(), v->Phi(), v->Psi(), v->Theta());
}

/** Convert position objects
 *
 *    <position x="0.5" y="0"  z="0"/>     
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<position>::operator()(xml_h e) const {
  xml_comp_t p(e);
  Position* v = (Position*)param;
  v->SetXYZ(p.x(), p.y(), p.z());
  printout(INFO,"Alignment<position>","  Position:   x=%9.3f y=%9.3f   z=%9.3f",
	   v->X(), v->Y(), v->Z());
}

/** Convert pivot objects
 *
 *    <pivot x="0.5" y="0"  z="0"/>     
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<pivot>::operator()(xml_h e) const {
  xml_comp_t p(e);
  double x,y,z;
  Translation3D* v = (Translation3D*)param;
  v->SetXYZ(x=p.x(), y=p.y(), z=p.z());
  printout(INFO,"Alignment<pivot>","     Pivot:      x=%9.3f y=%9.3f   z=%9.3f",x,y,z);
}

/** Convert transform3d objects
 *
 *     A generic alignment transformation is defined by
 *     - a translation in 3D space identified in XML as a
 *	   <position/> element
 *	 - a rotation in 3D space around a pivot point specified in XML by
 *	   2 elements: the <rotation/> and the <pivot/> element.
 *	 The specification of any of the elements is optional:
 *     - The absence of a translation implies the origine (0,0,0)
 *     - The absence of a pivot point implies the origine (0,0,0)
 *	 - The absence of a rotation implies the identity rotation.
 *	   Any supplied pivot point in this case is ignored.
 *
 *	<xx>
 *	  <position x="0" y="0"  z="0.0001*mm"/>
 *	  <rotation x="0" y="0"  z="0"/>     
 *	  <pivot    x="0" y="0"    z="100"/>     
 *	</xx>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<transform3d>::operator()(xml_h e) const {
  typedef pair<bool,Transform3D> Data;
  Position pos;
  RotationZYX rot;
  Translation3D piv;
  xml_h child_rot, child_pos, child_piv;
  Data* tr = (Data*)param;
    
  if ( (child_pos=e.child(_U(position),false)) )
    Converter<position>(lcdd,&pos)(child_pos);
  if ( (child_rot=e.child(_U(rotation),false)) )   {
    Converter<rotation>(lcdd,&rot)(child_rot);
    if ( (child_piv=e.child(_U(pivot),false)) )
      Converter<pivot>(lcdd,&piv)(child_piv);
  }
  tr->first = true;
  if ( child_rot && child_pos && child_piv )
    tr->second = Transform3D(Translation3D(pos)*piv*rot*(piv.Inverse()));
  else if ( child_rot && child_pos )
    tr->second = Transform3D(rot,pos);
  else if ( child_rot && child_piv )
    tr->second = Transform3D(piv*rot*(piv.Inverse()));
  else if ( child_rot )
    tr->second = Transform3D(rot);
  else if ( child_pos )
    tr->second = Transform3D(pos);
  else   {
    tr->first  = false;
  }
}

typedef AlignmentStack::StackEntry StackEntry;

/** Convert volume objects
 *
 *	<volume subpath="layer4_0">
 *	  <position x="0"   y="0"  z="0"/>
 *	  <rotation x="0.5" y="0.1"  z="0.2"/>     
 *	  <pivot    x="0" y="0"    z="100"/>
 *      <volume>
 *         ...
 *      <volume>
 *	</volume>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<volume>::operator()(xml_h e) const {
  pair<bool,Transform3D> trafo;
  pair<DetElement,string>* elt = (pair<DetElement,string>*)param;
  string subpath   = e.attr<string>(_U(path));
  bool   reset     = e.hasAttr(_U(reset)) ? e.attr<bool>(_U(reset)) : true;
  bool   reset_dau = e.hasAttr(_U(reset_children)) ? e.attr<bool>(_U(reset_children)) : true;
  bool   check     = e.hasAttr(_U(check_overlaps));
  bool   check_val = check ? e.attr<bool>(_U(check_overlaps)) : false;
  bool   overlap   = e.hasAttr(_U(overlap));
  double ovl       = overlap ? e.attr<double>(_U(overlap)) : 0.001;
  string eltPlacement  = elt->first.placementPath();
  string placementPath = subpath[0]=='/' ? subpath : eltPlacement + "/" + subpath;

  printout(INFO,"Alignment<volume>","    path:%s placement:%s reset:%s children:%s",
	   subpath.c_str(), placementPath.c_str(), yes_no(reset), yes_no(reset_dau));

  Converter<transform3d>(lcdd,&trafo)(e);
  int flags = 0;
  if ( overlap     ) flags |= AlignmentStack::OVERLAP_DEFINED;
  if ( trafo.first ) flags |= AlignmentStack::MATRIX_DEFINED;
  if ( reset       ) flags |= AlignmentStack::RESET_VALUE;
  if ( reset_dau   ) flags |= AlignmentStack::RESET_CHILDREN;
  if ( check       ) flags |= AlignmentStack::CHECKOVL_DEFINED;
  if ( check_val   ) flags |= AlignmentStack::CHECKOVL_VALUE;

  auto_ptr<StackEntry> entry(new StackEntry(elt->first,placementPath,trafo.second,ovl,flags));
  AlignmentStack::insert(entry);
  pair<DetElement,string> vol_param(elt->first,subpath);
  xml_coll_t(e,_U(volume)).for_each(Converter<volume>(lcdd,&vol_param));
}

/** Convert detelement objects
 *
 *    Function entry expects as a parameter a valid DetElement handle
 *    pointing to the subdetector, which detector elements should be 
 *    realigned.
 *
 *    <detelement path="/world/TPC/TPC_SideA/TPC_SideA_sector02">
 *      <position x="0"   y="0"  z="0"/>
 *      <rotation x="0.5" y="0"  z="0"/>     
 *	  <pivot    x="0" y="0"    z="100"/>     
 *    </detelement>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<detelement>::operator()(xml_h e) const {
  DetElement det   = param ? *(DetElement*)param : DetElement();
  string path      = e.attr<string>(_U(path));
  bool   check     = e.hasAttr(_U(check_overlaps));
  bool   check_val = check ? e.attr<bool>(_U(check_overlaps)) : false;
  bool   reset     = e.hasAttr(_U(reset)) ? e.attr<bool>(_U(reset)) : false;
  bool   reset_dau = e.hasAttr(_U(reset_children)) ? e.attr<bool>(_U(reset_children)) : false;
  bool   overlap   = e.hasAttr(_U(overlap));
  double ovl       = overlap ? e.attr<double>(_U(overlap)) : 0.001;
  DetElement elt   = Geometry::DetectorTools::findDaughterElement(det,path);
  string placementPath = elt.isValid() ? elt.placementPath() : string("-----");

  if ( !elt.isValid() )   {
    string err = "DD4hep: DetElement "+det.path()+" has no child:"+path+" [No such child]";
    throw runtime_error(err);
  }

  pair<bool,Transform3D> trafo;
  Converter<transform3d>(lcdd,&trafo)(e);
  int flags = 0;
  if ( trafo.first )  {
    flags |= AlignmentStack::MATRIX_DEFINED;
    reset = reset_dau = true;
  }
  if ( overlap     ) flags |= AlignmentStack::OVERLAP_DEFINED;
  if ( check       ) flags |= AlignmentStack::CHECKOVL_DEFINED;
  if ( reset       ) flags |= AlignmentStack::RESET_VALUE;
  if ( reset_dau   ) flags |= AlignmentStack::RESET_CHILDREN;
  if ( check_val   ) flags |= AlignmentStack::CHECKOVL_VALUE;

  printout(INFO,"Alignment<detelement>","path:%s [%s] placement:%s matrix:%s reset:%s children:%s",
	   path.c_str(), 
	   elt.isValid() ? elt.path().c_str() : "-----",
	   placementPath.c_str(),
	   yes_no(trafo.first), yes_no(reset), yes_no(reset_dau));

  auto_ptr<StackEntry> entry(new StackEntry(elt,placementPath,trafo.second,ovl,flags));
  AlignmentStack::insert(entry);

  pair<DetElement,string> vol_param(elt,"");
  xml_coll_t(e,_U(volume)).for_each(Converter<volume>(lcdd,&vol_param));
  xml_coll_t(e,_U(detelement)).for_each(Converter<detelement>(lcdd,&elt));
  xml_coll_t(e,_U(include)).for_each(Converter<include_file>(lcdd,&elt));
}

/** Convert detelement_include objects
 *
 *    <detelement path="/world/TPC/TPC_SideA/TPC_SideA_sector02">
 *      <position x="0"   y="0"  z="0"/>
 *      <rotation x="0.5" y="0"  z="0"/>     
 *    </detelement>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<include_file>::operator()(xml_h element) const {
  XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h node = doc.root();
  string tag = node.tag();
  if ( tag == "alignment" )  
    Converter<alignment>(lcdd,param)(node);
  else if ( tag == "detelement" )
    Converter<detelement>(lcdd,param)(node);
  else if ( tag == "subdetectors" || tag == "detelements" )
    xml_coll_t(node,_U(detelements)).for_each(Converter<detelement>(lcdd,param));
  else
    throw runtime_error("Undefined tag name in XML structure:"+tag+" XML parsing abandoned.");
}

/** Convert alignment objects
 *
 *    <alignment>
 *      <detelement path="TPC">
 *        ...
 *      </detelement>
 *    </alignment>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<alignment>::operator()(xml_h e)  const  {
  DetElement top = param ? *(DetElement*)param : lcdd.world();

  /// Now we process all allowed elements within the alignment tag:
  /// <detelement/>, <detelements/>, <subdetectors/> and <include/>
  xml_coll_t(e,_U(detelement)).for_each(Converter<detelement>(lcdd,&top));
  xml_coll_t(e,_U(detelements)).for_each(_U(detelement),Converter<detelement>(lcdd,&top));
  xml_coll_t(e,_U(subdetectors)).for_each(_U(detelement),Converter<detelement>(lcdd,&top));
  xml_coll_t(e,_U(include)).for_each(Converter<include_file>(lcdd,&top));
}

/** Basic entry point to read alignment files
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long setup_Alignment(lcdd_t& lcdd, const xml_h& e) {
  AlignmentCache::install(lcdd);
  AlignmentTransaction tr(lcdd, e);
  (DD4hep::Converter<DD4hep::alignment>(lcdd))(e);
  return 1;
}
DECLARE_XML_DOC_READER(alignment,setup_Alignment)

/** Basic entry point to install the alignment cache in a LCDD instance
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long install_Alignment(lcdd_t& lcdd, int, char**) {
  AlignmentCache::install(lcdd);
  return 1;
}
DECLARE_APPLY(DD4hepAlignmentInstall,install_Alignment)
