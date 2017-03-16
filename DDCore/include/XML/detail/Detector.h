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
#ifndef DD4HEP_XML_DETAIL_DETECTOR_H
#define DD4HEP_XML_DETAIL_DETECTOR_H
    
/** 
 *  Note: Do NEVER include this file directly!
 *
 *  Include XML/XMLDetector.h or JSON/Detector.h !
 */

// Framework includes
#include "XML/detail/Dimension.h"

/// XML Handle class extending the XML handle Dimension
/**
 *  Helper class to access any field in a xml tag.
 *  Specialized for attributes of a detector sub-element.
 *
 *  \author   M.Frank
 *  \version  1.0
 *  \ingroup DD4HEP_XML
 */
struct Component: public Dimension {
  /// Constructor from Handle
  Component(Handle_t e)
    : Dimension(e) {
  }
  /// Constructor from Element
  Component(const Element& e)
    : Dimension(e) {
  }
  /// Access material attribute as STL string
  std::string materialStr() const;
  /// Check if component is sensitive
  bool isSensitive() const;
      
  /// Check if component is a radiator (absorber)
  bool isRadiator() const;
      
  /// Create a shape using the plugin mechanism from an XML element
  NamedObject* createShape() const;
};

/// XML Handle class extending the XML handle Dimension
/**
 *  Helper class to access any field in a xml tag.
 *  Specialized for some of the attributes of a detector element.
 *
 *  \author   M.Frank
 *  \version  1.0
 *  \ingroup DD4HEP_XML
 */
struct DetElement: public Dimension {
  /// Constructor from Handle
  DetElement(Handle_t e)
    : Dimension(e) {
  }
  /// Access underlying XML handle object
  Handle_t handle() const {
    return m_element;
  }

  /// Access parameters: id
  int id() const;
  /// Access material attribute as STL string. If not present empty return empty string
  std::string materialStr() const;
  /// Check if xml detector element is sensitive
  bool isSensitive() const;
  /// Check if element describes a tracking detector
  bool isTracker() const;
  /// Check if element describes a calorimetric detector
  bool isCalorimeter() const;
  /// Check if element is inside the tracking volume
  bool isInsideTrackingVolume() const;
  /// Helper to check a condition (for debugging)
  void check(bool condition, const std::string& msg) const;
  /// Access attribute by type
  template <typename T> T attr(const XmlChar* tag_value) const {
    return m_element.attr < T > (tag_value);
  }
};

#endif    /* DD4HEP_XML_DETAIL_DETECTOR_H    */
