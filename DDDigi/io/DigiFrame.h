#ifndef PODIO_FRAME_H
#define PODIO_FRAME_H

#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/ICollectionProvider.h"
#include "podio/utilities/TypeHelpers.h"

#include <initializer_list>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace podio {

/// Alias template for enabling overloads only for Collections
template <typename T>
using EnableIfCollection = typename std::enable_if_t<isCollection<T>>;

/// Alias template for enabling overloads only for Collection r-values
template <typename T>
using EnableIfCollectionRValue = typename std::enable_if_t<isCollection<T> && !std::is_lvalue_reference_v<T>>;

namespace detail {
  /** The minimal interface for raw data types
   */
  struct EmptyFrameData {
    podio::CollectionIDTable getIDTable() const {
      return {};
    }

    std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string&) {
      return std::nullopt;
    }

    /** Get the still available, i.e. yet unpacked, collections from the raw data
     */
    std::vector<std::string> getAvailableCollections() const {
      return {};
    }

    /** Get the parameters that are stored in the raw data
     */
    std::unique_ptr<podio::GenericParameters> getParameters() {
      return std::make_unique<podio::GenericParameters>();
    }
  };
} // namespace detail

template <typename FrameDataT>
std::optional<podio::CollectionReadBuffers> unpack(FrameDataT* data, const std::string& name) {
  return data->getCollectionBuffers(name);
}

/**
 * Frame class that serves as a container of collection and meta data.
 */
class Frame {
  /**
   * Internal abstract interface for the type-erased implementation of the Frame
   * class
   */
  struct FrameConcept {
    virtual ~FrameConcept() = default;
    virtual const podio::CollectionBase* get(const std::string& name) const = 0;
    virtual const podio::CollectionBase* put(std::unique_ptr<podio::CollectionBase> coll, const std::string& name) = 0;
    virtual podio::GenericParameters& parameters() = 0;
    virtual const podio::GenericParameters& parameters() const = 0;

    virtual std::vector<std::string> availableCollections() const = 0;

    // Writing interface. Need this to be able to store all necessary information
    // TODO: Figure out whether this can be "hidden" somehow
    virtual podio::CollectionIDTable getIDTable() const = 0;
  };

  /**
   * The interface implementation of the abstract FrameConcept that is necessary
   * for a type-erased implementation of the Frame class
   */
  template <typename FrameDataT>
  struct FrameModel final : FrameConcept, public ICollectionProvider {

    FrameModel(std::unique_ptr<FrameDataT> data);
    ~FrameModel() = default;
    FrameModel(const FrameModel&) = delete;
    FrameModel& operator=(const FrameModel&) = delete;
    FrameModel(FrameModel&&) = default;
    FrameModel& operator=(FrameModel&&) = default;

    /** Try and get the collection from the internal storage and return a
     * pointer to it if found. Otherwise return a nullptr
     */
    const podio::CollectionBase* get(const std::string& name) const final;

    /** Try and place the collection into the internal storage and return a
     * pointer to it. If a collection already exists or insertion fails, return
     * a nullptr
     */
    const podio::CollectionBase* put(std::unique_ptr<CollectionBase> coll, const std::string& name) final;

    /** Get a reference to the internally used GenericParameters
     */
    podio::GenericParameters& parameters() override {
      return *m_parameters;
    }
    /** Get a const reference to the internally used GenericParameters
     */
    const podio::GenericParameters& parameters() const override {
      return *m_parameters;
    };

    bool get(int collectionID, podio::CollectionBase*& collection) const override;

    podio::CollectionIDTable getIDTable() const override {
      // Make a copy
      return {m_idTable.ids(), m_idTable.names()};
    }

    std::vector<std::string> availableCollections() const override;

  private:
    podio::CollectionBase* doGet(const std::string& name, bool setReferences = true) const;

    using CollectionMapT = std::unordered_map<std::string, std::unique_ptr<podio::CollectionBase>>;

    mutable CollectionMapT m_collections{};                 ///< The internal map for storing unpacked collections
    mutable std::unique_ptr<std::mutex> m_mapMtx{nullptr};  ///< The mutex for guarding the internal collection map
    std::unique_ptr<FrameDataT> m_data{nullptr};            ///< The raw data read from file
    mutable std::unique_ptr<std::mutex> m_dataMtx{nullptr}; ///< The mutex for guarding the raw data
    podio::CollectionIDTable m_idTable{};                   ///< The collection ID table
    std::unique_ptr<podio::GenericParameters> m_parameters{nullptr}; ///< The generic parameter store for this frame
    mutable std::set<int> m_retrievedIDs{}; ///< The IDs of the collections that we have already read (but not yet put
                                            ///< into the map)
  };

