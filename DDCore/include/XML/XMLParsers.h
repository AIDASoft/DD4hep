//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2017-03-15
//  \version  1.0
//
//==========================================================================
#ifndef DDCORE_XML_XMLPARSERS_H
#define DDCORE_XML_XMLPARSERS_H

// Framework include files
#include "DD4hep/AlignmentData.h"
#include "XML/XMLElements.h"

// C/C++ include files


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class OpaqueDataBlock;
  
  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML  {

    /// Set debug print level for this module. Default is OFF
    bool setXMLParserDebug(bool new_value);
    
    /// Convert rotation XML objects to DD4hep::Geometry::RotationZYX
    /**    <rotation x="0.5" y="0"  z="0"/>  => DD4hep::Geometry::RotationZYX
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse(Handle_t e, Geometry::RotationZYX& rot);

    /// Convert XML position objects to DD4hep::Geometry::Position
    /**
     *    <position x="0.5" y="0"  z="0"/>  => DD4hep::Geometry::Position
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse(Handle_t e, Geometry::Position& pos);

    /// Convert XML pivot objects to DD4hep::Geometry::Translation3D objects
    /**
     *    <pivot x="0.5" y="0"  z="0"/>
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse(Handle_t e, Geometry::Translation3D& tr);
    
    /// Convert alignment delta objects to Alignments::Delta
    /**
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
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse(Handle_t e, Alignments::Delta& delta);

    /// Parse delta into an opaque data block
    /**
     *  See void parse(Handle_t e, Alignments::Delta& delta) for details.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse_delta(Handle_t e, OpaqueDataBlock& block);

    /// Converts opaque maps to OpaqueDataBlock objects
    /**
     *  Opaque data mappings are formalized std::map objects (or std::hash_map etc.)
     *  where the data source is parsed into the map using boost::spirit as a data
     *  interpretation mechanism.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse_mapping(Handle_t e, OpaqueDataBlock& block);

    /// Converts linear STL containers from their string representation.
    /**
     *  Opaque data sequences are formalized linear STL container objects 
     *  (std::vector, std::list, std::set, std::deque etc.)
     *  where the data source is parsed into the map using boost::spirit as a data
     *  interpretation mechanism.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    void parse_sequence(Handle_t e, OpaqueDataBlock& block);

    
  }    /*   End namespace XML            */
}      /*   End namespace DD4hep         */
#endif /*   End DDCORE_XML_XMLPARSERS_H  */
