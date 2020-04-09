//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-07
//
//==========================================================================
#ifndef DD4HEP_DDG4_DDPYTHON_H 
#define DD4HEP_DDG4_DDPYTHON_H 1

// C/C++ include files
#include <string>

// ROOT include file(s)
#include "TPyReturn.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  struct DDPythonGlobalState;

  /// Python interface class for callbacks and GIL.
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class DDPython {
  protected:
    void* context;
    /// Standard constructor
    DDPython( ); 
  protected:
    static bool isMainThread();
  public:
    struct GILState {
      int state;
      explicit GILState(int);
      ~GILState();
    };
    struct BlockThreads {
      explicit BlockThreads(int);
      ~BlockThreads();
    };
    struct AllowThreads {
      explicit AllowThreads(int);
      ~AllowThreads();
    };

    /// Save thread state
    static void allowThreads();
    static void restoreThread();

    /// Object instantiator
    static DDPython instance();
    static void shutdown();
    static void setMainThread();

    /// Release python object
    static void releaseObject(PyObject*& obj);

    /// Release python object
    static void assignObject(PyObject*& obj, PyObject* new_obj);

    /// Start the interpreter in normal mode without hacks like 'python.exe' does.
    static int run_interpreter(int argc, char** argv);

    /// Copy constructor 
    DDPython(const DDPython& ) {}

    /// Destructor
    ~DDPython( );

    int  setArgs(int argc, char** argv)  const;
    int  runFile(const std::string& fname)  const;
    int  execute(const std::string& cmd)  const;
    int  evaluate(const std::string& cmd)  const;

    /// Call a python object with argument (typically a dictionary)
    /** Note:
     *  - Typical call from python -> C -> python
     *  - Errors are printed and then cleared. A return code of NULL is passed
     *    to the caller in the event of an error condition
     *  - The returned object is NOT owned by the caller.
     *  - No GIL state handling by the caller necessary
     */
    PyObject* call(PyObject* method, PyObject* args);

    /// Call a python object with argument (typically a dictionary). 
    /** Note:
     *  - Typical call from C -> python -> C
     *  - Errors are printed and then cleared. A return code of NULL is passed
     *    to the caller in the event of an error condition
     *  - The returned object TPyReturn must be destructed (ie. leave scope)
     *    BEFORE the GIL is released.
     *  - The caller MUST ensure the GIL state in case of multi-threading!
     */
    TPyReturn callC(PyObject* method, PyObject* args);

    /// Invoke command prompt
    void prompt()  const;

    /// Callback after forking.
    void afterFork()  const;

  private:

  };
}

#endif // DD4HEP_DDG4_DDPYTHON_H
