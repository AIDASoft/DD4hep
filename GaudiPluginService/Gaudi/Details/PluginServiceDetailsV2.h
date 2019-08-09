#ifndef _GAUDI_PLUGIN_SERVICE_DETAILS_V2_H_
#define _GAUDI_PLUGIN_SERVICE_DETAILS_V2_H_
/*****************************************************************************\
* (c) Copyright 2013 CERN                                                     *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

/// @author Marco Clemencic <marco.clemencic@cern.ch>

#include "Gaudi/Details/PluginServiceCommon.h"

#if __cplusplus >= 201703
#  include <any>
#else
#  include <boost/any.hpp>
namespace std {
  using boost::any;
  using boost::any_cast;
  using boost::bad_any_cast;
} // namespace std
#endif

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <utility>

namespace Gaudi {
  namespace PluginService {
    GAUDI_PLUGIN_SERVICE_V2_INLINE namespace v2 {
      /// \cond FWD_DECL
      template <typename>
      struct Factory;
      /// \cond

      /// Implementation details of Gaudi::PluginService
      namespace Details {
        template <typename>
        struct Traits;

        template <typename R, typename... Args>
        struct Traits<R( Args... )> {
          using ReturnType  = std::unique_ptr<std::remove_pointer_t<R>>;
          using FactoryType = std::function<ReturnType( Args... )>;
        };

        /// Return a canonical name for type_info object (implementation borrowed
        /// from GaudiKernel/System.h).
        GAUDIPS_API
        std::string demangle( const std::type_info& id );

        /// Return a canonical name for the template argument.
        template <typename T>
        inline std::string demangle() {
          return demangle( typeid( T ) );
        }

        /// Convert a generic `id` to `std::string` via `std::ostream::operator<<`.
        template <typename ID>
        inline std::string stringify_id( const ID& id ) {
          std::ostringstream o;
          o << id;
          return o.str();
        }
        /// Specialized no-op conversion from `std::string` to `std::string`.
        template <>
        inline std::string stringify_id<std::string>( const std::string& id ) {
          return id;
        }

        /// Helper to print debug output in case of mismatched FactoryType.
        void reportBadAnyCast( const std::type_info& factory_type, const std::string& id );

        /// Simple logging class, just to provide a default implementation.
        class GAUDIPS_API Logger {
        public:
          enum Level { Debug = 0, Info = 1, Warning = 2, Error = 3 };
          Logger( Level level = Warning ) : m_level( level ) {}
          virtual ~Logger() {}
          inline Level level() const { return m_level; }
          inline void  setLevel( Level level ) { m_level = level; }
          inline void  info( const std::string& msg ) { report( Info, msg ); }
          inline void  debug( const std::string& msg ) { report( Debug, msg ); }
          inline void  warning( const std::string& msg ) { report( Warning, msg ); }
          inline void  error( const std::string& msg ) { report( Error, msg ); }

        private:
          virtual void report( Level lvl, const std::string& msg );
          Level        m_level;
        };

        /// Return the current logger instance.
        GAUDIPS_API Logger& logger();
        /// Set the logger instance to use.
        /// It must be a new instance and the ownership is passed to the function.
        GAUDIPS_API void setLogger( Logger* logger );

        /// In-memory database of the loaded factories.
        class GAUDIPS_API Registry {
        public:
          using KeyType = std::string;

          /// Type used for the properties implementation.
          using Properties = std::map<KeyType, std::string>;

          struct FactoryInfo {
            std::string library;
            std::any    factory{};
            Properties  properties{};

            inline bool is_set() const {
#if __cplusplus >= 201703
              return factory.has_value();
#else
              return !factory.empty();
#endif
            }
            Properties::mapped_type getprop( const Properties::key_type& name ) const;
          };

          /// Type used for the database implementation.
          using FactoryMap = std::map<KeyType, FactoryInfo>;

          /// Get the factory function for a given `id` from the registry.
          template <typename F>
          F get( const KeyType& id ) {
            const FactoryInfo& info = Registry::instance().getInfo( id, true );
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
            if ( !info.getprop( "ReflexName" ).empty() ) {
              const std::string real_name = info.getprop( "ClassName" );
              logger().warning( "requesting factory via old name '" + id + "' use '" +
                                ( real_name.empty() ? "<undefined>" : real_name ) + "' instead" );
            }
#endif
            return std::any_cast<F>( info.factory );
          }

          /// Retrieve the singleton instance of Registry.
          static Registry& instance();

          /// Add factory info to the registry (used internally by DeclareFactory).
          FactoryInfo& add( const KeyType& id, FactoryInfo info );

          /// Retrieve the FactoryInfo object for an `id`.
          const FactoryInfo& getInfo( const KeyType& id, const bool load = false ) const;

          /// Add a property to an already existing FactoryInfo object (via its `id`).
          Registry& addProperty( const KeyType& id, const KeyType& k, const std::string& v );

          /// Return a list of all the known and loaded factories
          std::set<KeyType> loadedFactoryNames() const;

          /// Return the known factories (loading the list if not yet done).
          ///
          /// At the first call, the internal database of known factories is
          /// filled with the name of the libraries containing them, using the
          /// ".components" files in the `LD_LIBRARY_PATH`.
          const FactoryMap& factories() const;

        private:
          /// Private constructor for the singleton pattern.
          Registry();

          /// Private copy constructor for the singleton pattern.
          Registry( const Registry& ) = delete;

          /// Return the known factories (loading the list if not yet done).
          FactoryMap& factories();

          /// Initialize the registry loading the list of factories from the
          /// .component files in the library search path.
          void initialize();

          /// Flag recording if the registry has been initialized or not.
          mutable std::once_flag m_initialized;

          /// Internal storage for factories.
          FactoryMap m_factories;

          /// Mutex used to control concurrent access to the internal data.
          mutable std::recursive_mutex m_mutex;
        };

        /// Class providing default factory functions.
        ///
        /// The template argument T is the class to be created, while the methods
        /// template argument S is the specific factory signature.
        template <typename, typename>
        struct DefaultFactory;
        template <typename T, typename R, typename... Args>
        struct DefaultFactory<T, Factory<R( Args... )>> {
          inline typename Factory<R( Args... )>::ReturnType operator()( Args... args ) {
            return std::make_unique<T>( std::move( args )... );
          }
        };

        /// Helper to get the name of the library containing a given pointer to function.
        ///
        /// Implementation borrowed from `DsoUtils.h` (genconf).
        std::string getDSONameFor( void* fptr );
      } // namespace Details

      /// Backward compatibility with Reflex.
      GAUDIPS_API void SetDebug( int debugLevel );
      /// Backward compatibility with Reflex.
      GAUDIPS_API int Debug();
    }
  } // namespace PluginService
} // namespace Gaudi

#define _PS_V2_INTERNAL_FACTORY_MAKE_REGISTER_CNAME_TOKEN( serial ) _register_##serial
#define _PS_V2_INTERNAL_FACTORY_MAKE_REGISTER_CNAME( serial )                                                          \
  _PS_V2_INTERNAL_FACTORY_MAKE_REGISTER_CNAME_TOKEN( serial )
#define _PS_V2_INTERNAL_FACTORY_REGISTER_CNAME _PS_V2_INTERNAL_FACTORY_MAKE_REGISTER_CNAME( __LINE__ )

#endif //_GAUDI_PLUGIN_SERVICE_DETAILS_H_
