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

#include <Gaudi/PluginService.h>

#include <dlfcn.h>
#include <dirent.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>

#include <cxxabi.h>
#include <sys/stat.h>

#if  defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L 
#define REG_SCOPE_LOCK \
  std::lock_guard<std::recursive_mutex> _guard(m_mutex);


//namespace Gaudi { namespace PluginService { namespace Details { class Registry ; } } }

namespace {
  std::mutex registrySingletonMutex;
  //  Gaudi::PluginService::Details::Registry* _theRegistry ;
}
#define SINGLETON_LOCK \
  std::lock_guard<std::mutex> _guard(::registrySingletonMutex);
#else
#define REG_SCOPE_LOCK
#define SINGLETON_LOCK
#endif

// string trimming functions taken from
// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
#include <algorithm>
// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace)))
                                       .base(),
                s.end());
        return s;
}
// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

namespace {
  /// Helper function used to set values in FactoryInfo data members only
  /// if the original value is empty and reporting warnings in case of
  /// inconsistencies.
  inline void factoryInfoSetHelper(std::string& dest, const std::string& value,
                                   const std::string& desc,
                                   const std::string& id) {
    if (dest.empty()) {
      dest = value;
    } else if (dest != value) {
      std::ostringstream o;
      o << "new factory loaded for '" << id << "' with different "
        << desc << ": " << dest << " != " << value;
      Gaudi::PluginService::Details::logger().warning(o.str());
    }
  }

  struct OldStyleCnv {
    std::string name;
    void operator() (const char c) {
      switch(c) {
      case '<':
      case '>':
      case ',':
      case '(':
      case ')':
      case ':':
      case '.':
        name.push_back('_'); break;
      case '&':
        name.push_back('r'); break;
      case '*':
        name.push_back('p'); break;
      case ' ': break;
      default:
        name.push_back(c); break;
      }
    }
  };
  /// Convert a class name in the string used with the Reflex plugin service
  std::string old_style_name(const std::string& name) {
    return std::for_each(name.begin(), name.end(), OldStyleCnv()).name;
  }
}

namespace Gaudi { namespace PluginService {

  Exception::Exception(const std::string& msg): m_msg(msg) {}
  Exception::~Exception() throw() {}
  const char*  Exception::what() const throw() {
    return m_msg.c_str();
  }

  namespace Details {
    void* getCreator(const std::string& id, const std::string& type) {
      return Registry::instance().get(id, type);
    }

    std::string demangle(const std::string& id) {
      int   status;
      char* realname;
      realname = abi::__cxa_demangle(id.c_str(), 0, 0, &status);
      if (realname == 0) return id;
      std::string result(realname);
      free(realname);
      return result;
    }
    std::string demangle(const std::type_info& id) {
      return demangle(id.name());
    }

    // Registry& Registry::instance() {
    //   SINGLETON_LOCK
    // 	if( ::_theRegistry == 0 ){
    // 	  ::_theRegistry = new Registry ;
    // 	}
    //   return * ::_theRegistry ;
    // }

    Registry& Registry::instance() {
      SINGLETON_LOCK
	static Registry r;
      return r;
    }

    Registry::Registry(): m_initialized(false) {}

    void Registry::initialize() {
      REG_SCOPE_LOCK
      if (m_initialized) return;
      m_initialized = true;
#ifdef WIN32
      const char* envVar = "PATH";
      const char sep = ';';
#else
#ifdef APPLE
      const char* envVar = "DYLD_LIBRARY_PATH";
      const char sep = ':';
#else
      const char* envVar = "LD_LIBRARY_PATH";
      const char sep = ':';
#endif
#endif
      char *search_path = ::getenv(envVar);
      if (search_path) {
        logger().debug(std::string("searching factories in ") + envVar);
        std::string path(search_path);
        std::string::size_type pos = 0;
        std::string::size_type newpos = 0;
        while (pos != std::string::npos) {
          std::string dirName;
          // get the next entry in the path
          newpos = path.find(sep, pos);
          if (newpos != std::string::npos) {
            dirName = path.substr(pos, newpos - pos);
            pos = newpos+1;
          } else {
            dirName = path.substr(pos);
            pos = newpos;
          }
          logger().debug(std::string(" looking into ") + dirName);
          // look for files called "*.components" in the directory
          DIR *dir = opendir(dirName.c_str());
          if (dir) {
            struct dirent * entry;
            while ((entry = readdir(dir))) {
              std::string name(entry->d_name);
              // check if the file name ends with ".components"
              std::string::size_type extpos = name.find(".components");
              if ((extpos != std::string::npos) &&
                  ((extpos+11) == name.size())) {
                std::string fullPath = (dirName + '/' + name);
                { // check if it is a regular file
                  struct stat buf;
                  stat(fullPath.c_str(), &buf);
                  if (!S_ISREG(buf.st_mode)) continue;
                }
                // read the file
                logger().debug(std::string("  reading ") + name);
                std::ifstream facts(fullPath.c_str());
                std::string line;
                int factoriesCount = 0;
                int lineCount = 0;
                while (!facts.eof()) {
                  ++lineCount;
                  std::getline(facts, line);
                  trim(line);
                  // skip empty lines and lines starting with '#'
                  if (line.empty() || line[0] == '#') continue;
                  // look for the separator
                  std::string::size_type other_pos = line.find(':');
                  if (other_pos == std::string::npos) {
                    std::ostringstream o;
                    o << "failed to parse line " << fullPath
                      << ':' << lineCount;
                    logger().warning(o.str());
                    continue;
                  }
                  const std::string lib(line, 0, other_pos);
                  const std::string fact(line, other_pos+1);
                  m_factories.insert(std::make_pair(fact, FactoryInfo(lib)));
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
                  // add an alias for the factory using the Reflex convention
                  std::string old_name = old_style_name(fact);
                  if (fact != old_name) {
                    FactoryInfo old_info(lib);
                    old_info.properties["ReflexName"] = "true";
                    m_factories.insert(std::make_pair(old_name, old_info));
                  }
#endif
                  ++factoriesCount;
                }
                if (logger().level() <= Logger::Debug) {
                  std::ostringstream o;
                  o << "  found " << factoriesCount << " factories";
                  logger().debug(o.str());
                }
              }
            }
            closedir(dir);
          }
        }
      }
    }

