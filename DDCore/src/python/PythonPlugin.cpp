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

// Framework include files
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"

// ROOT includes
#include "TPython.h"

using namespace std;
using namespace dd4hep;

/// Do not clutter the global namespace ....
namespace  {
  
  void usage(int argc, char** argv)    {
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                                  \n"
      "     name:   factory name     DD4hep_Python                                        \n"
      "     -import  <string>        import a python module, making its classes available.\n"
      "     -macro   <string>        load a python script as if it were a macro.          \n"
      "     -exec    <string>        execute a python statement (e.g. \"import ROOT\".    \n"
      "     -eval    <string>        evaluate a python expression (e.g. \"1+1\")          \n"
      "     -prompt                  enter an interactive python session (exit with ^D)   \n"
      "     -dd4hep                  Equivalent to -exec \"import dd4hep\"                \n"
      "     -help                    Show this online help message.                       \n"
      "                                                                                   \n"
      "     Note: entries can be given multiple times and are executed in exactly the     \n"
      "           order specified at the command line!                                    \n"
      "                                                                                   \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  /// ROOT GDML writer plugin
  /**
   *  Factory: DD4hep_ROOTGDMLExtract
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  long call_python(Detector& /* description */, int argc, char** argv) {
    if ( argc > 0 )   {
      vector<pair<string, string> > commands;
      for(int i = 0; i < argc && argv[i]; ++i)  {
        if ( 0 == ::strncmp("-import",argv[i],2) )
          commands.push_back(make_pair("import",argv[++i]));
        else if ( 0 == ::strncmp("-dd4hep", argv[i],2) )
          commands.push_back(make_pair("exec","import dd4hep"));
        else if ( 0 == ::strncmp("-macro", argv[i],2) )
          commands.push_back(make_pair("macro",argv[++i]));
        else if ( 0 == ::strncmp("-exec", argv[i],2) )
          commands.push_back(make_pair("exec",argv[++i]));
        else if ( 0 == ::strncmp("-eval", argv[i],2) )
          commands.push_back(make_pair("calc",argv[++i]));
        else if ( 0 == ::strncmp("-prompt", argv[i],2) )
          commands.push_back(make_pair("prompt",""));
        else
          usage(argc, argv);
      }
      if ( commands.empty() )   {
        usage(argc, argv);
      }
      for(const auto& c : commands)   {
        Bool_t result = kFALSE;
        switch(c.first[0])  {
        case 'i':
          result = TPython::Import(c.second.c_str());
          break;
        case 'm':
          TPython::LoadMacro(c.second.c_str());
          result = kTRUE;
          break;
        case 'e':
          result = TPython::Exec(c.second.c_str());
          break;
        case 'c':
          TPython::Eval(c.second.c_str());
          result = kTRUE;
          break;
        case 'p':
          TPython::Prompt();
          result = kTRUE;
          break;
        default:
          usage(argc, argv);
          ::exit(EINVAL);
          break;
        }
        if ( result != kTRUE )    {
          except("DD4hep_Python","+++ Failed to invoke the statement: %s",c.second.c_str());
        }
      }
      return 1;
    }
    except("DD4hep_Python","+++ No commands file name given.");
    return 0;
  }
}

DECLARE_APPLY(DD4hep_Python,call_python)
