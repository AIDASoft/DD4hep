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

#define GAUDI_PLUGIN_SERVICE_V1
#include <Gaudi/PluginService.h>

#include <dirent.h>
#include <dlfcn.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

#include <cxxabi.h>
#include <sys/stat.h>

#define REG_SCOPE_LOCK std::lock_guard<std::recursive_mutex> _guard( m_mutex );

namespace {
  std::mutex registrySingletonMutex;
}
#define SINGLETON_LOCK std::lock_guard<std::mutex> _guard( ::registrySingletonMutex );

#include <algorithm>

namespace {
  // string trimming functions taken from
  // http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

  constexpr struct is_space_t {
    bool operator()( int i ) const { return std::isspace( i ); }
  } is_space{};

  // trim from start
  static inline std::string& ltrim( std::string& s ) {
    s.erase( s.begin(), std::find_if_not( s.begin(), s.end(), is_space ) );
    return s;
  }

  // trim from end
  static inline std::string& rtrim( std::string& s ) {
    s.erase( std::find_if_not( s.rbegin(), s.rend(), is_space ).base(), s.end() );
    return s;
  }
  // trim from both ends
  static inline std::string& trim( std::string& s ) { return ltrim( rtrim( s ) ); }
} // namespace

namespace {
  /// Helper function used to set values in FactoryInfo data members only
  /// if the original value is empty and reporting warnings in case of
  /// inconsistencies.
  inline void factoryInfoSetHelper( std::string& dest, const std::string value, const std::string& desc,
                                    const std::string& id ) {
    if ( dest.empty() ) {
      dest = value;
    } else if ( dest != value ) {
      Gaudi::PluginService::Details::logger().warning( "new factory loaded for '" + id + "' with different " + desc +
                                                       ": " + dest + " != " + value );
    }
  }

  struct OldStyleCnv {
    std::string name;
    void        operator()( const char c ) {
      switch ( c ) {
      case '<':
      case '>':
      case ',':
      case '(':
      case ')':
      case ':':
      case '.':
        name.push_back( '_' );
        break;
      case '&':
        name.push_back( 'r' );
        break;
      case '*':
        name.push_back( 'p' );
        break;
      case ' ':
        break;
      default:
        name.push_back( c );
        break;
      }
    }
  };
  /// Convert a class name in the string used with the Reflex plugin service
  std::string old_style_name( const std::string& name ) {
    return std::for_each( name.begin(), name.end(), OldStyleCnv() ).name;
  }
} // namespace

