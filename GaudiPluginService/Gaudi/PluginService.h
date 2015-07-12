#ifndef _GAUDI_PLUGIN_SERVICE_H_
#define _GAUDI_PLUGIN_SERVICE_H_
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
/// @see @ref GaudiPluginService-readme

#include <string>
#include <typeinfo>
#include <utility>
#include <Gaudi/Details/PluginServiceDetails.h>

#define DECLARE_FACTORY_WITH_ID(type, id, factory) \
  _INTERNAL_DECLARE_FACTORY(type, id, factory, __LINE__)

#define DECLARE_FACTORY(type, factory) \
  DECLARE_FACTORY_WITH_ID(type, \
      ::Gaudi::PluginService::Details::demangle<type>(), factory)

#define DECLARE_FACTORY_WITH_CREATOR_AND_ID(type, typecreator, id, factory) \
  _INTERNAL_DECLARE_FACTORY_WITH_CREATOR(type, typecreator, id, factory, __LINE__)

#define DECLARE_FACTORY_WITH_CREATOR(type, typecreator, factory) \
  DECLARE_FACTORY_WITH_CREATOR_AND_ID(type, typecreator, \
      ::Gaudi::PluginService::Details::demangle<type>(), factory)

#define DECLARE_COMPONENT(type) \
  DECLARE_FACTORY(type, type::Factory)

#define DECLARE_COMPONENT_WITH_ID(type, id) \
  DECLARE_FACTORY_WITH_ID(type, id, type::Factory)

namespace Gaudi { namespace PluginService {

#if !defined(__REFLEX__) || defined(ATLAS)
  /// Class wrapping the signature for a factory with any number of arguments.
  template <typename R, typename... Args>
  class Factory {
  public:
    typedef R  ReturnType;
    typedef R (*FuncType)(Args&&...);

    static ReturnType create(const std::string& id, Args... args) {
      const FuncType c = Details::getCreator<FuncType>(id);
      return c ? (*c)(std::forward<Args>(args)...) : 0;
    }

    template <typename T>
    static ReturnType create(const T& id, Args... args) {
      std::ostringstream o; o << id;
      return create(o.str(), std::forward<Args>(args)...);
    }
  };
#endif

  class GAUDIPS_EXPORT Exception: public std::exception {
  public:
    Exception(const std::string& msg);
    virtual ~Exception() throw();
    virtual const char* what() const throw();
  private:
    std::string m_msg;
  };
}}

#endif //_GAUDI_PLUGIN_SERVICE_H_
