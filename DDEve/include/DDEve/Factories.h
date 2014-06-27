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
#include "Reflex/PluginService.h"
#endif
#include "DDEve/View.h"
#include "DDEve/Display.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  class View;
  class Display;
  template <typename T> class ViewFactory {
  public:
    static View* create(Display* dis, const char* arg);
  };

}

namespace {

  template <typename P> class Factory<P, DD4hep::View*(DD4hep::Display* dis, const char*)> {
  public:
    static void Func(void *ret, void*, const std::vector<void*>& arg, void*) {
      *(void**)ret = new P((DD4hep::Display*)arg[0], (const char*) arg[1]);
    }
  };
}

#define DECLARE_VIEW_FACTORY(x) PLUGINSVC_FACTORY(x,DD4hep::View*(DD4hep::Display*, const char*))

#endif // DD4HEP_DDEVE_FACTORIES_H