namespace Gaudi {
  namespace PluginService {
    GAUDI_PLUGIN_SERVICE_V1_INLINE namespace v1 {
      Exception::Exception( std::string msg ) : m_msg( std::move( msg ) ) {}
      Exception::~Exception() throw() {}
      const char* Exception::what() const throw() { return m_msg.c_str(); }

      namespace Details {
        void* getCreator( const std::string& id, const std::string& type ) {
          return Registry::instance().get( id, type );
        }

        std::string demangle( const std::string& id ) {
          int  status;
          auto realname = std::unique_ptr<char, decltype( free )*>(
              abi::__cxa_demangle( id.c_str(), nullptr, nullptr, &status ), free );
          if ( !realname ) return id;
#if _GLIBCXX_USE_CXX11_ABI
          return std::regex_replace(
              realname.get(),
              std::regex{"std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >( (?=>))?"},
              "std::string" );
#else
          return std::string{realname.get()};
#endif
        }
        std::string demangle( const std::type_info& id ) { return demangle( id.name() ); }

        Registry& Registry::instance() {
          SINGLETON_LOCK
          static Registry r;
          return r;
        }

        Registry::Registry() : m_initialized( false ) {}

        void Registry::initialize() {
          REG_SCOPE_LOCK
          if ( m_initialized ) return;
          m_initialized = true;
#if defined( _WIN32 )
          const char* envVar = "PATH";
          const char  sep    = ';';
#elif defined( __APPLE__ )
          const char* envVar = "DYLD_LIBRARY_PATH";
          const char  sep    = ':';
#else
          const char* envVar = "LD_LIBRARY_PATH";
          const char  sep    = ':';
#endif
          char* search_path = ::getenv( envVar );
          if ( search_path ) {
            logger().debug( std::string( "searching factories in " ) + envVar );
            std::string            path( search_path );
            std::string::size_type pos    = 0;
            std::string::size_type newpos = 0;
            while ( pos != std::string::npos ) {
              std::string dirName;
              // get the next entry in the path
              newpos = path.find( sep, pos );
              if ( newpos != std::string::npos ) {
                dirName = path.substr( pos, newpos - pos );
                pos     = newpos + 1;
              } else {
                dirName = path.substr( pos );
                pos     = newpos;
              }
              logger().debug( std::string( " looking into " ) + dirName );
              // look for files called "*.components" in the directory
              DIR* dir = opendir( dirName.c_str() );
              if ( dir ) {
                struct dirent* entry;
                while ( ( entry = readdir( dir ) ) ) {
                  std::string name( entry->d_name );
                  // check if the file name ends with ".components"
                  std::string::size_type extpos = name.find( ".components" );
                  if ( ( extpos != std::string::npos ) && ( ( extpos + 11 ) == name.size() ) ) {
                    std::string fullPath = ( dirName + '/' + name );
                    { // check if it is a regular file
                      struct stat buf;
                      stat( fullPath.c_str(), &buf );
                      if ( !S_ISREG( buf.st_mode ) ) continue;
                    }
                    // read the file
                    logger().debug( std::string( "  reading " ) + name );
                    std::ifstream factories{fullPath};
                    std::string   line;
                    int           factoriesCount = 0;
                    int           lineCount      = 0;
                    while ( !factories.eof() ) {
                      ++lineCount;
                      std::getline( factories, line );
                      trim( line );
                      // skip empty lines and lines starting with '#'
                      if ( line.empty() || line[0] == '#' ) continue;
                      // only accept "v1" factories
                      if ( line.substr( 0, 4 ) == "v1::" )
                        line = line.substr( 4 );
                      else
                        continue;
                      // look for the separator
                      auto pos = line.find( ':' );
                      if ( pos == std::string::npos ) {
                        logger().warning( "failed to parse line " + fullPath + ':' + std::to_string( lineCount ) );
                        continue;
                      }
                      const std::string lib( line, 0, pos );
                      const std::string fact( line, pos + 1 );
                      m_factories.emplace( fact, FactoryInfo( lib ) );
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
                      // add an alias for the factory using the Reflex convention
                      std::string old_name = old_style_name( fact );
                      if ( fact != old_name ) {
                        FactoryInfo old_info( lib );
                        old_info.properties["ReflexName"] = "true";
                        m_factories.emplace( old_name, old_info );
                      }
#endif
                      ++factoriesCount;
                    }
                    if ( logger().level() <= Logger::Debug ) {
                      logger().debug( "  found " + std::to_string( factoriesCount ) + " factories" );
                    }
                  }
                }
                closedir( dir );
              }
            }
          }
        }

        Registry::FactoryInfo& Registry::add( const std::string& id, void* factory, const std::string& type,
                                              const std::string& rtype, const std::string& className,
                                              const Properties& props ) {
          REG_SCOPE_LOCK
          FactoryMap& facts = factories();
          auto        entry = facts.find( id );
          if ( entry == facts.end() ) {
            // this factory was not known yet
            entry = facts.emplace( id, FactoryInfo( "unknown", factory, type, rtype, className, props ) ).first;
          } else {
            // do not replace an existing factory with a new one
            if ( !entry->second.ptr ) entry->second.ptr = factory;
            factoryInfoSetHelper( entry->second.type, type, "type", id );
            factoryInfoSetHelper( entry->second.rtype, rtype, "return type", id );
            factoryInfoSetHelper( entry->second.className, className, "class", id );
          }
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
          // add an alias for the factory using the Reflex convention
          std::string old_name = old_style_name( id );
          if ( id != old_name )
            add( old_name, factory, type, rtype, className, props ).properties["ReflexName"] = "true";
#endif
          return entry->second;
        }

        void* Registry::get( const std::string& id, const std::string& type ) const {
          REG_SCOPE_LOCK
          const FactoryMap& facts = factories();
          auto              f     = facts.find( id );
          if ( f != facts.end() ) {
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
            const Properties& props = f->second.properties;
            if ( props.find( "ReflexName" ) != props.end() )
              logger().warning( "requesting factory via old name '" + id +
                                "'"
                                "use '" +
                                f->second.className + "' instead" );
#endif
            if ( !f->second.ptr ) {
              if ( !dlopen( f->second.library.c_str(), RTLD_LAZY | RTLD_GLOBAL ) ) {
                logger().warning( "cannot load " + f->second.library + " for factory " + id );
                char* dlmsg = dlerror();
                if ( dlmsg ) logger().warning( dlmsg );
                return nullptr;
              }
              f = facts.find( id ); // ensure that the iterator is valid
            }
            if ( f->second.type == type ) return f->second.ptr;
            logger().warning( "found factory " + id + ", but of wrong type: " + demangle( f->second.type ) +
                              " instead of " + demangle( type ) );
          }
          return nullptr; // factory not found
        }

        const Registry::FactoryInfo& Registry::getInfo( const std::string& id ) const {
          REG_SCOPE_LOCK
          static const FactoryInfo unknown( "unknown" );
          const FactoryMap&        facts = factories();
          auto                     f     = facts.find( id );
          return ( f != facts.end() ) ? f->second : unknown;
        }

        Registry& Registry::addProperty( const std::string& id, const std::string& k, const std::string& v ) {
          REG_SCOPE_LOCK
          FactoryMap& facts = factories();
          auto        f     = facts.find( id );
          if ( f != facts.end() ) f->second.properties[k] = v;
          return *this;
        }

        std::set<Registry::KeyType> Registry::loadedFactoryNames() const {
          REG_SCOPE_LOCK
          std::set<KeyType> l;
          for ( const auto& f : factories() ) {
            if ( f.second.ptr ) l.insert( f.first );
          }
          return l;
        }

        void Logger::report( Level lvl, const std::string& msg ) {
          static const char* levels[] = {"DEBUG  : ", "INFO   : ", "WARNING: ", "ERROR  : "};
          if ( lvl >= level() ) { std::cerr << levels[lvl] << msg << std::endl; }
        }

        static auto s_logger = std::make_unique<Logger>();
        Logger&     logger() { return *s_logger; }
        void        setLogger( Logger* logger ) { s_logger.reset( logger ); }

      } // namespace Details

      void SetDebug( int debugLevel ) {
        using namespace Details;
        Logger& l = logger();
        if ( debugLevel > 1 )
          l.setLevel( Logger::Debug );
        else if ( debugLevel > 0 )
          l.setLevel( Logger::Info );
        else
          l.setLevel( Logger::Warning );
      }

      int Debug() {
        using namespace Details;
        switch ( logger().level() ) {
        case Logger::Debug:
          return 2;
        case Logger::Info:
          return 1;
        default:
          return 0;
        }
      }
    }
  } // namespace PluginService
} // namespace Gaudi
