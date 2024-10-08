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

// C/C++ include files
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <pthread.h>

// Framework include files
#include <DD4hep/Printout.h>
#include <DDG4/Python/DDPython.h>

#if !defined(__MAKECINT__) && !defined(__CINT__) && !defined(G__DICTIONARY)
// -----------------------------------------------------------------------------
// Python hacks to avoid warnings if outside dictionaries .....
// -----------------------------------------------------------------------------
// --> /usr/include/python2.7/pyconfig.h:1161:0: warning: "_POSIX_C_SOURCE" redefined [enabled by default]
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif /* _POSIX_C_SOURCE */
// --> /usr/include/python2.7/pyconfig.h:1183:0: warning: "_XOPEN_SOURCE" redefined [enabled by default]
#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif /* _XOPEN_SOURCE */
#include <Python.h>
#endif

namespace {
  std::string loadScript(const std::string& fname) {
    std::ifstream file(fname.c_str());
    std::stringstream str;
    if( file ) {
      char ch;
      while( file.get(ch) ) str.put(ch);
      file.close();
      return str.str();
    }
    return "";
  }

  static dd4hep::DDPython* _instance = 0;
  static int       _blockers = 0;
  static pthread_t _mainThread = 0;
  static int       _refCount = 0;
  static PyObject* _main_dict = 0;
  static PyThreadState *_save_state = 0;
  int _execPy(const char* cmd)   {
    dd4hep::DDPython::GILState state(0);
    PyObject* ret = ::PyRun_String((char*)cmd, Py_file_input,_main_dict,_main_dict);
    if ( ::PyErr_Occurred() )   {
      ::PyErr_Print();
      ::PyErr_Clear();
      return 0;
    }
    if ( ret && ret == Py_None )  {
      Py_DECREF( ret );
      return 1;
    }
    else if ( ret )   {
      TPyReturn r(ret);
      Py_DECREF( ret );
      return (int)r;
    }
    return 0;
  }
  int _evalPy(const char* cmd)   {
    dd4hep::DDPython::GILState state(0);
    PyObject* ret = ::PyRun_String((char*)cmd, Py_eval_input,_main_dict,_main_dict);
    if ( ::PyErr_Occurred() )   {
      ::PyErr_Print();
      ::PyErr_Clear();
      return 0;
    }
    if ( ret && ret == Py_None )  {
      Py_DECREF( ret );
      return 1;
    }
    else if ( ret )   {
      TPyReturn r(ret);
      Py_DECREF( ret );
      return (int)r;
    }
    return 0;
  }
}

dd4hep::DDPython::GILState::GILState(int) : state(0) {
  if ( ::Py_IsInitialized() )  {
    PyGILState_STATE st = (PyGILState_STATE)::PyGILState_Ensure();
    state = (int)st;
  }
}

dd4hep::DDPython::GILState::~GILState()  {
  if ( ::Py_IsInitialized() )  {
    PyGILState_STATE st = (PyGILState_STATE)state;
    ::PyGILState_Release(st);
  }
}

dd4hep::DDPython::BlockThreads::BlockThreads(int)  {
  if ( _blockers == 0 )  {
    DDPython::restoreThread();
  }
  ++_blockers;
}

dd4hep::DDPython::BlockThreads::~BlockThreads()  {
  --_blockers;
  if ( _blockers == 0 )  {
    DDPython::allowThreads();
  }
}

dd4hep::DDPython::AllowThreads::AllowThreads(int)  {
  DDPython::allowThreads();
}

dd4hep::DDPython::AllowThreads::~AllowThreads()  {
  DDPython::restoreThread();
}

/// Standard constructor, initializes variables
dd4hep::DDPython::DDPython() : context(0)  {
  ++_refCount;
  bool inited = ::Py_IsInitialized();
  if ( !inited ) {
    ::Py_Initialize();
#if PY_MAJOR_VERSION <=2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 7)
    ::PyEval_InitThreads();
#endif
  }
  else  {
    _refCount += 1000; // Ensure we do not call Py_Finalize()!
  }
  if ( !_main_dict ) {
    // retrieve the main dictionary
    PyObject* module = ::PyImport_AddModule("__main__");
    if ( !module || ::PyErr_Occurred() )   {
      ::PyErr_Print();
      ::PyErr_Clear();
      dd4hep::printout(WARNING,"DDPython","Main dictionary pointer is NULL. Try to continue like this!");
    }
    else  {
      _main_dict = ::PyModule_GetDict(module);
      if ( _main_dict )  {
        Py_INCREF( _main_dict );
      }
      dd4hep::printout(DEBUG,"DDPython","Pointer to main dict:%p",(void*)_main_dict);
    }
    setMainThread();
  }
  //if ( !isMainThread() ) context = ::PyEval_SaveThread();
  if ( !_save_state )  {
    //_save_state = ::PyEval_SaveThread();
  }
}

/// Default Destructor
dd4hep::DDPython::~DDPython()   {
  --_refCount;
  if ( 0 == _refCount && ::Py_IsInitialized() ) {
    dd4hep::printout(ALWAYS,"DDPython","+++ Shutdown python interpreter......");
    if ( _main_dict )  {
      Py_DECREF(_main_dict);
      _main_dict = 0;
    }
    if ( _save_state )  {
      ::PyEval_RestoreThread(_save_state);
    }
    ::Py_Finalize();
    _instance = 0;
  }
} 


