// $Id: LCDDConverter.h 889 2013-11-14 15:55:39Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDALIGN_ALIGNMENTWRITER_H
#define DD4HEP_DDALIGN_ALIGNMENTWRITER_H

// Framework include files
#include "XML/XMLElements.h"
#include "DD4hep/Detector.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geomentry namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class AlignmentCache;

    /** @class AlignmentWriter AlignmentWriter.cpp AlignmentWriter.cpp
     *
     * Geometry converter from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class AlignmentWriter  {
    protected:
      /// Reference to detector description
      LCDD& m_lcdd;
      /// Reference to the alignment cache
      AlignmentCache* m_cache;

      /// Add single alignment node to the XML document
      void addNode(XML::Element elt, Alignment a)  const;

    public:
      /// Initializing Constructor
      AlignmentWriter(LCDD& lcdd);
      /// Standard destructor
      virtual ~AlignmentWriter();

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
#endif // DD4HEP_DDALIGN_ALIGNMENTWRITER_H
 