  std::unique_ptr<FrameConcept> m_self; ///< The internal concept pointer through which all the work is done

public:
  /** Empty Frame constructor
   */
  Frame();

  /** Frame constructor from (almost) arbitrary raw data
   */
  template <typename FrameDataT>
  Frame(std::unique_ptr<FrameDataT>);

  // The frame is a non-copyable type
  Frame(const Frame&) = delete;
  Frame& operator=(const Frame&) = delete;

  Frame(Frame&&) = default;
  Frame& operator=(Frame&&) = default;

  /** Frame destructor */
  ~Frame() = default;

  const std::unique_ptr<FrameConcept>& self() const  { return m_self; }

  /** Get a collection from the Frame
   */
  template <typename CollT, typename = EnableIfCollection<CollT>>
  const CollT& get(const std::string& name) const;

  /** (Destructively) move a collection into the Frame and get a const reference
   * back for further use
   */
  template <typename CollT, typename = EnableIfCollectionRValue<CollT>>
  const CollT& put(CollT&& coll, const std::string& name);

  /** Move a collection into the Frame handing over ownership to the Frame
   */
  void put(std::unique_ptr<podio::CollectionBase> coll, const std::string& name);

  /** Add a value to the parameters of the Frame (if the type is supported).
   * Copy the value into the internal store
   */
  template <typename T, typename = podio::EnableIfValidGenericDataType<T>>
  void putParameter(const std::string& key, T value) {
    m_self->parameters().setValue(key, value);
  }

  /** Add a string value to the parameters of the Frame by copying it. Dedicated
   * overload for enabling the on-the-fly conversion on the string literals.
   */
  void putParameter(const std::string& key, std::string value) {
    putParameter<std::string>(key, std::move(value));
  }

  /** Add a vector of strings to the parameters of the Frame (via copy).
   * Dedicated overload for enabling on-the-fly conversions of initializer_list
   * of string literals.
   */
  void putParameter(const std::string& key, std::vector<std::string> values) {
    putParameter<std::vector<std::string>>(key, std::move(values));
  }

  /** Add a vector of values into the parameters of the Frame. Overload for
   * catching on-the-fly conversions of initializer_lists of values.
   */
  template <typename T, typename = std::enable_if_t<detail::isInTuple<T, SupportedGenericDataTypes>>>
  void putParameter(const std::string& key, std::initializer_list<T>&& values) {
    putParameter<std::vector<T>>(key, std::move(values));
  }

  /** Retrieve parameters via key from the internal store. Return type will
   * either by a const reference or a value depending on the desired type.
   */
  template <typename T, typename = podio::EnableIfValidGenericDataType<T>>
  podio::GenericDataReturnType<T> getParameter(const std::string& key) const {
    return m_self->parameters().getValue<T>(key);
  }

  /** Get all **currently** available collections (including potentially
   * unpacked ones from raw data)
   */
  std::vector<std::string> getAvailableCollections() const {
    return m_self->availableCollections();
  }

  // Interfaces for writing below
  // TODO: Hide this from the public interface somehow?

  /**
   * Get the GenericParameters for writing
   */
  const podio::GenericParameters& getGenericParametersForWrite() const {
    return m_self->parameters();
  }

  /**
   * Get a collection for writing (in a prepared and "ready-to-write" state)
   */
  const podio::CollectionBase* getCollectionForWrite(const std::string& name) const {
    const auto* coll = m_self->get(name);
    if (coll) {
      coll->prepareForWrite();
    }

    return coll;
  }

  podio::CollectionIDTable getCollectionIDTableForWrite() const {
    return m_self->getIDTable();
  }
};

// implementations below

Frame::Frame() : Frame(std::make_unique<detail::EmptyFrameData>()) {
}

template <typename FrameDataT>
Frame::Frame(std::unique_ptr<FrameDataT> data) : m_self(std::make_unique<FrameModel<FrameDataT>>(std::move(data))) {
}

template <typename CollT, typename>
const CollT& Frame::get(const std::string& name) const {
  const auto* coll = dynamic_cast<const CollT*>(m_self->get(name));
  if (coll) {
    return *coll;
  }
  // TODO: Handle non-existing collections
  static const auto emptyColl = CollT();
  return emptyColl;
}

void Frame::put(std::unique_ptr<podio::CollectionBase> coll, const std::string& name) {
  const auto* retColl = m_self->put(std::move(coll), name);
  if (!retColl) {
    // TODO: Handle collisions
  }
}