dd4hep::DDPython dd4hep::DDPython::instance()   {
  if ( 0 == _instance ) _instance = new DDPython();
  return DDPython();
}

/// Save thread state
void dd4hep::DDPython::allowThreads()   {
  if ( !_save_state && ::Py_IsInitialized() )  {
    _save_state = ::PyEval_SaveThread();
  }
}

void dd4hep::DDPython::restoreThread()   {
  if ( _save_state ) {
    ::PyEval_RestoreThread(_save_state);
    _save_state = 0;
  }
}

int dd4hep::DDPython::setArgs(int argc, char** argv)  const   {
  std::vector<std::wstring> wargs;
  std::vector<const wchar_t*> wargv;
  for(int i=0; i<argc;++i)  {
    std::wstring wstr;
    if ( argv[i] )  {
      const size_t size = strlen(argv[i]);
      if (size > 0) {
        wstr.resize(size+1);
        std::mbstowcs(&wstr[0], argv[i], size);
        wstr[size] = 0;
      }
    }
    wargs.push_back(wstr);
  }
  for(auto& s : wargs ) wargv.push_back(s.c_str());

  PyStatus status;

  PyConfig config;
  PyConfig_InitPythonConfig( &config );

  status = PyConfig_SetString( &config, &config.program_name, wargv[0] );
  status = PyConfig_SetArgv( &config, 1, (wchar_t**)&wargv[0] );
  status = Py_InitializeFromConfig( &config );
  PyConfig_Clear( &config );

  return 1;
}

void dd4hep::DDPython::shutdown()   {
  if ( 0 != _instance )  {
    if ( 1 == _refCount ) {
      delete _instance;
      _instance = 0;
    }
  }
}

int dd4hep::DDPython::runFile(const std::string& fname)  const   {
  std::string cmd = loadScript(fname);
  return execute(cmd);
}

int dd4hep::DDPython::evaluate(const std::string& cmd)  const   {
  return _evalPy(cmd.c_str());
}

int dd4hep::DDPython::execute(const std::string& cmd)  const   {
  return _execPy(cmd.c_str());
}

PyObject* dd4hep::DDPython::call(PyObject* method, PyObject* args)   {
  dd4hep::DDPython::GILState state(0);
  if ( PyCallable_Check(method) )   {
    PyObject* ret = ::PyObject_CallObject(method,args==Py_None ? NULL : args);
    return ret;
  }
  ::PyErr_SetString(PyExc_RuntimeError,"DDPython::call: The argument is not a callable object!");
  return 0;
}

TPyReturn dd4hep::DDPython::callC(PyObject* method, PyObject* args)   {
  if ( PyCallable_Check(method) )   {
    PyObject* arg = args==Py_None || args==0 ? 0 : args;
    PyObject* ret = ::PyObject_CallObject(method,arg);
    if ( ::PyErr_Occurred() )   {
      ::PyErr_Print();
      ::PyErr_Clear();
      return TPyReturn();
    }
    else if ( ret )   {
      return TPyReturn(ret);
    }
  }
  throw std::runtime_error("dd4hep::DDPython::callC: Object is not callable!");
}

/// Release python object
void dd4hep::DDPython::releaseObject(PyObject*& obj)   {
  if ( obj && ::Py_IsInitialized() )  {
    Py_DECREF(obj);
  }
  obj = 0;
}

/// Release python object
void dd4hep::DDPython::assignObject(PyObject*& obj, PyObject* new_obj)   {
  if ( ::Py_IsInitialized() )  {
    if ( obj ) { Py_DECREF(obj); }
    if ( new_obj ){ Py_INCREF(new_obj); }
  }
  obj = new_obj;
}

void dd4hep::DDPython::prompt()  const   {
  dd4hep::DDPython::GILState state(0);
  ::PyRun_InteractiveLoop(stdin,const_cast<char*>("\0"));
}

void dd4hep::DDPython::afterFork()  const  {
  if ( ::Py_IsInitialized() ) {
    std::cout << "[INFO] Re-init python after forking....." << std::endl;
#if PY_VERSION_HEX < 0x03070000
    ::PyOS_AfterFork();
#else
    ::PyOS_AfterFork_Child();
#endif
#if PY_MAJOR_VERSION <=2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 7)
    ::PyEval_InitThreads();
    ::PyEval_ReleaseLock();
#endif
  }
}

void dd4hep::DDPython::setMainThread()  {
  _mainThread = pthread_self();
}

bool dd4hep::DDPython::isMainThread()   {
  return _mainThread == pthread_self();
}

/// Start the interpreter in normal mode without hacks like 'pythopn.exe' does.
int dd4hep::DDPython::run_interpreter(int argc, char** argv)   {
  std::vector<std::wstring> wargs;
  std::vector<const wchar_t*> wargv;
  for(int i=0; i<argc;++i)  {
    std::wstring wstr;
    if ( argv[i] )  {
      const size_t size = strlen(argv[i]);
      if (size > 0) {
        wstr.resize(size+1);
        std::mbstowcs(&wstr[0], argv[i], size);
        wstr[size] = 0;
      }
    }
    wargs.push_back(wstr);
  }
  for( auto& s : wargs ) wargv.push_back(s.c_str());
  return ::Py_Main(argc, (wchar_t**)&wargv[0]);
}
