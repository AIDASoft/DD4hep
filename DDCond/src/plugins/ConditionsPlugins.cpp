// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;

namespace {
  /// Plugin function:
  /// Install the alignment manager as an extension to the central LCDD object
  int ddcond_install_cond_mgr (LCDD& lcdd, int argc, char** argv)  {
    Handle<ConditionsManagerObject> mgr(lcdd.extension<ConditionsManagerObject>(false));
    if ( !mgr.isValid() )  {
      ConditionsManager mgr_handle(lcdd);
      lcdd.addExtension<ConditionsManagerObject>(mgr_handle.ptr());
      printout(INFO,"ConditionsManager",
               "+++ Successfully installed conditions manager instance to LCDD.");
      mgr = mgr_handle;
    }
    if ( argc == 2 )  {
      if ( ::strncmp(argv[0],"-handle",7)==0 )  {
        Handle<NamedObject>* h = (Handle<NamedObject>*)argv[1];
        *h = mgr;
      }
    }
    return 1;
  }
}
DECLARE_APPLY(DD4hep_ConditionsManagerInstaller,ddcond_install_cond_mgr)
// ======================================================================================

namespace {

  int ddcond_dump_conditions_functor(lcdd_t& lcdd, bool print_conditions, int argc, char** argv)   {
    typedef std::vector<const IOVType*> _T;
    typedef ConditionsIOVPool::Elements _E;
    typedef RangeConditions _R;
    ConditionsManager manager = ConditionsManager::from(lcdd);
    Condition::Processor* printer = 0;

    if ( argc > 0 )   {
      printer = (Condition::Processor*) argv[0];
    }

    const _T types = manager.iovTypesUsed();
    for( _T::const_iterator i = types.begin(); i != types.end(); ++i )    {
      const IOVType* type = *i;
      if ( type )   {
        ConditionsIOVPool* pool = manager.iovPool(*type);
        if ( pool )  {
          const _E& e = pool->elements;
          printout(INFO,"CondPoolDump","+++ ConditionsIOVPool for type %s  [%d IOV element%s]",
                   type->str().c_str(), int(e.size()),e.size()==1 ? "" : "s");
          for (_E::const_iterator j=e.begin(); j != e.end(); ++j)  {
            ConditionsPool* cp = (*j).second;
            cp->print("");
            if ( print_conditions )   {
              _R rc;
              cp->select_all(rc);
              for(_R::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)  {
                if ( printer )  {  (*printer)(*ic);                   }
                else            { /* print_conditions<void>(rc);  */  }
              }
            }
          }
        }
      }
    }
    printout(INFO,"CondPoolDump","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printout(INFO,"CondPoolDump","SUCCESS: +++ Conditions pools successfully dumped");
    printout(INFO,"CondPoolDump","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    return 1;
  }

  int ddcond_dump_pools(LCDD& lcdd, int argc, char** argv)   {
    return ddcond_dump_conditions_functor(lcdd,false, argc, argv);
  }
  int ddcond_dump_conditions(LCDD& lcdd, int argc, char** argv)   {
    return ddcond_dump_conditions_functor(lcdd,true, argc, argv);
  }
}

DECLARE_APPLY(DD4hep_ConditionsPoolDump,ddcond_dump_pools)
DECLARE_APPLY(DD4hep_ConditionsDump,ddcond_dump_conditions)
// ======================================================================================
#if 0
namespace {

  int ddcond_assign_keys(LCDD& lcdd, int /* argc */, char** /* argv */)   {
    typedef std::vector<const IOVType*> _T;
    typedef ConditionsIOVPool::Elements _E;
    typedef RangeConditions _R;
    ConditionsManager manager = ConditionsManager::from(lcdd);

    const _T types = manager.iovTypesUsed();
    for( _T::const_iterator i = types.begin(); i != types.end(); ++i )    {
      const IOVType* type = *i;
      if ( type )   {
        ConditionsIOVPool* pool = manager.iovPool(*type);
        if ( pool )  {
          const _E& e = pool->elements;
          for (_E::const_iterator j=e.begin(); j != e.end(); ++j)  {
            _R rc;
            ConditionsPool* cp = (*j).second;
            cp->select_all(rc);
            for(_R::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)  {
              Condition cond(*ic);
              
            }
          }
        }
      }
    }
    return 1;
  }
}

DECLARE_APPLY(DD4hep_AssignConditionsKeys,ddcond_assign_keys)
#endif
// ======================================================================================

namespace {
  /// Plugin entry point.
  static long ddcond_synchronize_conditions(lcdd_t& lcdd, int argc, char** argv) {
    if ( argc > 0 )   {
      string iov_type = argv[0];
      IOV::Key::first_type iov_key = *(IOV::Key::first_type*)argv[1];
      ConditionsManager    manager = ConditionsManager::from(lcdd);
      const IOVType*       epoch   = manager.iovType(iov_type);
      dd4hep_ptr<UserPool> user_pool;
      IOV  iov(epoch);

      iov.set(iov_key);
      long num_updated = manager.prepare(iov, user_pool);
      if ( iov_type == "epoch" )  {
        char   c_evt[64];
        struct tm evt;
        ::gmtime_r(&iov_key, &evt);
        ::strftime(c_evt,sizeof(c_evt),"%T %F",&evt);
        printout(INFO,"Conditions",
                 "+++ ConditionsUpdate: Updated %ld conditions... event time: %s",
                 num_updated, c_evt);
      }
      else  {
        printout(INFO,"Conditions",
                 "+++ ConditionsUpdate: Updated %ld conditions... key[%s]: %ld",
                 num_updated, iov_type.c_str(), iov_key);
      }
      user_pool->print("User pool");
      manager.clean(epoch, 20);
      user_pool->clear();
      return 1;
    }
    except("Conditions","+++ Failed update conditions. No event time argument given!");
    return 0;
  }
}
DECLARE_APPLY(DD4hep_ConditionsSynchronize,ddcond_synchronize_conditions)
// ======================================================================================

namespace {
  /// Plugin entry point.
  static long ddcond_clean_conditions(lcdd_t& lcdd, int argc, char** argv) {
    if ( argc > 0 )   {
      string iov_type = argv[0];
      int max_age = *(int*)argv[1];
      printout(INFO,"Conditions",
               "+++ ConditionsUpdate: Cleaning conditions... type:%s max age:%d",
               iov_type.c_str(), max_age);
      ConditionsManager manager = ConditionsManager::from(lcdd);
      const IOVType* epoch = manager.iovType(iov_type);
      manager.clean(epoch, max_age);
      return 1;
    }
    except("Conditions","+++ Failed cleaning conditions. Insufficient arguments!");
    return 0;
  }
}
DECLARE_APPLY(DD4hep_ConditionsClean,ddcond_clean_conditions)
// ======================================================================================

#include "DDCond/ConditionsRepository.h"
namespace {
  /// Plugin entry point.
  static long ddcond_create_repository(lcdd_t& lcdd, int argc, char** argv) {
    if ( argc > 0 )   {
      string output = argv[0];
      printout(INFO,"Conditions",
               "+++ ConditionsRepository: Creating %s",output.c_str());
      ConditionsManager manager = ConditionsManager::from(lcdd);
      ConditionsRepository().save(manager,output);
      return 1;
    }
    except("Conditions","+++ Failed creating conditions repository. Insufficient arguments!");
    return 0;
  }
}
DECLARE_APPLY(DD4hep_ConditionsCreateRepository,ddcond_create_repository)
// ======================================================================================

namespace {
  /// Plugin entry point.
  static long ddcond_dump_repository(lcdd_t& lcdd, int argc, char** argv) {
    if ( argc > 0 )   {
      typedef ConditionsRepository::Data Data;
      Data data;
      string input = argv[0];
      printout(INFO,"Conditions",
               "+++ ConditionsRepository: Dumping %s",input.c_str());
      ConditionsManager manager = ConditionsManager::from(lcdd);
      if ( ConditionsRepository().load(input, data) )  {
        printout(INFO,"Repository","%-8s  %-60s %-60s","Key","Name","Address");
        for(Data::const_iterator i=data.begin(); i!=data.end(); ++i)  {
          const ConditionsRepository::Entry& e = *i;
          string add = e.address;
          if ( add.length() > 80 ) add = e.address.substr(0,60) + "...";
          printout(INFO,"Repository","%08X  %s",e.key,e.name.c_str());
          printout(INFO,"Repository","          -> %s",e.address.c_str());
        }
      }
      return 1;
    }
    except("Conditions","+++ Failed dumping conditions repository. Insufficient arguments!");
    return 0;
  }
}
DECLARE_APPLY(DD4hep_ConditionsDumpRepository,ddcond_dump_repository)
// ======================================================================================

namespace {
  /// Plugin entry point.
  static long ddcond_load_repository(lcdd_t& lcdd, int argc, char** argv) {
    if ( argc > 0 )   {
      string input = argv[0];
      printout(INFO,"Conditions",
               "+++ ConditionsRepository: Loading %s",input.c_str());
      ConditionsManager manager = ConditionsManager::from(lcdd);
      ConditionsRepository::Data data;
      ConditionsRepository().load(input, data);
      return 1;
    }
    except("Conditions","+++ Failed loading conditions repository. Insufficient arguments!");
    return 0;
  }
}
DECLARE_APPLY(DD4hep_ConditionsLoadRepository,ddcond_load_repository)
// ======================================================================================
