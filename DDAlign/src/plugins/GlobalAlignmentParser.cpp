//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Path.h"
#include "DD4hep/Mutex.h"
#include "DD4hep/Printout.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DDAlign/AlignmentTags.h"
#include "DDAlign/GlobalAlignmentStack.h"
#include "DDAlign/GlobalAlignmentCache.h"
#include "DDAlign/GlobalDetectorAlignment.h"

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
    class alignment_delta;
    class debug;
  }

  /// Forward declarations for all specialized converters
  template <> void Converter<debug>::operator()(xml_h seq)  const;
  template <> void Converter<pivot>::operator()(xml_h seq)  const;
  template <> void Converter<position>::operator()(xml_h seq)  const;
  template <> void Converter<rotation>::operator()(xml_h seq)  const;
  template <> void Converter<alignment_delta>::operator()(xml_h seq)  const;

  template <> void Converter<volume>::operator()(xml_h seq)  const;
  template <> void Converter<alignment>::operator()(xml_h seq)  const;
  template <> void Converter<detelement>::operator()(xml_h seq)  const;
  template <> void Converter<include_file>::operator()(xml_h seq)  const;
}

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;
using DD4hep::Geometry::Position;
using DD4hep::Geometry::Translation3D;

/** Convert to enable/disable debugging.
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<debug>::operator()(xml_h e) const {
  bool value = e.attr<bool>(_U(value));
  GlobalDetectorAlignment::debug(value);
}

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
  printout(INFO,"Alignment<rot>",
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
  printout(INFO,"Alignment<pos>","  Position:   x=%9.3f y=%9.3f   z=%9.3f",
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
  printout(INFO,"Alignment<piv>","     Pivot:      x=%9.3f y=%9.3f   z=%9.3f",x,y,z);
}

/** Convert delta objects
 *
 *     A generic alignment transformation is defined by
 *     - a translation in 3D space identified in XML as a
 *         <position/> element
 *       - a rotation in 3D space around a pivot point specified in XML by
 *         2 elements: the <rotation/> and the <pivot/> element.
 *       The specification of any of the elements is optional:
 *     - The absence of a translation implies the origine (0,0,0)
 *     - The absence of a pivot point implies the origine (0,0,0)
 *       - The absence of a rotation implies the identity rotation.
 *         Any supplied pivot point in this case is ignored.
 *
 *      <xx>
 *        <position x="0" y="0"  z="0.0001*mm"/>
 *        <rotation x="0" y="0"  z="0"/>
 *        <pivot    x="0" y="0"    z="100"/>
 *      </xx>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<alignment_delta>::operator()(xml_h e) const {
  Position pos;
  RotationZYX rot;
  Translation3D piv;
  xml_h  child_rot, child_pos, child_piv;
  Delta* delta = (Delta*)param;

  if ( (child_pos=e.child(_U(position),false)) )
    Converter<position>(lcdd,&delta->translation)(child_pos);
  if ( (child_rot=e.child(_U(rotation),false)) )   {
    Converter<rotation>(lcdd,&delta->rotation)(child_rot);
    if ( (child_piv=e.child(_U(pivot),false)) )
      Converter<pivot>(lcdd,&delta->pivot)(child_piv);
  }
  if ( child_rot && child_pos && child_piv )
    delta->flags |= Delta::HAVE_ROTATION|Delta::HAVE_PIVOT|Delta::HAVE_TRANSLATION;
  else if ( child_rot && child_pos )
    delta->flags |= Delta::HAVE_ROTATION|Delta::HAVE_TRANSLATION;
  else if ( child_rot && child_piv )
    delta->flags |= Delta::HAVE_ROTATION|Delta::HAVE_PIVOT;
  else if ( child_rot )
    delta->flags |= Delta::HAVE_ROTATION;
  else if ( child_pos )
    delta->flags |= Delta::HAVE_TRANSLATION;
}

typedef GlobalAlignmentStack::StackEntry StackEntry;

/** Convert volume objects
 *
 *      <volume subpath="layer4_0">
 *        <position x="0"   y="0"  z="0"/>
 *        <rotation x="0.5" y="0.1"  z="0.2"/>
 *        <pivot    x="0" y="0"    z="100"/>
 *      <volume>
 *         ...
 *      <volume>
 *      </volume>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<volume>::operator()(xml_h e) const {
  Delta val;
  pair<DetElement,string>* elt = (pair<DetElement,string>*)param;
  GlobalAlignmentStack* stack = _option<GlobalAlignmentStack>();
  string subpath    = e.attr<string>(_ALU(path));
  bool   reset      = e.hasAttr(_ALU(reset)) ? e.attr<bool>(_ALU(reset)) : true;
  bool   reset_dau  = e.hasAttr(_ALU(reset_children)) ? e.attr<bool>(_ALU(reset_children)) : true;
  bool   check      = e.hasAttr(_ALU(check_overlaps));
  bool   check_val  = check ? e.attr<bool>(_ALU(check_overlaps)) : false;
  bool   overlap    = e.hasAttr(_ALU(overlap));
  double ovl        = overlap ? e.attr<double>(_ALU(overlap)) : 0.001;
  string elt_place  = elt->first.placementPath();
  string placement  = subpath[0]=='/' ? subpath : elt_place + "/" + subpath;

  printout(INFO,"Alignment<vol>","    path:%s placement:%s reset:%s children:%s",
           subpath.c_str(), placement.c_str(), yes_no(reset), yes_no(reset_dau));

  Converter<alignment_delta>(lcdd,&val)(e);
  if ( val.flags ) val.flags |= GlobalAlignmentStack::MATRIX_DEFINED;
  if ( overlap   ) val.flags |= GlobalAlignmentStack::OVERLAP_DEFINED;
  if ( reset     ) val.flags |= GlobalAlignmentStack::RESET_VALUE;
  if ( reset_dau ) val.flags |= GlobalAlignmentStack::RESET_CHILDREN;
  if ( check     ) val.flags |= GlobalAlignmentStack::CHECKOVL_DEFINED;
  if ( check_val ) val.flags |= GlobalAlignmentStack::CHECKOVL_VALUE;

  dd4hep_ptr<StackEntry> entry(new StackEntry(elt->first,placement,val,ovl));
  stack->insert(entry);
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
 *        <pivot    x="0" y="0"    z="100"/>
 *    </detelement>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void Converter<detelement>::operator()(xml_h e) const {
  DetElement det(_param<DetElement::Object>());
  GlobalAlignmentStack* stack = _option<GlobalAlignmentStack>();
  string path      = e.attr<string>(_ALU(path));
  bool   check     = e.hasAttr(_ALU(check_overlaps));
  bool   check_val = check ? e.attr<bool>(_ALU(check_overlaps)) : false;
  bool   reset     = e.hasAttr(_ALU(reset)) ? e.attr<bool>(_ALU(reset)) : false;
  bool   reset_dau = e.hasAttr(_ALU(reset_children)) ? e.attr<bool>(_ALU(reset_children)) : false;
  bool   overlap   = e.hasAttr(_ALU(overlap));
  double ovl       = overlap ? e.attr<double>(_ALU(overlap)) : 0.001;
  DetElement elt   = Geometry::DetectorTools::findDaughterElement(det,path);
  string placement = elt.isValid() ? elt.placementPath() : string("-----");

  if ( !elt.isValid() )   {
    string err = "DD4hep: DetElement "+det.path()+" has no child:"+path+" [No such child]";
    throw runtime_error(err);
  }

  Delta delta;
  Converter<alignment_delta>(lcdd,&delta)(e);
  if ( delta.flags )  {
    delta.flags |= GlobalAlignmentStack::MATRIX_DEFINED;
    reset = reset_dau = true;
  }
  if ( overlap     ) delta.flags |= GlobalAlignmentStack::OVERLAP_DEFINED;
  if ( check       ) delta.flags |= GlobalAlignmentStack::CHECKOVL_DEFINED;
  if ( reset       ) delta.flags |= GlobalAlignmentStack::RESET_VALUE;
  if ( reset_dau   ) delta.flags |= GlobalAlignmentStack::RESET_CHILDREN;
  if ( check_val   ) delta.flags |= GlobalAlignmentStack::CHECKOVL_VALUE;

  printout(INFO,"Alignment<det>","path:%s [%s] placement:%s matrix:%s reset:%s children:%s",
           path.c_str(),
           elt.isValid() ? elt.path().c_str() : "-----",
           placement.c_str(),
           yes_no(delta.checkFlag(GlobalAlignmentStack::MATRIX_DEFINED)),
           yes_no(reset), yes_no(reset_dau));

  dd4hep_ptr<StackEntry> entry(new StackEntry(elt,placement,delta,ovl));
  stack->insert(entry);

  pair<DetElement,string> vol_param(elt,"");
  xml_coll_t(e,_U(volume)).for_each(Converter<volume>(lcdd,&vol_param,optional));
  xml_coll_t(e,_ALU(detelement)).for_each(Converter<detelement>(lcdd,elt.ptr(),optional));
  xml_coll_t(e,_U(include)).for_each(Converter<include_file>(lcdd,elt.ptr(),optional));
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
    Converter<alignment>(lcdd,param,optional)(node);
  else if ( tag == "detelement" )
    Converter<detelement>(lcdd,param,optional)(node);
  else if ( tag == "subdetectors" || tag == "detelements" )
    xml_coll_t(node,_ALU(detelements)).for_each(Converter<detelement>(lcdd,param,optional));
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
  /// Now we process all allowed elements within the alignment tag:
  /// <detelement/>, <detelements/>, <subdetectors/> and <include/>
  xml_coll_t(e,_ALU(debug)).for_each(Converter<debug>(lcdd,param,optional));
  xml_coll_t(e,_ALU(detelement)).for_each(Converter<detelement>(lcdd,param,optional));
  xml_coll_t(e,_ALU(detelements)).for_each(_ALU(detelement),Converter<detelement>(lcdd,param,optional));
  xml_coll_t(e,_ALU(subdetectors)).for_each(_ALU(detelement),Converter<detelement>(lcdd,param,optional));
  xml_coll_t(e,_U(include)).for_each(Converter<include_file>(lcdd,param,optional));
}

/** Basic entry point to read alignment files
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long setup_Alignment(lcdd_t& lcdd, const xml_h& e) {
  static dd4hep_mutex_t s_mutex;
  dd4hep_lock_t lock(s_mutex);
  bool open_trans  = e.hasChild(_ALU(open_transaction));
  bool close_trans = e.hasChild(_ALU(close_transaction));

  GlobalAlignmentCache* cache = GlobalAlignmentCache::install(lcdd);
  /// Check if transaction already present. If not, open, else issue an error
  if ( open_trans )   {
    if ( GlobalAlignmentStack::exists() )  {
      except("GlobalAlignment","Request to open a second alignment transaction stack -- not allowed!");
    }
    GlobalAlignmentStack::create();
  }
  if ( !GlobalAlignmentStack::exists() )  {
    printout(ERROR,"GlobalAlignment","Request process global alignments without cache.");
    printout(ERROR,"GlobalAlignment","Call plugin DD4hep_GlobalAlignmentInstall first OR add XML tag <open_transaction/>");
    except("GlobalAlignment","Request process global alignments without cache.");
  }
  GlobalAlignmentStack& stack = GlobalAlignmentStack::get();
  (DD4hep::Converter<DD4hep::alignment>(lcdd,lcdd.world().ptr(),&stack))(e);
  if ( close_trans )  {
    cache->commit(stack);
    GlobalAlignmentStack::get().release();
  }
  if ( GlobalDetectorAlignment::debug() )  {
    xml_elt_t elt(e);
    Path uri = elt.document().uri();
    printout(INFO,"GlobalAlignment","+++ Successfully parsed XML: %s",
             uri.filename().c_str());
  }
  return 1;
}
DECLARE_XML_DOC_READER(global_alignment,setup_Alignment)

/** Basic entry point to install the alignment cache in a LCDD instance
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long install_Alignment(lcdd_t& lcdd, int, char**) {
  GlobalAlignmentCache::install(lcdd);
  return 1;
}
DECLARE_APPLY(DD4hep_GlobalAlignmentInstall,install_Alignment)
