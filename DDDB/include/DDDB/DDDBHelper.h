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
#include "DD4hep/Conditions.h"

#include "XML/DocumentHandler.h"
#include "XML/Utilities.h"
#include "XML/UriReader.h"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// Forward declarations
    class dddb;

    /// Class supporting the interface of the LHCb conditions database to dd4hep
    /**
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class DDDBHelper   {
    public:
      typedef std::vector<std::pair<std::string, VisAttr> > VisAttrs;
      typedef std::map<std::string,std::pair<DetElement,std::string> > Det_Conditions;

    public:
      /// Standard constructor
      DDDBHelper(Detector& description);
      /// Default destructor
      virtual ~DDDBHelper();

      /// Access XML reader object
      xml::UriReader*  xmlReader() const       {  return m_xmlReader; }
      /// Set XML reader object
      void setXmlReader(xml::UriReader* rdr)   {  m_xmlReader = rdr;  }
      /// Access local database representation
      dddb* detectorDescription() const        {  return m_detDesc;   }
      /// Set XML reader object
      void setDetectorDescription(dddb* geo);
      /// Access visualization attribute for a given volume by path
      VisAttr visAttr(const std::string& path)  const;
      /// Add visualization attribute
      void addVisAttr(const std::string& path, const std::string attr_name);
      /// Add visualization attribute
      void addVisAttr(const std::string& path, VisAttr attr);
      /// Add new conditions entry
      bool addConditionEntry(const std::string& val, DetElement det, const std::string& item);
      /// Access conditions entry
      std::pair<DetElement,std::string> getConditionEntry(const std::string& item)  const;
      
    public:
      /// Reference to main detector description instance
      Detector&       m_description;
      /// Reference to XML entity resolver
      xml::UriReader* m_xmlReader;
      /// Reference to extracted detector description information
      dddb*           m_detDesc;
      /// Optional container of visualization attributes
      VisAttrs        m_visAttrs;
      /// The inventory mapping of conditions items to Detector elements.
      Det_Conditions  m_detCond;
    };

  }    /* End namespace DDDB        */
}      /* End namespace dd4hep      */
#endif /* DD4HEP_DDDB_DDDBHELPER_H  */
