// $Id$
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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DDDBHELPER_H
#define DD4HEP_DDDB_DDDBHELPER_H

// Framework includes
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/FieldTypes.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Objects.h"

#include "XML/DocumentHandler.h"
#include "XML/Utilities.h"
#include "XML/UriReader.h"


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// Forward declarations
    class dddb;

    /// Class supporting the interface of the LHCb conditions database to DD4hep
    /**
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class DDDBHelper   {
    public:
      typedef std::vector<std::pair<std::string, Geometry::VisAttr> > VisAttrs;

    public:
      /// Standard constructor
      DDDBHelper(Geometry::LCDD& lcdd);
      /// Default destructor
      virtual ~DDDBHelper();

      /// Access XML reader object
      XML::UriReader*  xmlReader() const       {  return m_xmlReader; }
      /// Set XML reader object
      void setXmlReader(XML::UriReader* rdr)   {  m_xmlReader = rdr;  }
      /// Access local database representation
      dddb* detectorDescription() const        {  return m_detDesc;   }
      /// Set XML reader object
      void setDetectorDescription(dddb* geo);
      /// Access visualization attribute for a given volume by path
      Geometry::VisAttr visAttr(const std::string& path)  const;
      /// Add visualization attribute
      void addVisAttr(const std::string& path, const std::string attr_name);
      /// Add visualization attribute
      void addVisAttr(const std::string& path, Geometry::VisAttr attr);

    public:
      /// Reference to main detector description instance
      Geometry::LCDD& m_lcdd;
      /// Reference to XML entity resolver
      XML::UriReader* m_xmlReader;
      /// Reference to extracted detector description information
      dddb*           m_detDesc;
      /// Optional container of visualization attributes
      VisAttrs        m_visAttrs;
    };

  }    /* End namespace DDDB        */
}      /* End namespace DD4hep      */
#endif /* DD4HEP_DDDB_DDDBHELPER_H  */
