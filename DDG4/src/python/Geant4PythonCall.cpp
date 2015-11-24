// Framework include files
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-11-03
//
//==========================================================================
// $Id$

// Framework include files
#include "DDG4Python/Geant4PythonCall.h"
#include "DDG4Python/DDPython.h"

// C/C++ include files
#include <stdexcept>
#include "Python.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor, initializes variables
Geant4PythonCall::Geant4PythonCall()
  : m_callable(0), m_arguments(0)
{
  DDPython::instance();
}

Geant4PythonCall::~Geant4PythonCall()   {
  DDPython::GILState state(0);
  DDPython::releaseObject(m_callable);
  DDPython::releaseObject(m_arguments);
}
 
/// Set the callback structures for callbacks with arguments
void Geant4PythonCall::set(PyObject* callable, PyObject* arguments)   {
  DDPython::GILState state(0);
  DDPython::assignObject(m_callable,callable);
  DDPython::assignObject(m_arguments,arguments);
}

/// Set the callback structures for callbacks without arguments
void Geant4PythonCall::set(PyObject* callable)   {
  DDPython::GILState state(0);
  DDPython::assignObject(m_callable,callable);
  DDPython::assignObject(m_arguments,0);
}

namespace DD4hep { namespace Simulation {

    /// Execute command in the python interpreter.
    template <typename RETURN> RETURN Geant4PythonCall::execute() const   {
      DDPython::GILState state(0);
      TPyReturn ret(DDPython::instance().callC(m_callable, m_arguments));
      return (RETURN)ret;
    }

    /// Execute command in the python interpreter.
    template <typename RETURN> RETURN Geant4PythonCall::execute(PyObject* method) const   {
      DDPython::GILState state(0);
      TPyReturn ret(DDPython::instance().callC(method,0));
      return (RETURN)ret;
    }

    /// Execute command in the python interpreter.
    template <typename RETURN> RETURN Geant4PythonCall::execute(PyObject* method, PyObject* args) const   {
      DDPython::GILState state(0);
      TPyReturn ret(DDPython::instance().callC(method,args));
      return (RETURN)ret;
    }
#define INSTANTIATE(X)                                                  \
    template X Geant4PythonCall::execute<X>() const;                      \
    template X Geant4PythonCall::execute<X>(PyObject* method) const;      \
    template X Geant4PythonCall::execute<X>(PyObject* method, PyObject* args) const

    INSTANTIATE(char);
    INSTANTIATE(short);
    INSTANTIATE(int);
    INSTANTIATE(long);
    INSTANTIATE(unsigned short);
    INSTANTIATE(unsigned int);
    INSTANTIATE(unsigned long);
    INSTANTIATE(float);
    INSTANTIATE(double);
    INSTANTIATE(char*);
    INSTANTIATE(const char*);
    INSTANTIATE(PyObject*);
    INSTANTIATE(void*);
  }}
