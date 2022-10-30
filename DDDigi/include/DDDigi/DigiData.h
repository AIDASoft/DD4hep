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
#ifndef DDDIGI_DIGIDATA_H
#define DDDIGI_DIGIDATA_H

/// Framework include files
#include <DD4hep/Objects.h>

/// C/C++ include files
#include <cstdint>
#include <memory>
#include <mutex>
#include <map>
#include <any>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    union Key;
    class Particle;
    class EnergyDeposit;
    class ParticleMapping;
    class DepositMapping;
    class DigiEvent;
    class DataSegment;

    using Position = dd4hep::Position;
    using Direction = dd4hep::Direction;

    ///  Key defintion to access the event data
    /**
     *  Helper to convert item and mask to a 64 bit integer
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    union Key   {
      typedef std::uint64_t key_type;
      typedef std::uint32_t itemkey_type;
      typedef std::uint16_t mask_type;
      typedef std::uint8_t  segment_type;
      typedef std::uint8_t  submask_type;

    private:      
      /// First union entry used for fast initialization
      key_type key;
      /// Second union entry to use for discrimination
      struct {
	// Ordering is important here: 
	// We want to group the containers by item ie. by container name
	// and not by mask
	segment_type segment;
	submask_type submask;
        mask_type    mask;
        itemkey_type item;
      } values;
    public:
      /// Default constructor
      Key();
      /// Move constructor
      Key(Key&&);
      /// Copy constructor
      Key(const Key&);
      /// Initializaing constructor (fast)
      Key(key_type full_mask) = delete;
      /// Initializing constructor with key generation using hash algorithm
      explicit Key(const char* item, mask_type mask);
      /// Initializing constructor with key generation using hash algorithm
      explicit Key(const std::string& item, mask_type mask);
      /// Assignment operator
      Key& operator = (const Key::key_type) = delete;
      /// Assignment operator
      Key& operator = (const Key&);
      /// Move assignment operator
      Key& operator = (Key&& copy);
      /// Equality operator
      bool operator == (const Key&)   const;
      /// Operator less
      bool operator < (const Key&)   const;
      /// Operator greator
      bool operator > (const Key&)   const;

      /// Generate key using hash algorithm
      void set(const std::string& name, int mask);

      /// Set key mask
      Key& set_mask(mask_type m)  {
	this->values.mask = m;
	return *this;
      }
      /// Set key submask
      Key& set_submask(submask_type m)  {
	this->values.submask = m;
	return *this;
      }
      /// Set key submask
      Key& set_submask(const char* opt_tag);
      /// Set key item identifier
      Key& set_item(itemkey_type i)  {
	this->values.item = i;
	return *this;
      }
      /// Set key mask
      Key& set_segment(segment_type seg)  {
	this->values.segment = seg;
	return *this;
      }
      /// Project the item part of the key
      mask_type mask()  const {
	return this->values.mask;
      }
      /// Project the mask part of the key
      itemkey_type item()  const  {
	return this->values.item;
      }
      /// Project the segment part of the key
      segment_type segment()  const  {
        return this->values.segment;
      }
      /// Access key as long integer
      key_type value()  const {
	return this->key;
      }
      /// Project the mask part of the key
      static itemkey_type item(Key k)  {
	return k.values.item;
      }
      /// Project the item part of the key
      static mask_type mask(Key k)  {
	return k.values.mask;
      }
      /// Access key name (if registered properly)
      static std::string key_name(const Key& key);
    };

    /// Default constructor
    inline Key::Key()    {
      this->key = 0;
    }

    /// Move constructor
    inline Key::Key(Key&& copy)   {
      this->key = copy.key;
    }

    /// Copy constructor
    inline Key::Key(const Key& copy)   {
      this->key = copy.key;
    }
    /// Initializaing constructor with key generation using hash algorithm
    inline Key::Key(const char* item, mask_type mask)  {
      this->set(item, mask);
    }

    /// Initializaing constructor with key generation using hash algorithm
    inline Key::Key(const std::string& item, mask_type mask)  {
      this->set(item, mask);
    }

    /// Move assignment operator
    inline Key& Key::operator = (Key&& copy)   {
      this->key = copy.key;
      return *this;
    }

    /// Assignment operator
    inline Key& Key::operator = (const Key& copy)   {
      this->key = copy.key;
      return *this;
    }

    /// Equality operator
    inline bool Key::operator == (const Key& other)   const    {
      return this->key == other.key;
    }

    /// Operator less
    inline bool Key::operator < (const Key& other)   const    {
      return this->key < other.key;
    }

    /// Operator greator
    inline bool Key::operator > (const Key& other)   const    {
      return this->key > other.key;
    }

    class SegmentEntry   {
    public:
      std::string      name { };
      Key              key  { };
    public:
      /// Initializing constructor
      SegmentEntry(const std::string& name, Key::mask_type mask);
      /// Default constructor
      SegmentEntry() = default;
      /// Disable move constructor
      SegmentEntry(SegmentEntry&& copy) = default;
      /// Disable copy constructor
      SegmentEntry(const SegmentEntry& copy) = default;      
      /// Default destructor
      virtual ~SegmentEntry() = default;
      /// Disable move assignment
      SegmentEntry& operator=(SegmentEntry&& copy) = default;
      /// Disable copy assignment
      SegmentEntry& operator=(const SegmentEntry& copy) = default;      
    };

    /// Initializing constructor
    inline SegmentEntry::SegmentEntry(const std::string& nam, Key::mask_type msk)
      : name(nam)
    {
      key.set(nam, msk);
    }

    /// Particle definition for digitization
    /** Particle definition for digitization
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Particle   {
    public:
      Position  start_position { };
      Position  end_position   { };
      Direction momentum       { };
      double    mass           { 0e0 };
      char      charge         { 0 };
      /// Source contributing
      Key       history;

    public:
      /// Initializing constructor
      Particle(Key history);
      /// Default constructor
      Particle() = default;
      /// Disable move constructor
      Particle(Particle&& copy) = default;
      /// Disable copy constructor
      Particle(const Particle& copy) = default;
      /// Default destructor
      virtual ~Particle() = default;
      /// Disable move assignment
      Particle& operator=(Particle&& copy) = default;
      /// Disable copy assignment
      Particle& operator=(const Particle& copy) = default;
    };

    /// Initializing constructor
    inline Particle::Particle(Key h)
      : history(h)
    {
    }

    /// Particle mapping definition for digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class ParticleMapping : public SegmentEntry   {
      using container_t    = std::map<Key, Particle>;
      using value_type     = container_t::value_type;
      using mapped_type    = container_t::mapped_type;
      using key_type       = container_t::key_type;
      using iterator       = container_t::iterator;
      using const_iterator = container_t::const_iterator;

      container_t data;

    public: 
      /// Initializing constructor
      ParticleMapping(const std::string& name, Key::mask_type mask);
      /// Default constructor
      ParticleMapping() = default;
      /// Disable move constructor
      ParticleMapping(ParticleMapping&& copy) = default;
      /// Disable copy constructor
      ParticleMapping(const ParticleMapping& copy) = default;
      /// Default destructor
      virtual ~ParticleMapping() = default;
      /// Disable move assignment
      ParticleMapping& operator=(ParticleMapping&& copy) = default;
      /// Disable copy assignment
      ParticleMapping& operator=(const ParticleMapping& copy) = default;

      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(ParticleMapping&& updates);
      /// Add new entry to the particle mapping (not thread safe!)
      void push(Key key, Particle&& particle);

      /// Merge new deposit map onto existing map (not thread safe!) (CONST)
      std::size_t insert(const ParticleMapping& updates);
      /// Add new entry to the particle mapping (not thread safe!) (CONST)
      void insert(Key key, const Particle& particle);

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }
      /// Insert new entry
      //std::pair<iterator, bool> emplace(key_type entry_key, mapped_type&& entry_data);
      void emplace(Key entry_key, Particle&& entry_data);

      /** Iteration support */
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
    };

    /// Initializing constructor
    inline ParticleMapping::ParticleMapping(const std::string& nam, Key::mask_type msk)
      : SegmentEntry(nam, msk)
    {
    }

    ///  Class to hold the processing history of the detector response
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class History   {
    public:
      struct hist_entry_t   {
	Key    source { };
	double weight { 0e0 };
	hist_entry_t(Key s, double w);
	hist_entry_t() = default;
	hist_entry_t(hist_entry_t&& copy) = default;
	hist_entry_t(const hist_entry_t& copy) = default;
	hist_entry_t& operator=(hist_entry_t&& copy) = default;
	hist_entry_t& operator=(const hist_entry_t& copy) = default;
	~hist_entry_t() = default;
      };
      /// Sources contributing to the deposit indexed by the cell identifier
      std::vector<hist_entry_t> hits;
      std::vector<hist_entry_t> particles;

    public:
      /// Default constructor
      History() = default;
      /// Disable move constructor
      History(History&& copy) = default;
      /// Disable copy constructor
      History(const History& copy) = default;
      /// Default destructor
      virtual ~History() = default;
      /// Disable move assignment
      History& operator=(History&& copy) = default;
      /// Disable copy assignment
      History& operator=(const History& copy) = default;

      /// Update history
      void update(const History& upda);
      /// Drop history information
      std::pair<std::size_t,std::size_t> drop();
    };

    inline History::hist_entry_t::hist_entry_t(Key s, double w)
      : source(s), weight(w)  {
    }

    /// Energy deposit definition for digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class EnergyDeposit   {
    public:
      /// Hit position
      Position       position    { };
      /// Hit direction
      Direction      momentum    { };
      /// Length of the track segment contributing to this hit
      double         length      { 0 };
      /// Total energy deposit
      double         deposit     { 0 };
      /// Optional flag for user masks
      long           flag        { 0 };
      /// Source mask of this deposit
      Key::mask_type mask        { 0 };

      /// Sources contributing to this deposit
      History        history;

    public:
      /// Default constructor
      EnergyDeposit() = default;
      /// Disable move constructor
      EnergyDeposit(EnergyDeposit&& copy) = default;
      /// Disable copy constructor
      EnergyDeposit(const EnergyDeposit& copy) = default;
      /// Default destructor
      virtual ~EnergyDeposit() = default;
      /// Disable move assignment
      EnergyDeposit& operator=(EnergyDeposit&& copy) = default;
      /// Disable copy assignment
      EnergyDeposit& operator=(const EnergyDeposit& copy) = default;
      /// Update the deposit using deposit weighting
      void update_deposit_weighted(EnergyDeposit&& update);
      /// Update the deposit using deposit weighting
      void update_deposit_weighted(const EnergyDeposit& update);
    };


    /// Energy deposit vector definition for digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DepositVector : public SegmentEntry  {
    public: 
      using container_t    = std::vector<std::pair<CellID, EnergyDeposit> >;
      using iterator       = container_t::iterator;
      using const_iterator = container_t::const_iterator;

      container_t    data { };

    public: 
      /// Initializing constructor
      DepositVector(const std::string& name, Key::mask_type mask);
      /// Default constructor
      DepositVector() = default;
      /// Disable move constructor
      DepositVector(DepositVector&& copy) = default;
      /// Disable copy constructor
      DepositVector(const DepositVector& copy) = default;      
      /// Default destructor
      virtual ~DepositVector() = default;
      /// Disable move assignment
      DepositVector& operator=(DepositVector&& copy) = default;
      /// Disable copy assignment
      DepositVector& operator=(const DepositVector& copy) = default;      
      /// Merge new deposit map onto existing vector (destroys inputs. not thread safe!)
      std::size_t merge(DepositVector&& updates);
      /// Merge new deposit map onto existing vector (destroys inputs. not thread safe!)
      std::size_t merge(const DepositVector& updates);
      /// Merge new deposit map onto existing map (destroys inputs. not thread safe!)
      std::size_t merge(DepositMapping&& updates);
      /// Merge new deposit map onto existing map (destroys inputs. not thread safe!)
      std::size_t merge(const DepositMapping& updates);
      /// Merge new deposit map onto existing vector (keep inputs. not thread safe!)
      std::size_t insert(const DepositVector& updates);
      /// Merge new deposit map onto existing map (keep inputs. not thread safe!)
      std::size_t insert(const DepositMapping& updates);
      /// Emplace entry
      void emplace(CellID cell, EnergyDeposit&& deposit);

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }

      /** Iteration support */
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
    };

    /// Initializing constructor
    inline DepositVector::DepositVector(const std::string& nam, Key::mask_type msk)
      : SegmentEntry(nam, msk)
    {
    }

    /// Emplace entry
    inline void DepositVector::emplace(CellID cell, EnergyDeposit&& deposit)   {
      this->data.emplace_back(cell, std::move(deposit));
    }

    /// Energy deposit mapping definition for digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DepositMapping : public SegmentEntry  {
    public: 
      using container_t    = std::multimap<CellID, EnergyDeposit>;
      using iterator       = container_t::iterator;
      using const_iterator = container_t::const_iterator;

      container_t    data { };

    public: 
      /// Initializing constructor
      DepositMapping(const std::string& name, Key::mask_type mask);
      /// Default constructor
      DepositMapping() = default;
      /// Disable move constructor
      DepositMapping(DepositMapping&& copy) = default;
      /// Disable copy constructor
      DepositMapping(const DepositMapping& copy) = default;      
      /// Default destructor
      virtual ~DepositMapping() = default;
      /// Disable move assignment
      DepositMapping& operator=(DepositMapping&& copy) = default;
      /// Disable copy assignment
      DepositMapping& operator=(const DepositMapping& copy) = default;      

      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(DepositMapping&& updates);
      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t insert(const DepositMapping& updates);

      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(DepositVector&& updates);
      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t insert(const DepositVector& updates);

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }

      /** Iteration support */
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
    };

    /// Initializing constructor
    inline DepositMapping::DepositMapping(const std::string& nam, Key::mask_type msk)
      : SegmentEntry(nam, msk)
    {
    }

    class ADCValue   {
    public:
      using value_t = uint32_t;
      using address_t = uint64_t;
    public:
      value_t    value;
      address_t  address;
    };
    
    /// Detector response vector definition for digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DetectorResponse : public SegmentEntry  {
    public: 
      using container_t    = std::vector<std::pair<CellID, ADCValue> >;
      using iterator       = container_t::iterator;
      using const_iterator = container_t::const_iterator;

      container_t    data { };

    public: 
      /// Initializing constructor
      DetectorResponse(const std::string& name, Key::mask_type mask);
      /// Default constructor
      DetectorResponse() = default;
      /// Disable move constructor
      DetectorResponse(DetectorResponse&& copy) = default;
      /// Disable copy constructor
      DetectorResponse(const DetectorResponse& copy) = default;      
      /// Default destructor
      virtual ~DetectorResponse() = default;
      /// Disable move assignment
      DetectorResponse& operator=(DetectorResponse&& copy) = default;
      /// Disable copy assignment
      DetectorResponse& operator=(const DetectorResponse& copy) = default;      

      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(DetectorResponse&& updates);
      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t insert(const DetectorResponse& updates);
      /// Emplace entry
      void emplace(CellID cell, ADCValue&& value);

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }

      /** Iteration support */
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
    };

    /// Initializing constructor
    inline DetectorResponse::DetectorResponse(const std::string& nam, Key::mask_type msk)
      : SegmentEntry(nam, msk)
    {
    }


    /// Emplace entry
    inline void DetectorResponse::emplace(CellID cell, ADCValue&& value)   {
      this->data.emplace_back(cell, std::move(value));
    }

    /// Detector history vector definition for digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DetectorHistory : public SegmentEntry  {
    public: 
      using container_t    = std::vector<std::pair<CellID, History> >;
      using iterator       = container_t::iterator;
      using const_iterator = container_t::const_iterator;

      container_t    data { };

    public: 
      /// Initializing constructor
      DetectorHistory(const std::string& name, Key::mask_type mask);
      /// Default constructor
      DetectorHistory() = default;
      /// Disable move constructor
      DetectorHistory(DetectorHistory&& copy) = default;
      /// Disable copy constructor
      DetectorHistory(const DetectorHistory& copy) = default;      
      /// Default destructor
      virtual ~DetectorHistory() = default;
      /// Disable move assignment
      DetectorHistory& operator=(DetectorHistory&& copy) = default;
      /// Disable copy assignment
      DetectorHistory& operator=(const DetectorHistory& copy) = default;      

      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(DetectorHistory&& updates);
      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t insert(const DetectorHistory& updates);

      /// Insert new entry
      void insert(CellID cell, const History& value);
      /// Emplace new entry
      void emplace(CellID cell, History&& value);

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }

      /** Iteration support */
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
    };

    /// Initializing constructor
    inline DetectorHistory::DetectorHistory(const std::string& nam, Key::mask_type msk)
      : SegmentEntry(nam, msk)
    {
    }

    /// Emplace new entry
    inline void DetectorHistory::emplace(CellID cell, History&& value)   {
      this->data.emplace_back(cell, std::move(value));
    }

    /// Insert new entry
    inline void DetectorHistory::insert(CellID cell, const History& value)   {
      this->data.emplace_back(cell, value);
    }


    ///  Data segment definition (locked map)
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DataSegment   {
    public:
      using key_t = Key::key_type;
      using container_map_t = std::map<Key, std::any>;
      using iterator        = container_map_t::iterator;
      using const_iterator  = container_map_t::const_iterator;

    private:
      /// Call on failed any-casts
      std::string invalid_cast(Key key, const std::type_info& type)  const;
      /// Call on failed data requests during data requests
      std::string invalid_request(Key key)  const;

      /// Access data item by key
      std::any* get_item(Key key, bool exc);
      /// Access data item by key  (CONST)
      const std::any* get_item(Key key, bool exc)  const;

    public:
      container_map_t data;
      std::mutex&     lock;
      uint32_t        id  { 0 };
    public:
      /// Initializing constructor
      DataSegment(std::mutex& lock, int id);
      /// Default constructor
      DataSegment() = delete;
      /// Disable move constructor
      DataSegment(DataSegment&& copy) = delete;
      /// Disable copy constructor
      DataSegment(const DataSegment& copy) = delete;      
      /// Default destructor
      virtual ~DataSegment() = default;
      /// Disable move assignment
      DataSegment& operator=(DataSegment&& copy) = delete;
      /// Disable copy assignment
      DataSegment& operator=(const DataSegment& copy) = delete;      

      /** Locked operations */
      /// Emplace data item (locked)
      bool emplace(Key key, std::any&& data);
      /// Move data items other than std::any to the data segment
      template <typename DATA> bool put(Key key, DATA&& data);
      /// Remove data item from segment (locked)
      bool erase(Key key);
      /// Remove data items from segment (locked)
      std::size_t erase(const std::vector<Key>& keys);
      /// Print segment keys
      void print_keys()   const;
      
      /** Unlocked operations */
      /// Access data by key. If not existing, nullptr is returned
      std::any* entry(Key key)              { return this->get_item(key, false); }
      /// Access data by key. If not existing, nullptr is returned
      const std::any* entry(Key key)  const { return this->get_item(key, false); }

      /// Access data as reference by key. If not existing, an exception is thrown
      template<typename T> T& get(Key key);
      /// Access data as reference by key. If not existing, an exception is thrown
      template<typename T> const T& get(Key key)  const;

      /// Access data as pointers by key. If not existing, nullptr is returned
      template<typename T> T* pointer(Key key);
      /// Access data as pointers by key. If not existing, nullptr is returned
      template<typename T> const T* pointer(Key key)  const;

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Find entry by key
      iterator find(Key key)              { return this->data.find(key);     }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
      /// Find entry by key
      const_iterator find(Key key) const  { return this->data.find(key);     }
    };

    /// Access data as reference by key. If not existing, an exception is thrown
    template<typename T> inline T& DataSegment::get(Key key)     {
      if ( T* ptr = std::any_cast<T>(this->get_item(key, true)) )
	return *ptr;
      throw std::runtime_error(this->invalid_cast(key, typeid(T)));
    }
    /// Access data as reference by key. If not existing, an exception is thrown
    template<typename T> inline const T& DataSegment::get(Key key)  const   {
      if ( const T* ptr = std::any_cast<T>(this->get_item(key, true)) )
	return *ptr;
      throw std::runtime_error(this->invalid_cast(key, typeid(T)));
    }

    /// Access data as pointers by key. If not existing, nullptr is returned
    template<typename T> inline T* DataSegment::pointer(Key key)     {
      if ( T* ptr = std::any_cast<T>(this->get_item(key, false)) )
	return ptr;
      return nullptr;
    }
    /// Access data as pointers by key. If not existing, nullptr is returned
    template<typename T> inline const T* DataSegment::pointer(Key key)  const   {
      if ( const T* ptr = std::any_cast<T>(this->get_item(key, false)) )
	return ptr;
      return nullptr;
    }

    ///  User event data for DDDigi
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class  DigiEvent  {
    private:
      using segment_t = std::unique_ptr<DataSegment>;
      /// Event lock
      std::mutex  m_lock;
      /// String identifier of this event (for debug printouts)
      std::string m_id;
      /// Reference to the general purpose data segment
      segment_t m_data;
      /// Reference to the counts data segment
      segment_t m_counts;
      /// Reference to the input data segment
      segment_t m_inputs;
      /// Reference to the output data segment
      segment_t m_outputs;
      /// Reference to the deposit data segment
      segment_t m_deposits;

      /// Helper: Save access with segment creation if it does not exist
      DataSegment& access_segment(segment_t& seg, uint32_t id);

    public:
      /// Current event number
      int eventNumber  { 0 };

    public:
#if defined(DD4HEP_INTERPRETER_MODE) || defined(G__ROOT)
      /// Inhibit default constructor
      DigiEvent();
#endif
      /// Inhibit move constructor
      DigiEvent(DigiEvent&& copy) = delete;
      /// Inhibit copy constructor
      DigiEvent(const DigiEvent& copy) = delete;
      /// Intializing constructor
      DigiEvent(int num);
      /// Default destructor
      virtual ~DigiEvent();
      /// String identifier of this event
      const char* id()   const    {   return this->m_id.c_str();   }
      /// Retrieve data segment from the event structure
      DataSegment& get_segment(const std::string& name);
    };
  }    // End namespace digi
}      // End namespace dd4hep

#endif // DDDIGI_DIGIDATA_H
