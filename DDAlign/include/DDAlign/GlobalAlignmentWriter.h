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
#ifndef DD4HEP_DDALIGN_GLOBALALIGNMENTWRITER_H
#define DD4HEP_DDALIGN_GLOBALALIGNMENTWRITER_H

// Framework include files
#include "XML/XMLElements.h"
#include "DD4hep/Detector.h"
#include "DD4hep/GlobalAlignment.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {

    // Forward declarations
    class GlobalAlignmentCache;

    /// Write aligment data to XML data file
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class GlobalAlignmentWriter  {
    protected:
      /// Reference to detector description
      LCDD& m_lcdd;
      /// Reference to the alignment cache
      GlobalAlignmentCache* m_cache;

      /// Add single alignment node to the XML document
      void addNode(XML::Element elt, GlobalAlignment a)  const;

    public:
      /// Initializing Constructor
      GlobalAlignmentWriter(LCDD& lcdd);
      /// Standard destructor
      virtual ~GlobalAlignmentWriter();

      /// Dump one full DetElement subtree into a newly created document
      XML::Document dump(DetElement element, bool enable_transactions=false) const;
      /// Scan one DetElement structure and return an XML element containing the alignment in this subtree.
      XML::Element scan(XML::Document doc, DetElement element)  const;
      /// Create the element corresponding to one single detector element without children
      XML::Element createElement(XML::Document doc, DetElement element)  const;
      /// Write the XML document structure to a file.
      long write(XML::Document doc, const std::string& output)  const;
    };
  }    // End namespace XML
}      // End namespace DD4hep
#endif // DD4HEP_DDALIGN_GLOBALALIGNMENTWRITER_H

