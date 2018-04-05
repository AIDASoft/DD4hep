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
#ifndef DD4HEP_DETSERVICE_H
#define DD4HEP_DETSERVICE_H

// Framework includes
#include "Detector/IDetService.h"
#include "DD4hep/ComponentProperties.h"

// C/C++ includes
#include <mutex>

/// Gaudi namespace declaration
namespace gaudi  {

  /// Example gaudi-like service to access dd4hep conditions
  /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
  class DetService : virtual public IDetService,
                    public dd4hep::PropertyConfigurable
  {
    /// Cache entry definition
    class CacheEntry
    {
    public:
      Slice slice;
      dd4hep::IOV iov{0};
      int age = 0;
      CacheEntry() = default;
      CacheEntry(CacheEntry &&) = delete;
      CacheEntry(const CacheEntry &) = delete;
      CacheEntry &operator=(const CacheEntry &) = delete;
    };

    /// Short-cut to the ConditionsManager
    typedef dd4hep::cond::ConditionsManager ConditionsManager;
    /// Short-cut to the cache slices
    typedef std::vector<CacheEntry *> CachedSlices;
    /// Short-cut to the content cache
    typedef std::pair<std::string, Content> ContentEntry;
    /// Cached content container
    typedef std::vector<ContentEntry> CachedContents;

    /// Main object lock for content manipulation
    std::mutex m_contentLock;
    /// Main object lock for slice manipulation
    std::mutex m_sliceLock;
    /// Reference to the conditions manager object
    ConditionsManager m_manager;
    /// Container of active content objects
    CachedContents m_activeContents;
    /// Container of content objects being edited
    CachedContents m_openContents;
    /// The slice cache of re-usable slices
    CachedSlices m_cache;
    /// The maximum age limit
    int m_ageLimit;

  protected:
    /// Age all slices and check for slices which have expired.
    void _age();
    /// Find a conditions slice in the cache
    Slice _find(Content &content, const IOVType *typ, EventStamp stamp);
    /// Project a newly created conditions slice.
    Slice _create(Content &content, Context *ctx, const IOVType *typ, EventStamp stamp);
    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    Slice _project(Content &content, Context *ctx, const IOVType *typ, EventStamp stamp);
    /// Remove content from cache
    bool _remove(CachedContents &cache, Content &content);
    /// Add a condition address to the content. Condition is identified by it's global key
    bool _addContent(Content &content, Condition::key_type key, const std::string &address);

  public:
    /// Default constructor
    DetService() = delete;
    /// Initializing constructor
    DetService(ConditionsManager m);
    /// Initializing constructor
    DetService(const DetService &copy) = delete;
    /// Default destructor
    virtual ~DetService() = default;
    /// Assignment operator
    DetService &operator=(const DetService &copy) = delete;

    /// Initialize the service
    virtual int initialize();
    /// Finalize the service
    virtual int finalize();

    /** General accessors to manipulate the conditions   */
    /// Accessor: get refernece to the conditions manager
    virtual ConditionsManager manager() const override;

    /// Accessor: Access IOV Type from conditions manager
    virtual const IOVType *iovType(const std::string &identifier) const override;

    /// Open content creation context
    virtual Content openContent(const std::string &name) override;

    /// Open content creation context and copy items from registered object
    virtual Content getContent(const std::string &name) override;

    /// Add a condition address to the content. Condition is identified by it's global key
    virtual void addContent(Content &content,
                            Condition::key_type key,
                            const std::string &address) override;

    /// Add a condition address to the content
    virtual void addContent(Content &content,
                            DetElement det,
                            const std::string &item,
                            const std::string &address) override;

    /// Add a condition functor for a derived condition to the content
    virtual void addContent(Content &content, Dependency *call) override;

    /// Close content creation context
    virtual void closeContent(Content &content) override;

    /// Remove content object from cache.
    virtual bool removeContent(Content &content) override;

    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    virtual Slice project(Content &content,
                          Context *ctx,
                          const IOVType *typ,
                          EventStamp stamp) override;
    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    virtual Slice project(Content &content,
                          Context *ctx,
                          const std::string &typ,
                          EventStamp stamp) override;
    /// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
    virtual Slice project(const std::string &content,
                          Context *ctx,
                          const std::string &typ,
                          EventStamp stamp) override;
    /// Invoke slice cleanup
    virtual void cleanup(const Cleanup &cleaner)  override;
  };
}
#endif // DD4HEP_DETSERVICE_H
