#ifndef _GAUDI_PLUGIN_SERVICE_V1_H_
#define _GAUDI_PLUGIN_SERVICE_V1_H_
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

#include <Gaudi/Details/PluginServiceDetailsV1.h>
#include <string>
#include <typeinfo>
#include <utility>

#define _PS_V1_DECLARE_FACTORY_WITH_ID( type, id, factory )                                                            \
  _PS_V1_INTERNAL_DECLARE_FACTORY( type, id, factory, __LINE__ )

#define _PS_V1_DECLARE_FACTORY( type, factory )                                                                        \
  _PS_V1_DECLARE_FACTORY_WITH_ID( type, ::Gaudi::PluginService::v1::Details::demangle<type>(), factory )

#define _PS_V1_DECLARE_FACTORY_WITH_CREATOR_AND_ID( type, typecreator, id, factory )                                   \
  _PS_V1_INTERNAL_DECLARE_FACTORY_WITH_CREATOR( type, typecreator, id, factory, __LINE__ )

#define _PS_V1_DECLARE_FACTORY_WITH_CREATOR( type, typecreator, factory )                                              \
  _PS_V1_DECLARE_FACTORY_WITH_CREATOR_AND_ID( type, typecreator,                                                       \
                                              ::Gaudi::PluginService::v1::Details::demangle<type>(), factory )

#define _PS_V1_DECLARE_COMPONENT( type ) _PS_V1_DECLARE_FACTORY( type, type::Factory )

#define _PS_V1_DECLARE_COMPONENT_WITH_ID( type, id ) _PS_V1_DECLARE_FACTORY_WITH_ID( type, id, type::Factory )

#if !GAUDI_PLUGIN_SERVICE_USE_V2
#  define DECLARE_FACTORY_WITH_ID( type, id, factory ) _PS_V1_DECLARE_FACTORY_WITH_ID( type, id, factory )
#  define DECLARE_FACTORY( type, factory ) _PS_V1_DECLARE_FACTORY( type, factory )
#  define DECLARE_FACTORY_WITH_CREATOR_AND_ID( type, typecreator, id, factory )                                        \
    _PS_V1_DECLARE_FACTORY_WITH_CREATOR_AND_ID( type, typecreator, id, factory )
#  define DECLARE_FACTORY_WITH_CREATOR( type, typecreator, factory )                                                   \
    _PS_V1_DECLARE_FACTORY_WITH_CREATOR( type, typecreator, factory )
#  define DECLARE_COMPONENT( type ) _PS_V1_DECLARE_COMPONENT( type )
#  define DECLARE_COMPONENT_WITH_ID( type, id ) _PS_V1_DECLARE_COMPONENT_WITH_ID( type, id )
#endif

namespace Gaudi {
  namespace PluginService {
    GAUDI_PLUGIN_SERVICE_V1_INLINE namespace v1 {
      /// Class wrapping the signature for a factory with any number of arguments.
      template <typename R, typename... Args>
      class Factory {
      public:
        typedef R ReturnType;
        typedef R ( *FuncType )( Args&&... );

        static ReturnType create( const std::string& id, Args... args ) {
          const FuncType c = Details::getCreator<FuncType>( id );
          return c ? ( *c )( std::forward<Args>( args )... ) : 0;
        }

        template <typename T>
        static ReturnType create( const T& id, Args... args ) {
          std::ostringstream o;
          o << id;
          return create( o.str(), std::forward<Args>( args )... );
        }
      };

      class GAUDIPS_EXPORT Exception : public std::exception {
      public:
        Exception( std::string msg );
        ~Exception() throw() override;
        const char* what() const throw() override;

      private:
        std::string m_msg;
      };
    }
  } // namespace PluginService
} // namespace Gaudi

#endif //_GAUDI_PLUGIN_SERVICE_H_
