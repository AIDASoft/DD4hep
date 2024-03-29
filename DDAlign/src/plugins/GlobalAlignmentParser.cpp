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
#include <DD4hep/Detector.h>
#include <DD4hep/Path.h>
#include <DD4hep/Mutex.h>
#include <DD4hep/Printout.h>
#include <XML/Conversions.h>
#include <XML/XMLParsers.h>
#include <XML/DocumentHandler.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/DetFactoryHelper.h>

#include <DDAlign/AlignmentTags.h>
#include <DDAlign/GlobalAlignmentStack.h>
#include <DDAlign/GlobalAlignmentCache.h>
#include <DDAlign/GlobalDetectorAlignment.h>

// C/C++ include files
#include <stdexcept>

namespace dd4hep  {

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
  template <> void Converter<volume>::operator()(xml_h seq)  const;
  template <> void Converter<alignment>::operator()(xml_h seq)  const;
  template <> void Converter<detelement>::operator()(xml_h seq)  const;
  template <> void Converter<include_file>::operator()(xml_h seq)  const;

  static long setup_Alignment(Detector& description, const xml_h& e);
}

using namespace dd4hep::align;
using StackEntry = GlobalAlignmentStack::StackEntry;


/** Convert to enable/disable debugging.
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void dd4hep::Converter<dd4hep::debug>::operator()(xml_h e) const {
  bool value = e.attr<bool>(_U(value));
  GlobalDetectorAlignment::debug(value);
}

/** Convert volume objects
 *
 *      <volume subpath="layer4_0">
 *        <position x="0*mm" y="0*mm" z="0*mm"/>
 *        <rotation x="0.5"  y="0.1"  z="0.2"/>
 *        <pivot    x="0*mm" y="0*mm" z="100*mm"/>
 *      <volume>
 *         ...
 *      <volume>
 *      </volume>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void dd4hep::Converter<dd4hep::volume>::operator()(xml_h e) const {
  Delta val;
  GlobalAlignmentStack* stack  = _option<GlobalAlignmentStack>();
  std::pair<DetElement,std::string>* elt = _param<std::pair<DetElement,std::string> >();
  std::string subpath = e.attr<std::string>(_ALU(path));
  bool   reset      = e.hasAttr(_ALU(reset)) ? e.attr<bool>(_ALU(reset)) : true;
  bool   reset_dau  = e.hasAttr(_ALU(reset_children)) ? e.attr<bool>(_ALU(reset_children)) : true;
  bool   check      = e.hasAttr(_ALU(check_overlaps));
  bool   check_val  = check ? e.attr<bool>(_ALU(check_overlaps)) : false;
  bool   overlap    = e.hasAttr(_ALU(overlap));
  double ovl        = overlap ? e.attr<double>(_ALU(overlap)) : 0.001;
  std::string elt_place  = elt->first.placementPath();
  std::string placement  = subpath[0]=='/' ? subpath : elt_place + "/" + subpath;

  printout(INFO,"Alignment<vol>","    path:%s placement:%s reset:%s children:%s",
           subpath.c_str(), placement.c_str(), yes_no(reset), yes_no(reset_dau));

  xml::parse(e,val);
  if ( val.flags ) val.flags |= GlobalAlignmentStack::MATRIX_DEFINED;
  if ( overlap   ) val.flags |= GlobalAlignmentStack::OVERLAP_DEFINED;
  if ( reset     ) val.flags |= GlobalAlignmentStack::RESET_VALUE;
  if ( reset_dau ) val.flags |= GlobalAlignmentStack::RESET_CHILDREN;
  if ( check     ) val.flags |= GlobalAlignmentStack::CHECKOVL_DEFINED;
  if ( check_val ) val.flags |= GlobalAlignmentStack::CHECKOVL_VALUE;

  dd4hep_ptr<StackEntry> entry(new StackEntry(elt->first,placement,val,ovl));
  stack->insert(entry);
  std::pair<DetElement,std::string> vol_param(elt->first,subpath);
  xml_coll_t(e,_U(volume)).for_each(Converter<volume>(description,&vol_param));
}

/** Convert detelement objects
 *
 *    Function entry expects as a parameter a valid DetElement handle
 *    pointing to the subdetector, which detector elements should be
 *    realigned.
 *
 *    <detelement path="/world/TPC/TPC_SideA/TPC_SideA_sector02">
 *      <position x="0*mm" y="0*mm" z="0*mm"/>
 *      <rotation x="0.5"  y="0.1"  z="0.2"/>
 *      <pivot    x="0*mm" y="0*mm" z="100*mm"/>
 *    </detelement>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void dd4hep::Converter<dd4hep::detelement>::operator()(xml_h e) const {
  DetElement det(_param<DetElement::Object>());
  GlobalAlignmentStack* stack = _option<GlobalAlignmentStack>();
  std::string path = e.attr<std::string>(_ALU(path));
  bool   check     = e.hasAttr(_ALU(check_overlaps));
  bool   check_val = check ? e.attr<bool>(_ALU(check_overlaps)) : false;
  bool   reset     = e.hasAttr(_ALU(reset)) ? e.attr<bool>(_ALU(reset)) : false;
  bool   reset_dau = e.hasAttr(_ALU(reset_children)) ? e.attr<bool>(_ALU(reset_children)) : false;
  bool   overlap   = e.hasAttr(_ALU(overlap));
  double ovl       = overlap ? e.attr<double>(_ALU(overlap)) : 0.001;
  DetElement elt   = detail::tools::findDaughterElement(det,path);
  std::string placement = elt.isValid() ? elt.placementPath() : std::string("-----");

  if ( !elt.isValid() )   {
    except("GlocalAlignmentParser",
           "dd4hep: DetElement %s has no child: %s [No such child]", det.path().c_str(), path.c_str());
  }

  Delta delta;
  xml::parse(e, delta);
  if ( delta.flags )  {
    delta.flags |= GlobalAlignmentStack::MATRIX_DEFINED;
    reset = reset_dau = true;
  }
  if ( overlap     ) delta.flags |= GlobalAlignmentStack::OVERLAP_DEFINED;
  if ( check       ) delta.flags |= GlobalAlignmentStack::CHECKOVL_DEFINED;
  if ( reset       ) delta.flags |= GlobalAlignmentStack::RESET_VALUE;
  if ( reset_dau   ) delta.flags |= GlobalAlignmentStack::RESET_CHILDREN;
  if ( check_val   ) delta.flags |= GlobalAlignmentStack::CHECKOVL_VALUE;

  printout(INFO,
           "Alignment<det>","path:%s [%s] placement:%s matrix:%s reset:%s children:%s",
           path.c_str(),
           elt.isValid() ? elt.path().c_str() : "-----",
           placement.c_str(),
           yes_no(delta.checkFlag(GlobalAlignmentStack::MATRIX_DEFINED)),
           yes_no(reset), yes_no(reset_dau));

  dd4hep_ptr<StackEntry> entry(new StackEntry(elt,placement,delta,ovl));
  stack->insert(entry);

  std::pair<DetElement, std::string> vol_param(elt,"");
  xml_coll_t(e,_U(volume)).for_each(Converter<volume>(description,&vol_param,optional));
  xml_coll_t(e,_ALU(detelement)).for_each(Converter<detelement>(description,elt.ptr(),optional));
  xml_coll_t(e,_U(include)).for_each(Converter<include_file>(description,elt.ptr(),optional));
}

/** Convert detelement_include objects
 *
 *    <detelement path="/world/TPC/TPC_SideA/TPC_SideA_sector02">
 *      <position x="0*mm" y="0*mm" z="0*mm"/>
 *      <rotation x="0.5"  y="0.1"  z="0.2"/>
 *    </detelement>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
template <> void dd4hep::Converter<dd4hep::include_file>::operator()(xml_h element) const {
  xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h node = doc.root();
  std::string tag = node.tag();
  if ( tag == "alignment" )
    Converter<alignment>(description,param,optional)(node);
  else if ( tag == "global_alignment" )
    setup_Alignment(description, node);
  else if ( tag == "detelement" )
    Converter<detelement>(description,param,optional)(node);
  else if ( tag == "subdetectors" || tag == "detelements" )
    xml_coll_t(node,_ALU(detelements)).for_each(Converter<detelement>(description,param,optional));
  else
    except("GlocalAlignmentParser", "Undefined tag name in XML structure: %s XML parsing abandoned.", tag.c_str());
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
template <> void dd4hep::Converter<dd4hep::alignment>::operator()(xml_h e)  const  {
  /// Now we process all allowed elements within the alignment tag:
  /// <detelement/>, <detelements/>, <subdetectors/> and <include/>
  xml_coll_t(e,_ALU(debug)).for_each(Converter<debug>(description,param,optional));
  xml_coll_t(e,_ALU(detelement)).for_each(Converter<detelement>(description,param,optional));
  xml_coll_t(e,_ALU(detelements)).for_each(_ALU(detelement),Converter<detelement>(description,param,optional));
  xml_coll_t(e,_ALU(subdetectors)).for_each(_ALU(detelement),Converter<detelement>(description,param,optional));
  xml_coll_t(e,_U(include)).for_each(Converter<include_file>(description,param,optional));
}

/** Basic entry point to read alignment files
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
long dd4hep::setup_Alignment(dd4hep::Detector& description, const xml_h& e)  {
  static dd4hep::dd4hep_mutex_t s_mutex;
  dd4hep::dd4hep_lock_t lock(s_mutex);
  bool open_trans  = e.hasChild(_ALU(open_transaction));
  bool close_trans = e.hasChild(_ALU(close_transaction));

  GlobalAlignmentCache* cache = GlobalAlignmentCache::install(description);
  /// Check if transaction already present. If not, open, else issue an error
  if ( open_trans )   {
    if ( GlobalAlignmentStack::exists() )  {
      except("GlobalAlignment","Request to open a second alignment transaction stack -- not allowed!");
    }
    GlobalAlignmentStack::create();
  }
  if ( !GlobalAlignmentStack::exists() )  {
    printout(ERROR,"GlobalAlignment",
             "Request process global alignments without cache.");
    printout(ERROR,"GlobalAlignment",
             "Call plugin DD4hep_GlobalAlignmentInstall first OR add XML tag <open_transaction/>");
    except("GlobalAlignment","Request process global alignments without cache.");
  }
  GlobalAlignmentStack& stack = GlobalAlignmentStack::get();
  (dd4hep::Converter<dd4hep::alignment>(description,description.world().ptr(),&stack))(e);
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

/** Basic entry point to install the alignment cache in a Detector instance
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/04/2014
 */
static long install_Alignment(dd4hep::Detector& description, int, char**) {
  GlobalAlignmentCache::install(description);
  return 1;
}
DECLARE_APPLY(DD4hep_GlobalAlignmentInstall,install_Alignment)
