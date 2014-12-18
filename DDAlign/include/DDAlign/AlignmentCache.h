// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTCACHE_H
#define DD4HEP_ALIGNMENT_ALIGNMENTCACHE_H

// Framework include files
#include "DD4hep/Alignment.h"
#include "DDAlign/AlignmentStack.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Forward declarations
    class AlignmentOperator;
    class AlignmentCache;
    class AlignmentStack;
    class LCDD;

    /// Class caching all known alignment operations for one LCDD instance.
    /**
     *  Internally the instances are fragmented to subdetectors defined
     *  by the next-to-top level detector elements.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentCache  {
      friend class LCDD;
      friend class AlignmentOperator;

    public:
      typedef AlignmentStack::StackEntry Entry;
      typedef std::map<unsigned int, TGeoPhysicalNode*> Cache;
      typedef std::map<std::string,AlignmentCache*> SubdetectorAlignments;

    protected:
      LCDD&       m_lcdd;
      /// Cache of subdetectors
      SubdetectorAlignments m_detectors;
      /// The subdetector specific map of alignments caches
      Cache       m_cache;
      /// Branchg name: If it is not the main tree instance, the name of the subdetector
      std::string m_sdPath;
      /// The length of the branch name to optimize lookups....
      size_t      m_sdPathLen;
      /// Reference count
      int         m_refCount;
      /// Flag to indicate the top instance
      bool        m_top;

    protected:
      /// Default constructor
      AlignmentCache(LCDD& lcdd, const std::string& sdPath, bool top);
      /// Default destructor
      virtual ~AlignmentCache();
      /// Retrieve branch cache by name. If not present it will be created
      AlignmentCache* subdetectorAlignments(const std::string& name);

      /// Population entry: Apply a complete stack of ordered alignments to the geometry structure
      void apply(AlignmentStack& stack);
      /// Apply a vector of SD entries of ordered alignments to the geometry structure
      void apply(const std::vector<Entry*> &changes);
      /// Add a new entry to the cache. The key is the placement path
      bool insert(Alignment alignment);

    public:
      /// Create and install a new instance tree
      static void install(LCDD& lcdd);
      /// Unregister and delete a tree instance
      static void uninstall(LCDD& lcdd);

      /// Add reference count
      int addRef();
      /// Release object. If reference count goes to NULL, automatic deletion is triggered.
      int release();
      /// Access the section name
      const std::string& name() const   {   return m_sdPath;  }
      /// Open a new transaction stack (Note: only one stack allowed!)
      void openTransaction();
      /// Close existing transaction stack and apply all alignments
      void closeTransaction();
      /// Retrieve the cache section corresponding to the path of an entry.
      AlignmentCache* section(const std::string& path_name) const;
      /// Retrieve an alignment entry by its lacement path
      Alignment get(const std::string& path) const;
      /// Return all entries matching a given path. Careful: Expensive operaton!
      std::vector<Alignment> matches(const std::string& path_match, bool exclude_exact=false) const;
    };

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_ALIGNMENT_ALIGNMENTCACHE_H       */
