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
#ifndef DDCOND_CONDITIONSTREEPERSISTENCY_H
#define DDCOND_CONDITIONSTREEPERSISTENCY_H

// Framework/ROOT include files
#include "DDCond/ConditionsPool.h"

#include "TNamed.h"
class TFile;

// C/C++ include files
#include <map>
#include <list>
#include <vector>
#include <memory>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /*
     *   event (iov)
     *       |
     *       |
     *       |           Link attribute:
     *       |           (priority,tag-id)
     *       V            0..*                       valid for mutiple conditions
     *   global tag ---------------->  condition tag (iov,version,pool-id)
     *   (named,iov)                       | 1       [cached, # of consecutive iov]
     *   [in memory]                       |
     *                                     |
     *                                     |
     *                                     V 0..1
     *                               conditions pool (pool-id)
     *                                     |         [DDCond cache/management]
     *                                     |
     *                                     |
     *                                     |
     *                                     V 0..*
     *                                 condition
     *                                                              
     *
     */
    
    /** Class representing a conditions tag
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsTag  {
    public:
      /// Tag name
      std::string name;
      /// Interval of validity
      IOV         iov{0};
      /// Version number of the condition
      int         version = -1;
      /// The collections contributing to this tag
      int         collectionID = -1;
      /// Default destructor
      ~ConditionsTag() = default;
      /// Default constructor
      ConditionsTag() = default;
      /// Copy contructor
      ConditionsTag(const ConditionsTag& copy) =  default;
      /// Assignment operator
      ConditionsTag& operator=(const ConditionsTag& copy) =  default;
    };

    /** Class representing a global tag
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class GlobalTag  {
    public:
      /// Name of the global tag
      std::string       name;
      /// The interval of validity for this global tag
      IOV               iov{0};

      /// This vector contains all the individual tags contributing to this global tag
      /** Tuple: (priority,tag-id)   ->   tree collection identifier          */
      std::map<std::pair<int,int>, int>  tags;


      /// Default detructor
      ~GlobalTag()   = default;
      /// Default constructor
      GlobalTag()    = default;
      /// Copy contructor
      GlobalTag(const GlobalTag& copy) =  default;
      /// Assignment operator
      GlobalTag& operator=(const GlobalTag& copy) =  default;
    };

    /// Forward declarations
    class ConditionsSlice;
    class ConditionsIOVPool;

    /// Helper to save conditions pools to ROOT
    /** 
     *  This is a rathr simplistic persistency mechanism for conditions.
     *  It is not suited to actually store and retrieve individual conditions
     *  by key. Though it is rather efficient to store and retrive 
     *  entire conditions snapshots of the conditions store implemented by DDCond.
     *  It e.g. allows to fast load the conditions store for HLT applications,
     *  of which we know it takes ages to populate it from a database.
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsTreePersistency : public TNamed  {
    public:
      typedef std::vector<Condition>                                  pool_type;
      typedef std::pair<std::string, pool_type>                       named_pool_type;
      typedef std::pair<std::string,std::pair<std::pair<std::string,int>,IOV::Key> > iov_key_type;
      typedef std::list<std::pair<iov_key_type, pool_type> >          persistent_type;

      persistent_type conditionPools {};
      persistent_type iovPools  {};
      float           duration  {0};
      enum ImportStrategy  {
        IMPORT_ALL             = 1<<0,
        IMPORT_EXACT           = 1<<1,
        IMPORT_CONTAINED       = 1<<2,
        IMPORT_CONTAINED_LOWER = 1<<3,
        IMPORT_CONTAINED_UPPER = 1<<4,
        IMPORT_EDGE_LOWER      = 1<<5,
        IMPORT_EDGE_UPPER      = 1<<6,
        LAST
      };
      /// Load ConditionsIOVPool and populate conditions manager
      size_t _import(ImportStrategy     strategy,
                     persistent_type&   pers,
                     const std::string& id,
                     const std::string& iov_type,
                     const IOV::Key&    iov_key,
                     ConditionsManager  mgr);

      /// Clear object content and release allocated memory
      void _clear(persistent_type& pool);
      
    public:
      /// No copy constructor
      ConditionsTreePersistency(const ConditionsTreePersistency& copy) = delete;
      /// Initializing constructor
      ConditionsTreePersistency(const std::string& name, const std::string& title="DD4hep conditions container");
      /// Default constructor
      ConditionsTreePersistency();
      /// Default destructor
      virtual ~ConditionsTreePersistency();
      /// No assignment
      ConditionsTreePersistency& operator=(const ConditionsTreePersistency& copy) = delete;

      /// Clear object content and release allocated memory
      void clear();
      /// Open ROOT file in read mode
      static TFile* openFile(const std::string& fname);      
      
      /// Add conditions content to be saved. Note, that dependent conditions shall not be saved!
      size_t add(const std::string& tag, const IOV& iov, std::vector<Condition>& conditions);
      /// Add conditions content to be saved. Note, that dependent conditions shall not be saved!
      size_t add(const std::string& tag, ConditionsPool& pool);
      /// Add conditions content to be saved. Note, that dependent conditions shall not be saved!
      size_t add(const std::string& tag, const ConditionsIOVPool& pool);

      
      /// Load conditions content from file.
      static std::unique_ptr<ConditionsTreePersistency> load(TFile* file,const std::string& object);
      
      /// Load conditions content from file.
      static std::unique_ptr<ConditionsTreePersistency> load(const std::string& file,const std::string& object)  {
        return load(openFile(file), object);
      }
      
      /// Load conditions IOV pool and populate conditions manager
      size_t importIOVPool(const std::string& id, const std::string& iov_type, ConditionsManager mgr);
      /// Load conditions pool and populate conditions manager
      size_t importConditionsPool(const std::string& id, const std::string& iov_type, ConditionsManager mgr);
      /// Load conditions pool and populate conditions manager. Allow tro be selective also for the key
      size_t importConditionsPool(ImportStrategy     strategy,
                                  const std::string& id,
                                  const std::string& iov_type,
                                  const IOV::Key&    key,
                                  ConditionsManager  mgr);

      /// Save the data content to a root file
      int save(TFile* file);
      /// Save the data content to a root file
      int save(const std::string& file_name);

      /// ROOT object ClassDef
      ClassDef(ConditionsTreePersistency,1);
    };
    
  }        /* End namespace cond                            */
}          /* End namespace dd4hep                          */
#endif // DDCOND_CONDITIONSTREEPERSISTENCY_H

