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
#ifndef DDALIGN_GLOBALALIGNMENTWRITER_H
#define DDALIGN_GLOBALALIGNMENTWRITER_H

// Framework include files
#include "XML/XMLElements.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/GlobalAlignment.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace align {

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
      Detector& m_detDesc;
      /// Reference to the alignment cache
      GlobalAlignmentCache* m_cache;

      /// Add single alignment node to the XML document
      void addNode(xml::Element elt, GlobalAlignment a)  const;

    public:
      /// Initializing Constructor
      GlobalAlignmentWriter(Detector& description);
      /// Standard destructor
      virtual ~GlobalAlignmentWriter();

      /// Dump one full DetElement subtree into a newly created document
      xml::Document dump(DetElement element, bool enable_transactions=false) const;
      /// Scan one DetElement structure and return an XML element containing the alignment in this subtree.
      xml::Element scan(xml::Document doc, DetElement element)  const;
      /// Create the element corresponding to one single detector element without children
      xml::Element createElement(xml::Document doc, DetElement element)  const;
      /// Write the XML document structure to a file.
      long write(xml::Document doc, const std::string& output)  const;
    };
  }    // End namespace xml
}      // End namespace dd4hep
#endif // DDALIGN_GLOBALALIGNMENTWRITER_H