    Registry::FactoryInfo&
    Registry::add(const std::string& id, void *factory,
                  const std::string& type, const std::string& rtype,
                  const std::string& className,
                  const Properties& props){
      REG_SCOPE_LOCK
      FactoryMap &facts = factories();
      FactoryMap::iterator entry = facts.find(id);
      if (entry == facts.end())
      {
        // this factory was not known yet
        entry = facts.insert(std::make_pair(id,
                                            FactoryInfo("unknown", factory,
                                                        type, rtype, className, props))).first;
#if DEBUG_FOR_MAC
	std::cout << " --  registering factory id: " << id << " class : " << className << " with registry " << this << std::endl ;
#endif

      } else {
        // do not replace an existing factory with a new one
        if (!entry->second.ptr) {
          entry->second.ptr = factory;
        }
        factoryInfoSetHelper(entry->second.type, type, "type", id);
        factoryInfoSetHelper(entry->second.rtype, rtype, "return type", id);
        factoryInfoSetHelper(entry->second.className, className, "class", id);
      }
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
      // add an alias for the factory using the Reflex convention
      std::string old_name = old_style_name(id);
      if (id != old_name)
        add(old_name, factory, type, rtype, className, props)
          .properties["ReflexName"] = "true";
#endif
      return entry->second;
    }

    void* Registry::get(const std::string& id, const std::string& type) const {
      REG_SCOPE_LOCK
      const FactoryMap &facts = factories();
      FactoryMap::const_iterator f = facts.find(id);
      if (f != facts.end())
      {
#ifdef GAUDI_REFLEX_COMPONENT_ALIASES
        const Properties& props = f->second.properties;
        if (props.find("ReflexName") != props.end())
          logger().warning("requesting factory via old name '" + id + "'"
                           "use '" + f->second.className + "' instead");
#endif
        if (!f->second.ptr) {
          if (!dlopen(f->second.library.c_str(), RTLD_LAZY | RTLD_GLOBAL)) {
            logger().warning("cannot load " + f->second.library +
                             " for factory " + id);
            char *dlmsg = dlerror();
            if (dlmsg)
              logger().warning(dlmsg);
            return 0;
          }
          f = facts.find(id); // ensure that the iterator is valid
        }
        if (f->second.type == type) {
          return f->second.ptr;
        } else {
          logger().warning("found factory " + id + ", but of wrong type: " +
              demangle(f->second.type) + " instead of " + demangle(type));
        }
      }
      return 0; // factory not found
    }

    const Registry::FactoryInfo& Registry::getInfo(const std::string& id) const {
      REG_SCOPE_LOCK
      static FactoryInfo unknown("unknown");
      const FactoryMap &facts = factories();
      FactoryMap::const_iterator f = facts.find(id);
      if (f != facts.end())
      {
        return f->second;
      }
      return unknown; // factory not found
    }

    Registry&
    Registry::addProperty(const std::string& id,
                          const std::string& k,
                          const std::string& v) {
      REG_SCOPE_LOCK
      FactoryMap &facts = factories();
      FactoryMap::iterator f = facts.find(id);
      if (f != facts.end())
      {
        f->second.properties[k] = v;
      }
      return *this;
    }

    std::set<Registry::KeyType> Registry::loadedFactories() const {
      REG_SCOPE_LOCK
      const FactoryMap &facts = factories();
      std::set<KeyType> l;
      for (FactoryMap::const_iterator f = facts.begin();
           f != facts.end(); ++f)
      {
        if (f->second.ptr)
          l.insert(f->first);
      }
      return l;
    }

    void Logger::report(Level lvl, const std::string& msg) {
      if (lvl >= level()) {
        static const char* levels[] = {"DEBUG  : ",
                                       "INFO   : ",
                                       "WARNING: ",
                                       "ERROR  : "};
        std::cerr << levels[lvl] << msg << std::endl;
      }
    }

    static std::auto_ptr<Logger> s_logger(new Logger);
    Logger& logger() {
      return *s_logger;
    }
    void setLogger(Logger* logger) {
      s_logger.reset(logger);
    }

  } // namespace Details

  void SetDebug(int debugLevel) {
    using namespace Details;
    Logger& l = logger();
    if (debugLevel > 1)
      l.setLevel(Logger::Debug);
    else if (debugLevel > 0)
      l.setLevel(Logger::Info);
    else l.setLevel(Logger::Warning);
  }

  int Debug() {
    using namespace Details;
    switch (logger().level()) {
    case Logger::Debug: return 2; break;
    case Logger::Info: return 1; break;
    default: return 0;
    }
  }

}} // namespace Gaudi::PluginService
