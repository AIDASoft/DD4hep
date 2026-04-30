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
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Printout.h>

// ROOT includes
#include <TPython.h>

// C++ includes
#include <cstdlib>
#include <regex>
#include <sstream>

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
          commands.emplace_back("import",argv[++i]);
        else if ( 0 == ::strncmp("-dd4hep", argv[i],2) )
          commands.emplace_back("exec","import dd4hep");
        else if ( 0 == ::strncmp("-macro", argv[i],2) )
          commands.emplace_back("macro",argv[++i]);
        else if ( 0 == ::strncmp("-exec", argv[i],2) )
          commands.emplace_back("exec",argv[++i]);
        else if ( 0 == ::strncmp("-eval", argv[i],2) )
          commands.emplace_back("calc",argv[++i]);
        else if ( 0 == ::strncmp("-prompt", argv[i],2) )
          commands.emplace_back("prompt","");
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
          // we do not care about the result
          TPython::Exec(c.second.c_str());
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

  // ---------------------------------------------------------------------------
  // DD4hep_PythonDetector — delegate geometry construction to a Python function.
  //
  // Compact XML usage:
  //   <detector id="X_ID" name="X" type="DD4hep_PythonDetector"
  //             module="my_package.barrel_geo"
  //             function="create_detector"
  //             readout="XHits"
  //             pythonpath="/path/to/package">  <!-- ${ENV_VAR} expanded -->
  //     ...
  //   </detector>
  //
  // The Python function must have the signature:
  //   def create_detector(description, xml_element, sens) -> dd4hep.DetElement
  //
  // C++ objects are passed as addresses (integers); reconstruct them in Python
  // using cppyy.bind_object(addr, typename).  The dd4hep_geo_plugin helper
  // module (shipped with DD4hep) handles this automatically.
  // ---------------------------------------------------------------------------

  /// Expand ${VAR} and $VAR patterns using std::getenv.
  static std::string expand_env(const std::string& input) {
    static const std::regex env_re(R"(\$\{([^}]+)\}|\$([A-Za-z_][A-Za-z0-9_]*))");
    std::string result;
    result.reserve(input.size());
    auto it  = std::sregex_iterator(input.begin(), input.end(), env_re);
    auto end = std::sregex_iterator();
    std::size_t last_pos = 0;
    for (; it != end; ++it) {
      const auto& m = *it;
      result.append(input, last_pos, m.position() - last_pos);
      const std::string var = m[1].matched ? m[1].str() : m[2].str();
      const char* val = std::getenv(var.c_str());
      if (val) result.append(val);
      last_pos = m.position() + m.length();
    }
    result.append(input, last_pos, std::string::npos);
    return result;
  }

  /// Escape a C++ string into a Python single-quoted string literal.
  /// Handles backslashes, quotes, newlines, and non-printable bytes.
  static std::string py_str(const std::string& value) {
    std::string result = "'";
    for (unsigned char c : value) {
      switch (c) {
        case '\\': result += "\\\\"; break;
        case '\'': result += "\\'";  break;
        case '\n': result += "\\n";  break;
        case '\r': result += "\\r";  break;
        case '\t': result += "\\t";  break;
        default:
          if (c < 0x20 || c == 0x7f) {
            char buf[8];
            snprintf(buf, sizeof(buf), "\\x%02x", c);
            result += buf;
          } else {
            result += static_cast<char>(c);
          }
      }
    }
    result += "'";
    return result;
  }

  /// DetElement returned from Python; swapped back to C++ via SwapWithObjAtAddr.
  static DetElement s_python_detector_result;

  /**
   *  Factory: DD4hep_PythonDetector
   *
   *  Reads `module`, `function`, and optionally `pythonpath` from the XML
   *  <detector> element, then calls the specified Python function to build the
   *  geometry.  The returned DetElement is transferred back to C++ via
   *  ROOT::Internal::SwapWithObjAtAddr.
   *
   *  \author  DD4hep developers
   */
  Ref_t create_python_detector(Detector& description, xml_h e, SensitiveDetector sens) {
    xml_det_t x_det(e);
    const std::string factory_name = "DD4hep_PythonDetector";

    auto get_attr = [&](const char* name) {
      return x_det.attr<std::string>(dd4hep::xml::Strng_t(name));
    };
    auto has_attr = [&](const char* name) {
      return x_det.hasAttr(dd4hep::xml::Strng_t(name));
    };

    const std::string mod  = get_attr("module");
    const std::string func = get_attr("function");
    const std::string pypath = has_attr("pythonpath")
                                 ? expand_env(get_attr("pythonpath")) : "";

    printout(DEBUG, factory_name,
             "+++ Loading Python module '%s', function '%s'", mod.c_str(), func.c_str());

    // Addresses of the C++ objects, passed as integers to Python.
    const std::uintptr_t desc_addr = reinterpret_cast<std::uintptr_t>(&description);
    const std::uintptr_t xml_addr  = reinterpret_cast<std::uintptr_t>(static_cast<xml_h*>(&e));
    const std::uintptr_t sens_addr = reinterpret_cast<std::uintptr_t>(&sens);
    const std::uintptr_t res_addr  = reinterpret_cast<std::uintptr_t>(&s_python_detector_result);

    std::ostringstream script;
    // Prepend pythonpath to sys.path when specified.
    if (!pypath.empty()) {
      script << "import sys\n"
             << "if " << py_str(pypath) << " not in sys.path:\n"
             << "    sys.path.insert(0, " << py_str(pypath) << ")\n";
    }
    script << "import importlib, cppyy\n"
           << "_dd4hep_py_mod = importlib.import_module(" << py_str(mod) << ")\n"
           << "_dd4hep_py_desc = cppyy.bind_object(" << desc_addr << ", 'dd4hep::Detector')\n"
           << "_dd4hep_py_xml  = cppyy.bind_object(" << xml_addr  << ", 'dd4hep::xml::Handle_t')\n"
           << "_dd4hep_py_sens = cppyy.bind_object(" << sens_addr << ", 'dd4hep::SensitiveDetector')\n"
           << "try:\n"
           << "    _dd4hep_py_det = getattr(_dd4hep_py_mod, " << py_str(func) << ")"
           << "(_dd4hep_py_desc, _dd4hep_py_xml, _dd4hep_py_sens)\n"
           << "except Exception:\n"
           << "    import traceback; traceback.print_exc()\n"
           << "    raise\n"
           << "cppyy.gbl.ROOT.Internal.SwapWithObjAtAddr['dd4hep::DetElement']"
           << "(_dd4hep_py_det, " << res_addr << ")\n";

    if (!TPython::Exec(script.str().c_str())) {
      except(factory_name, "+++ Python function %s.%s failed", mod.c_str(), func.c_str());
    }
    return s_python_detector_result;
  }
}

DECLARE_APPLY(DD4hep_Python,call_python)
DECLARE_DETELEMENT(DD4hep_PythonDetector,create_python_detector)

