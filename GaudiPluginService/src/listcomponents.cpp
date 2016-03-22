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

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <list>
#include <memory>
#include <cstdlib>

#include <getopt.h>
#include <dlfcn.h>

#include <Gaudi/PluginService.h>

void help(std::string argv0) {
  std::cout << "Usage: " << argv0 << " [option] library1 [library2 ...]\n"
      "\n list the component factories present in the given libraries\n\n"
      "Options:\n\n"
      "  -h, --help       show this help message and exit\n"
      "  -o OUTPUT, --output OUTPUT\n"
      "                   write the list of factories on the file OUTPUT, use - for\n"
      "                   standard output (default)\n"
      << std::endl;
}

void usage(std::string argv0) {
  std::cout << "Usage: " << argv0 << " [option] library1 [library2 ...]\n"
      "Try `" << argv0 << " -h' for more information.\n"
      << std::endl;
}

int main(int argc, char* argv[]) {
  Gaudi::PluginService::Details::Registry &reg =
      Gaudi::PluginService::Details::Registry::instance();
  typedef Gaudi::PluginService::Details::Registry::KeyType key_type;

#if APPLE
#define DEBUG_FOR_MAC 1
#endif

#if DEBUG_FOR_MAC
  Gaudi::PluginService::SetDebug( 2 ) ;
#endif


  // cache to keep track of the loaded factories
  std::map<key_type, std::string> loaded;
  {
    // initialize the local cache
    std::set<key_type> base = reg.loadedFactories();
    for (std::set<key_type>::const_iterator f = base.begin(); f != base.end(); ++f)
    {
      loaded.insert(std::make_pair(*f, std::string("<preloaded>")));
    }
  }

#if DEBUG_FOR_MAC
  std::cout << " --- using registry : " << &reg  << "  --- with " << loaded.size() << " pre-loeaded libraries " << std::endl ;
#endif


  // Parse command line
  std::list<char*> libs;
  std::string output_opt("-");
  {
    std::string argv0(argv[0]);
    {
      auto i = argv0.rfind('/');
      if (i != std::string::npos)
        argv0 = argv0.substr(i+1);
    }

    int i = 1;
    while (i < argc) {
      const std::string arg(argv[i]);
      if (arg == "-o" || arg == "--output") {
        if (++i < argc) {
          output_opt = argv[i];
        } else {
          std::cerr << "ERROR: missing argument for option " << arg << std::endl;
          std::cerr << "See `" << argv0 << " -h' for more details." << std::endl;
          return EXIT_FAILURE;
        }
      } else if (arg == "-h" || arg == "--help") {
        help(argv0);
        return EXIT_SUCCESS;
      } else {
        libs.push_back(argv[i]);
      }
      ++i;
    }
    if (libs.empty()) {
      usage(argv0);
      return EXIT_FAILURE;
    }
  }

  // handle output option
  std::unique_ptr<std::ostream> output_file;
  if (output_opt != "-") {
    output_file = std::unique_ptr<std::ostream>(new std::ofstream(output_opt.c_str()));
  }
  std::ostream &output = (output_file ? *output_file : std::cout);

  // loop over the list of libraries passed on the command line
  for (char* lib: libs) {

    if (dlopen(lib, RTLD_LAZY | RTLD_GLOBAL)) {

      std::set<key_type> factories = reg.loadedFactories();
      std::set<key_type>::const_iterator f;

#if DEBUG_FOR_MAC
      std::cout << " --- registry : " << &reg  << "  --- has " << factories.size() << " factories " << std::endl ;
#endif

      for (f = factories.begin(); f != factories.end(); ++f) {
        if (loaded.find(*f) == loaded.end())
        {
          output << lib << ":" << *f << std::endl;
          loaded[*f] = lib;
        }
        else
          std::cerr << "WARNING: factory '" << *f
                    << "' already found in " << loaded[*f]
                    << std::endl;
      }

    } else {
      std::cerr << "ERROR: failed to load " << lib << std::endl;
      std::cerr << dlerror() <<std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
