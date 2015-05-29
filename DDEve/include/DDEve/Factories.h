// $Id: Factories.h 1201 2014-06-02 18:21:45Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_FACTORIES_H
#define DD4HEP_DDEVE_FACTORIES_H

#ifndef __CINT__
#include "DD4hep/Plugins.h"
#endif
#include "DDEve/View.h"
#include "DDEve/Display.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class View;
  class Display;

  /// Factory template for view objects
  template <typename T> class ViewFactory {
  public:
    static View* create(Display* dis, const char* arg);
  };

}

namespace {

  /// Base factory template
  template <typename P, typename S> class Factory;

  template <typename P> class Factory<P, DD4hep::View*(DD4hep::Display* dis, const char*)> {
  public:
    static void Func(void *ret, void*, const std::vector<void*>& arg, void*) {
      *(void**)ret = new P((DD4hep::Display*)arg[0], (const char*) arg[1]);
    }
  };
}

#define DECLARE_VIEW_FACTORY(x) DD4HEP_PLUGINSVC_FACTORY(x,DD4hep_DDEve_##x,DD4hep::View*(DD4hep::Display*, const char*),__LINE__)

#endif // DD4HEP_DDEVE_FACTORIES_H
