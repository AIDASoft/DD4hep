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
#ifndef DD4HEP_ALIGNMENT_GLOBALALIGNMENTCACHE_H
#define DD4HEP_ALIGNMENT_GLOBALALIGNMENTCACHE_H

// Framework include files
#include "DD4hep/GlobalAlignment.h"
#include "DDAlign/GlobalAlignmentStack.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace align {

    /// Forward declarations
    class GlobalAlignmentOperator;
    class GlobalAlignmentCache;
    class GlobalAlignmentStack;

    /// Class caching all known alignment operations for one Detector instance.
    /**
     *  Internaly the instances are fragmented to subdetectors defined
     *  by the next-to-top level detector elements.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class GlobalAlignmentCache  {
      friend class dd4hep::Detector;
      friend class GlobalAlignmentOperator;

    public:
      typedef GlobalAlignmentStack                        Stack;
      typedef Stack::StackEntry                           Entry;
      typedef std::map<unsigned int, TGeoPhysicalNode*>   Cache;
      typedef std::map<std::string,GlobalAlignmentCache*> SubdetectorAlignments;

    protected:
      Detector&       m_detDesc;
      /// Cache of subdetectors
      SubdetectorAlignments m_detectors;
      /// The subdetector specific map of alignments caches
      Cache       m_cache;
      /// 
      /// Branchg name: If it is not the main tree instance, the name of the subdetector
      std::string m_sdPath;
      /// The length of the branch name to optimize lookups....
      size_t      m_sdPathLen;
      /// Reference count
      int         m_refCount;
      /// Flag to indicate the top instance
      bool        m_top;

    protected:
      /// Default constructor initializing variables
      GlobalAlignmentCache(Detector& description, const std::string& sdPath, bool top);
      /// Default destructor
      virtual ~GlobalAlignmentCache();

      /// Retrieve branch cache by name. If not present it will be created
      GlobalAlignmentCache* subdetectorAlignments(const std::string& name);

      /// Population entry: Apply a complete stack of ordered alignments to the geometry structure
      void apply(GlobalAlignmentStack& stack);
      /// Apply a vector of SD entries of ordered alignments to the geometry structure
      void apply(const std::vector<Entry*> &changes);
      /// Add a new entry to the cache. The key is the placement path
      bool insert(GlobalAlignment alignment);

    public:
      /// Create and install a new instance tree
      static GlobalAlignmentCache* install(Detector& description);
      /// Unregister and delete a tree instance
      static void uninstall(Detector& description);
      /// Add reference count
      int addRef();
      /// Release object. If reference count goes to NULL, automatic deletion is triggered.
      int release();
      /// Access the section name
      const std::string& name() const   {   return m_sdPath;  }
      /// Close existing transaction stack and apply all alignments
      void commit(GlobalAlignmentStack& stack);
      /// Retrieve the cache section corresponding to the path of an entry.
      GlobalAlignmentCache* section(const std::string& path_name) const;
      /// Retrieve an alignment entry by its placement path
      GlobalAlignment get(const std::string& path) const;
      /// Return all entries matching a given path. Careful: Expensive operaton!
      std::vector<GlobalAlignment> matches(const std::string& path_match, bool exclude_exact=false) const;
    };

  } /* End namespace align                            */
} /* End namespace dd4hep                                  */
#endif    /* DD4HEP_ALIGNMENT_GLOBALALIGNMENTCACHE_H       */
