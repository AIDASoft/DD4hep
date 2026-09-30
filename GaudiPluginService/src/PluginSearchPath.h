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
#ifndef GAUDIPLUGINSERVICE_SRC_PLUGINSEARCHPATH_H
#define GAUDIPLUGINSERVICE_SRC_PLUGINSEARCHPATH_H

#include <sys/stat.h>

#include <cstdlib>
#include <set>
#include <string>

// Only included by the PluginService sources in this directory
namespace {
  /// The directories searched for ".components" files.
  struct SearchPath {
    /// The environment variable(s) the path was read from, for the debug output
    std::string variables;
    /// Directories separated by `separator`
    std::string path;
    char        separator;
  };

  /// Return the search path for the ".components" files.
  ///
  /// On macOS, System Integrity Protection removes DYLD_LIBRARY_PATH from the
  /// environment of every process started through a protected binary such as
  /// /bin/sh or /usr/bin/env, i.e. of every script with a "#!/usr/bin/env python"
  /// shebang. So next to DYLD_LIBRARY_PATH also search DD4HEP_LIBRARY_PATH (which
  /// thisdd4hep.sh sets for exactly this reason) and LD_LIBRARY_PATH, which SIP
  /// leaves alone. A directory listed in several of them is searched only once, at
  /// its first appearance.
  inline SearchPath pluginSearchPath() {
    const std::string defaultPath = "/usr/lib64:/usr/lib:/usr/local/lib";
#if defined( _WIN32 )
    const char* envPtr = std::getenv( "PATH" );
    return {"PATH", envPtr ? envPtr : defaultPath, ';'};
#elif defined( __APPLE__ )
    const char  sep       = ':';
    const char* envVars[] = {"DD4HEP_LIBRARY_PATH", "DYLD_LIBRARY_PATH", "LD_LIBRARY_PATH"};
    SearchPath  result{"", "", sep};
    std::set<std::string> seen;
    bool anySet = false;
    for ( const char* envVar : envVars ) {
      result.variables += ( result.variables.empty() ? "" : ", " ) + std::string( envVar );
      const char* envPtr = std::getenv( envVar );
      if ( !envPtr ) continue;
      anySet = true;
      const std::string value( envPtr );
      std::string::size_type pos = 0;
      while ( pos <= value.size() ) {
        auto end = value.find( sep, pos );
        if ( end == std::string::npos ) end = value.size();
        const std::string dir = value.substr( pos, end - pos );
        if ( !dir.empty() && seen.insert( dir ).second ) {
          if ( !result.path.empty() ) result.path += sep;
          result.path += dir;
        }
        pos = end + 1;
      }
    }
    if ( !anySet ) result.path = defaultPath;
    return result;
#else
    const char* envPtr = std::getenv( "LD_LIBRARY_PATH" );
    return {"LD_LIBRARY_PATH", envPtr ? envPtr : defaultPath, ':'};
#endif
  }

  /// Return how to dlopen the library `lib`, listed in a ".components" file found in
  /// the directory `dir`.
  ///
  /// On macOS a bare library name is resolved through DYLD_LIBRARY_PATH, which SIP
  /// may have removed (see pluginSearchPath), so there a library that sits next to
  /// its ".components" file is loaded by its full path. Everywhere else, and for
  /// libraries living elsewhere, the name is returned unchanged.
  inline std::string componentLibrary( const std::string& dir, const std::string& lib ) {
#if defined( __APPLE__ )
    if ( !dir.empty() && lib.find( '/' ) == std::string::npos ) {
      const std::string full = dir + '/' + lib;
      struct stat       buf;
      if ( 0 == ::stat( full.c_str(), &buf ) && S_ISREG( buf.st_mode ) ) return full;
    }
#else
    (void)dir;
#endif
    return lib;
  }
} // namespace

#endif // GAUDIPLUGINSERVICE_SRC_PLUGINSEARCHPATH_H