template <typename CollT, typename>
const CollT& Frame::put(CollT&& coll, const std::string& name) {
  const auto* retColl = static_cast<const CollT*>(m_self->put(std::make_unique<CollT>(std::move(coll)), name));
  if (retColl) {
    return *retColl;
  }
  // TODO: Handle collision case
  static const auto emptyColl = CollT();
  return emptyColl;
}

template <typename FrameDataT>
Frame::FrameModel<FrameDataT>::FrameModel(std::unique_ptr<FrameDataT> data) :
    m_mapMtx(std::make_unique<std::mutex>()),
    m_data(std::move(data)),
    m_dataMtx(std::make_unique<std::mutex>()),
    m_idTable(std::move(m_data->getIDTable())),
    m_parameters(std::move(m_data->getParameters())) {
}

template <typename FrameDataT>
const podio::CollectionBase* Frame::FrameModel<FrameDataT>::get(const std::string& name) const {
  return doGet(name);
}

template <typename FrameDataT>
podio::CollectionBase* Frame::FrameModel<FrameDataT>::doGet(const std::string& name, bool setReferences) const {
  {
    // First check whether the collection is in the map already
    //
    // Collections only land here if they are fully unpacked, i.e.
    // prepareAfterRead has been called or it has been put into the Frame
    std::lock_guard lock{*m_mapMtx};
    if (const auto it = m_collections.find(name); it != m_collections.end()) {
      return it->second.get();
    }
  }

  podio::CollectionBase* retColl = nullptr;

  // Now try to get it from the raw data if we have the possibility
  if (m_data) {
    // Have the buffers in the outer scope here to hold the raw data lock as
    // briefly as possible
    auto buffers = std::optional<podio::CollectionReadBuffers>{std::nullopt};
    {
      std::lock_guard lock{*m_dataMtx};
      buffers = unpack(m_data.get(), name);
    }
    if (buffers) {
      auto coll = buffers->createCollection(buffers.value(), buffers->data == nullptr);
      coll->prepareAfterRead();
      coll->setID(m_idTable.collectionID(name));
      {
        std::lock_guard mapLock{*m_mapMtx};
        auto [it, success] = m_collections.emplace(name, std::move(coll));
        // TODO: Check success? Or simply assume that everything is fine at this point?
        // TODO: Collision handling?
        retColl = it->second.get();
      }

      if (setReferences) {
        retColl->setReferences(this);
      }
    }
  }

  return retColl;
}

template <typename FrameDataT>
bool Frame::FrameModel<FrameDataT>::get(int collectionID, CollectionBase*& collection) const {
  const auto& name = m_idTable.name(collectionID);
  const auto& [_, inserted] = m_retrievedIDs.insert(collectionID);

  if (!inserted) {
    auto coll = doGet(name);
    if (coll) {
      collection = coll;
      return true;
    }
  } else {
    auto coll = doGet(name, false);
    if (coll) {
      collection = coll;
      return true;
    }
  }

  return false;
}

template <typename FrameDataT>
const podio::CollectionBase* Frame::FrameModel<FrameDataT>::put(std::unique_ptr<podio::CollectionBase> coll,
                                                                const std::string& name) {
  {
    std::lock_guard lock{*m_mapMtx};
    auto [it, success] = m_collections.try_emplace(name, std::move(coll));
    if (success) {
      // TODO: Check whether this collection is already known to the idTable
      // -> What to do on collision?
      // -> Check before we emplace it into the internal map to prevent possible
      //    collisions from collections that are potentially present from rawdata?
      it->second->setID(m_idTable.add(name));
      return it->second.get();
    }
  }

  return nullptr;
}

template <typename FrameDataT>
std::vector<std::string> Frame::FrameModel<FrameDataT>::availableCollections() const {
  // TODO: Check if there is a more efficient way to do this. Currently this is
  // done very conservatively, but in a way that should always work, regardless
  // of assumptions. It might be possible to simply return what is in the
  // idTable here, because that should in principle encompass everything that is
  // in the raw data as well as things that have been put into the frame

  // Lock both the internal map and the rawdata for this
  std::scoped_lock lock{*m_mapMtx, *m_dataMtx};

  auto collections = m_data->getAvailableCollections();
  collections.reserve(collections.size() + m_collections.size());

  for (const auto& [name, _] : m_collections) {
    collections.push_back(name);
  }

  return collections;
}

} // namespace podio

#endif // PODIO_FRAME_H
