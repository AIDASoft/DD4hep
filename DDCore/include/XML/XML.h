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

#ifndef XML_XML_H
#define XML_XML_H

#include "XML/XMLTags.h"
#include "XML/XMLDimension.h"
#include "XML/XMLDetector.h"
#include "XML/XMLChildValue.h"
#include "XML/DocumentHandler.h"

// Useful shortcuts to process XML structures.
typedef dd4hep::xml::Strng_t         Unicode;
typedef dd4hep::xml::Handle_t        xml_h;
typedef dd4hep::xml::Tag_t           xml_tag_t;
typedef dd4hep::xml::Attribute       xml_attr_t;
typedef dd4hep::xml::Collection_t    xml_coll_t;
typedef dd4hep::xml::Element         xml_elt_t;
typedef dd4hep::xml::RefElement      xml_ref_t;
typedef dd4hep::xml::Dimension       xml_dim_t;
typedef dd4hep::xml::DetElement      xml_det_t;
typedef dd4hep::xml::Component       xml_comp_t;
typedef dd4hep::xml::ChildValue      xml_val_t;
typedef dd4hep::xml::Document        xml_doc_t;
typedef dd4hep::xml::DocumentHolder  xml_doc_holder_t;
typedef dd4hep::xml::DocumentHandler xml_handler_t;

#endif // XML_XML_H
