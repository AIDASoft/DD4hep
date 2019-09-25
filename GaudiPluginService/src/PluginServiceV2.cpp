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

#define GAUDI_PLUGIN_SERVICE_V2
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

#ifdef _GNU_SOURCE
#  include <cstring>
#  include <dlfcn.h>
#endif

#ifdef USE_BOOST_FILESYSTEM
#  include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#  include <filesystem>
namespace fs = std::filesystem;
#endif // USE_BOOST_FILESYSTEM

#if __cplusplus >= 201703 || (__clang__ && __APPLE__)
#  include <string_view>
#else
#  include <experimental/string_view>
namespace std {
  using experimental::string_view;
}
#endif

#define REG_SCOPE_LOCK std::lock_guard<std::recursive_mutex> _guard( m_mutex );

namespace {
  std::mutex registrySingletonMutex;
}
#define SINGLETON_LOCK std::lock_guard<std::mutex> _guard( ::registrySingletonMutex );

#include <algorithm>

namespace {
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
    GAUDI_PLUGIN_SERVICE_V2_INLINE namespace v2 {
      namespace Details {
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

        void reportBadAnyCast( const std::type_info& factory_type, const std::string& id ) {
          if ( logger().level() <= Logger::Debug ) {
            std::stringstream msg;
            const auto&       info = Registry::instance().getInfo( id );
            msg << "bad any_cast: requested factory " << id << " of type " << demangle( factory_type ) << ", got ";
            if ( info.is_set() )
              msg << demangle( info.factory.type() ) << " from " << info.library;
            else
              msg << "nothing";
            logger().debug( msg.str() );
          }
        }

        Registry::Properties::mapped_type Registry::FactoryInfo::getprop( const Properties::key_type& name ) const {
          auto p = properties.find( name );
          return ( p != end( properties ) ) ? p->second : Properties::mapped_type{};
        }

        Registry::Registry() {}

        void Registry::initialize() {
          REG_SCOPE_LOCK
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

          std::regex  line_format{"^(?:[[:space:]]*(?:(v[0-9]+)::)?([^:]+):(.*[^[:space:]]))?[[:space:]]*(?:#.*)?$"};
          std::smatch m;

          std::string search_path = std::getenv( envVar );
          if ( !search_path.empty() ) {
            logger().debug( std::string( "searching factories in " ) + envVar );

            std::string::size_type start_pos = 0, end_pos = 0;
            while ( start_pos != std::string::npos ) {
              // correctly handle begin of string or path separator
              if ( start_pos ) ++start_pos;

              end_pos = search_path.find( sep, start_pos );
              if ( end_pos == start_pos )   {
                start_pos = std::string::npos;
                continue;
              }
              fs::path dirName =
#ifdef USE_BOOST_FILESYSTEM
                  std::string{search_path.substr( start_pos, end_pos - start_pos )};
#else
                  search_path.substr( start_pos, end_pos - start_pos );
#endif
              start_pos = end_pos;

              logger().debug( " looking into " + dirName.string() );
              // look for files called "*.components" in the directory
              if ( is_directory( dirName ) ) {
                for ( auto& p : fs::directory_iterator( dirName ) ) {
                  if ( p.path().extension() == ".components" && is_regular_file( p.path() ) ) {
                    // read the file
                    const auto& fullPath = p.path().string();
                    logger().debug( "  reading " + p.path().filename().string() );
                    std::ifstream factories{fullPath};
                    std::string   line;
                    int           factoriesCount = 0;
                    int           lineCount      = 0;
                    while ( !factories.eof() ) {
                      ++lineCount;
                      std::getline( factories, line );
                      if ( regex_match( line, m, line_format ) ) {
                        if ( m[1] == "v2" ) { // ignore non "v2" and "empty" lines
                          const std::string lib{m[2]};
                          const std::string fact{m[3]};
                          m_factories.emplace( fact, FactoryInfo{lib, {}, {{"ClassName", fact}}} );
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
                          // add an alias for the factory using the Reflex convention
                          std::string old_name = old_style_name( fact );
                          if ( fact != old_name ) {
                            m_factories.emplace( old_name,
                                                 FactoryInfo{lib, {}, {{"ReflexName", "true"}, {"ClassName", fact}}} );
                          }
#endif
                          ++factoriesCount;
                        }
                      } else {
                        logger().warning( "failed to parse line " + fullPath + ':' + std::to_string( lineCount ) );
                      }
                    }
                    if ( logger().level() <= Logger::Debug ) {
                      logger().debug( "  found " + std::to_string( factoriesCount ) + " factories" );
                    }
                  }
                }
              }
            }
          }
        }

        const Registry::FactoryMap& Registry::factories() const {
          std::call_once( m_initialized, &Registry::initialize, const_cast<Registry*>( this ) );
          return m_factories;
        }

        Registry::FactoryMap& Registry::factories() {
          std::call_once( m_initialized, &Registry::initialize, this );
          return m_factories;
        }

        Registry::FactoryInfo& Registry::add( const KeyType& id, FactoryInfo info ) {
          REG_SCOPE_LOCK
          FactoryMap& facts = factories();

#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
          // add an alias for the factory using the Reflex convention
          const auto old_name = old_style_name( id );
          if ( id != old_name ) {
            auto new_info = info;

            new_info.properties["ReflexName"] = "true";

            add( old_name, new_info );
          }
#endif

          auto entry = facts.find( id );
          if ( entry == facts.end() ) {
            // this factory was not known yet
            entry = facts.emplace( id, std::move( info ) ).first;
          } else {
            // do not replace an existing factory with a new one
            if ( !entry->second.is_set() ) entry->second = std::move( info );
          }
          return entry->second;
        }

        const Registry::FactoryInfo& Registry::getInfo( const KeyType& id, const bool load ) const {
          REG_SCOPE_LOCK
          static const FactoryInfo unknown = {"unknown"};
          const FactoryMap&        facts   = factories();
          auto                     f       = facts.find( id );

          if ( f != facts.end() ) {
            if ( load && !f->second.is_set() ) {
              const std::string library = f->second.library;
              if ( !dlopen( library.c_str(), RTLD_LAZY | RTLD_GLOBAL ) ) {
                logger().warning( "cannot load " + library + " for factory " + id );
                char* dlmsg = dlerror();
                if ( dlmsg ) logger().warning( dlmsg );
                return unknown;
              }
              f = facts.find( id ); // ensure that the iterator is valid
            }
            return f->second;
          } else {
            return unknown;
          }
        }

        Registry& Registry::addProperty( const KeyType& id, const KeyType& k, const std::string& v ) {
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
            if ( f.second.is_set() ) l.insert( f.first );
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

        // This chunk of code was taken from GaudiKernel (genconf) DsoUtils.h
        std::string getDSONameFor( void* fptr ) {
#ifdef _GNU_SOURCE
          Dl_info info;
          if ( dladdr( fptr, &info ) == 0 ) return "";

          auto pos = std::strrchr( info.dli_fname, '/' );
          if ( pos )
            ++pos;
          else
            return info.dli_fname;
          return pos;
#else
          return "";
#endif
        }
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
