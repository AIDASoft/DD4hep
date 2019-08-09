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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>

#include <dlfcn.h>
#include <getopt.h>

#define GAUDI_PLUGIN_SERVICE_V2
#include <Gaudi/PluginService.h>
#include <Gaudi/PluginServiceV1.h>

void help( std::string argv0 ) {
  std::cout << "Usage: " << argv0
            << " [option] library1 [library2 ...]\n"
               "\n list the component factories present in the given libraries\n\n"
               "Options:\n\n"
               "  -h, --help       show this help message and exit\n"
               "  -o OUTPUT, --output OUTPUT\n"
               "                   write the list of factories on the file OUTPUT, use - for\n"
               "                   standard output (default)\n"
            << std::endl;
}

void usage( std::string argv0 ) {
  std::cout << "Usage: " << argv0
            << " [option] library1 [library2 ...]\n"
               "Try `"
            << argv0 << " -h' for more information.\n"
            << std::endl;
}

int main( int argc, char* argv[] ) {
  auto& reg2 = Gaudi::PluginService::v2::Details::Registry::instance();
  auto& reg1 = Gaudi::PluginService::v1::Details::Registry::instance();

  using key_type = Gaudi::PluginService::v2::Details::Registry::KeyType;

  // cache to keep track of the loaded factories
  std::map<key_type, std::string> loaded;
  // initialize the local cache
  for ( const auto& name : reg2.loadedFactoryNames() ) loaded.emplace( name, "<preloaded>" );
  for ( const auto& name : reg1.loadedFactoryNames() ) loaded.emplace( name, "<preloaded>" );

  // Parse command line
  std::list<char*> libs;
  std::string      output_opt( "-" );
  {
    std::string argv0( argv[0] );
    {
      auto i = argv0.rfind( '/' );
      if ( i != std::string::npos ) argv0 = argv0.substr( i + 1 );
    }

    int i = 1;
    while ( i < argc ) {
      const std::string arg( argv[i] );
      if ( arg == "-o" || arg == "--output" ) {
        if ( ++i < argc ) {
          output_opt = argv[i];
        } else {
          std::cerr << "ERROR: missing argument for option " << arg << std::endl;
          std::cerr << "See `" << argv0 << " -h' for more details." << std::endl;
          return EXIT_FAILURE;
        }
      } else if ( arg == "-h" || arg == "--help" ) {
        help( argv0 );
        return EXIT_SUCCESS;
      } else {
        libs.push_back( argv[i] );
      }
      ++i;
    }
    if ( libs.empty() ) {
      usage( argv0 );
      return EXIT_FAILURE;
    }
  }

  // handle output option
  std::unique_ptr<std::ostream> output_file;
  if ( output_opt != "-" ) { output_file.reset( new std::ofstream{output_opt} ); }
  std::ostream& output = ( output_file ? *output_file : std::cout );

  auto dump_from = [&output, &loaded]( auto& reg, const char* lib, const char* prefix ) {
    for ( const auto& factoryName : reg.loadedFactoryNames() ) {
      auto f = loaded.find( factoryName );
      if ( f == loaded.end() ) {
        output << prefix << "::" << lib << ":" << factoryName << std::endl;
        loaded.emplace( factoryName, lib );
      } else
        std::cerr << "WARNING: factory '" << factoryName << "' already found in " << f->second << std::endl;
    }
  };

  // loop over the list of libraries passed on the command line
  for ( const char* lib : libs ) {
    if ( dlopen( lib, RTLD_LAZY | RTLD_LOCAL ) ) {
      dump_from( reg2, lib, "v2" );
      dump_from( reg1, lib, "v1" );
    } else {
      std::cerr << "ERROR: failed to load " << lib << ": " << dlerror() << std::endl;
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
