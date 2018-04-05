//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-02-02
//  \version 1.0
//
//==========================================================================
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

#ifndef DD4HEP_DDDB_DDDBCONDITONSLOADER_H
#define DD4HEP_DDDB_DDDBCONDITONSLOADER_H

// Framework include files
#include "DDCond/ConditionsDataLoader.h"
#include "DD4hep/ConditionsListener.h"
#include "DD4hep/Printout.h"
#include "XML/UriReader.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace DDDB  {

    /// Implementation of a stack of conditions assembled before application
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class DDDBConditionsLoader : public cond::ConditionsDataLoader  {
      typedef std::pair<std::string, std::string> Key;
      typedef std::map<Condition::key_type, Key>  KeyMap;
      /** 
       *  \author   M.Frank
       *  \version  1.0
       *  \ingroup  DD4HEP_CONDITIONS
       */
      class KeyCollector : public cond::ConditionsListener  {
      public:
        std::pair<cond::ConditionsListener*,void*> call;
        KeyMap keys;
        /// Initializing constructor
        KeyCollector();
        /// ConditionsListener overload: onRegister new condition
        virtual void onRegisterCondition(Condition cond, void* param);
      };
      xml::UriReader* m_resolver;
      KeyCollector    m_keys;

      /// Load single conditions document
      void loadDocument(xml::UriContextReader& rdr, const Key& k);
      /// Load single conditions document
      void loadDocument(xml::UriContextReader& rdr, 
                        const std::string& sys_id,
                        const std::string& obj_id);

    public:
      /// Default constructor
      DDDBConditionsLoader(Detector& description, cond::ConditionsManager mgr, const std::string& nam);
      /// Default destructor
      virtual ~DDDBConditionsLoader();
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_single(key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range( key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Optimized update using conditions slice data
      virtual size_t load_many(  const IOV& req_validity,
                                 RequiredItems&  work,
                                 LoadedItems&    loaded,
                                 IOV&       conditions_validity);
    };
  } /* End namespace DDDB                    */
} /* End namespace dd4hep                    */

#endif /* DD4HEP_DDDB_DDDBCONDITONSLOADER_H  */
