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
#ifndef DD4HEP_IDETSERVICE_H
#define DD4HEP_IDETSERVICE_H

// Framework includes
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsCleanup.h"


/// Gaudi namespace declaration
namespace gaudi   {

  /// Example gaudi-like service to access dd4hep conditions
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  class IDetService  {
  public:
    /// Short-cut to the ConditionsManager
    typedef dd4hep::cond::ConditionsManager           ConditionsManager;
    /// Other useful type definitions and type short-cuts
    typedef dd4hep::cond::ConditionUpdateUserContext  Context;
    typedef dd4hep::cond::ConditionDependency         Dependency;
    typedef dd4hep::cond::ConditionsCleanup           Cleanup;
    typedef dd4hep::DetElement                        DetElement;
    typedef dd4hep::Condition                         Condition;
    typedef dd4hep::IOV::Key_value_type               EventStamp;
    typedef dd4hep::IOVType                           IOVType;
    typedef std::shared_ptr<dd4hep::cond::ConditionsContent> Content;
    typedef std::shared_ptr<dd4hep::cond::ConditionsSlice>   Slice;
    
    /// Default destructor
    virtual ~IDetService() = default;

    /** General accessors to manipulate the conditions   */
    /// Accessor: get refernece to the conditions manager
    virtual ConditionsManager manager() const = 0;

    /// Accessor: Access IOV Type from conditions manager
    virtual const IOVType* iovType(const std::string& identifier)  const = 0;
    
    /** Content management interface routines            */
    /// Open content creation context
    virtual Content openContent(const std::string& name) = 0;

    /// Retrive existing and registered conditions content object by name
    virtual Content getContent(const std::string& name) = 0;

    /// Add a condition address to the content. Condition is identified by it's global key
    virtual void addContent(Content& content,
                            Condition::key_type key,
                            const std::string& address) = 0;

    /// Add a condition address to the content. Condition is identified by the detector and item name
    virtual void addContent(Content& content,
                            DetElement det,
                            const std::string& item,
                            const std::string& address) = 0;

    /// Add a condition functor for a derived condition to the content
    virtual void addContent(Content& content,
                            Dependency* call) = 0;

    /// Open content creation context
    virtual void closeContent(Content& content) = 0;

    /// Remove content object from cache.
    virtual bool removeContent(Content& content) = 0;

    /** Slice management interface routines              */
    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    virtual Slice project(Content &content,
                          Context *ctx,
                          const IOVType *typ,
                          EventStamp stamp) = 0;

    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    virtual Slice project(Content &content,
                          Context *ctx,
                          const std::string &typ,
                          EventStamp stamp) = 0;
    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    virtual Slice project(const std::string &content,
                          Context *ctx,
                          const std::string &typ,
                          EventStamp stamp) = 0;
    /// Invoke slice cleanup
    virtual void cleanup(const Cleanup& cleaner) = 0;
  };
}      // End namespace gaudi
#endif // DD4HEP_IDETSERVICE_H
