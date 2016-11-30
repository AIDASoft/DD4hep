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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/DetectorProcessor.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsRepository.h"
#include "DDCond/ConditionsManagerObject.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Geometry::DetElement;
using Geometry::PlacedVolume;

/// Plugin function: Install the alignment manager as an extension to the central LCDD object
/**
 *  Factory: DD4hep_ConditionsManagerInstaller
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_install_cond_mgr (LCDD& lcdd, int argc, char** argv)  {
  Handle<NamedObject>* h = 0;
  Handle<ConditionsManagerObject> mgr(lcdd.extension<ConditionsManagerObject>(false));
  if ( !mgr.isValid() )  {
    bool arg_error = false;
    string factory = "DD4hep_ConditionsManager_Type1";
    for(int i=0; i<argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-type",argv[i],4) )
        factory = argv[++i];
      else if ( 0 == ::strncmp("-handle",argv[i],5) )
        h = (Handle<NamedObject>*)argv[++i];
      else
        arg_error = true;
    }
    if ( arg_error )   {
      /// Help printout describing the basic command line interface
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                             \n"
        "     name:   factory name     DD4hep_ConditionsManagerInstaller               \n"
        "     -type   <string>         Manager type.                                   \n"
        "                              Default: ConditionsManagerObject_Type1_t        \n"
        "     -handle <pointer>        Pointer to Handle<NamedObject> to pass pointer  \n"
        "                              to the caller.                                  \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    ConditionsManagerObject* obj = createPlugin<ConditionsManagerObject>(factory,lcdd);
    if ( !obj )  {
      except("ConditionsManagerInstaller","Failed to create manager object of type %s",
             factory.c_str());
    }
    lcdd.addExtension<ConditionsManagerObject>(obj);
    printout(INFO,"ConditionsManager",
             "+++ Successfully installed conditions manager instance '%s' to LCDD.",
             factory.c_str());
    mgr = obj;
  }
  else if ( argc > 0 )  {
    for(int i=0; i<argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-handle",argv[i],5) )   {
        h = (Handle<NamedObject>*)argv[++i];
        break;
      }
    }
  }
  if ( h ) *h = mgr;
  return 1;
}
DECLARE_APPLY(DD4hep_ConditionsManagerInstaller,ddcond_install_cond_mgr)

/**
 *  Prepare the conditions manager for execution
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static ConditionsSlice* ddcond_prepare(lcdd_t& lcdd, const string& iov_typ, long iov_val, int argc, char** argv)  {
  const IOVType*    iovtype  = 0;
  long              iovvalue = iov_val;
  ConditionsManager manager  = ConditionsManager::from(lcdd);

  for(int i=0; i<argc; ++i)  {
    if ( ::strncmp(argv[i],"-iov_type",7) == 0 )
      iovtype = manager.iovType(argv[++i]);
    else if ( ::strncmp(argv[i],"-iov_value",7) == 0 )
      iovvalue = ::atol(argv[++i]);
  }
  if ( 0 == iovtype )
    iovtype = manager.iovType(iov_typ);
  if ( 0 == iovtype )
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  if ( 0 > iovvalue )
    except("ConditionsPrepare",
           "++ Unknown IOV value supplied for iov type %s.",iovtype->str().c_str());

  IOV iov(iovtype,iovvalue);
  dd4hep_ptr<ConditionsSlice> slice(Conditions::createSlice(manager,*iovtype));
  manager.prepare(iov, *slice);
  printout(INFO,"Conditions",
           "+++ ConditionsUpdate: Collected %ld conditions of type %s [iov-value:%ld].",
           long(slice->size()), iovtype ? iovtype->str().c_str() : "???", iovvalue);
  return slice.release();
}

// ======================================================================================
/// Plugin function: Dump of all Conditions pool with or without conditions
/**
 *  Factory: DD4hep_ConditionsPoolProcessor
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_conditions_pool_processor(lcdd_t& lcdd, bool process_pool, bool process_conditions, int argc, char** argv)   {
  DetElement::Processor* p = createProcessor<DetElement::Processor>(lcdd,argc,argv);
  ConditionsProcessor* processor = dynamic_cast<ConditionsProcessor*>(p);
  typedef std::vector<const IOVType*> _T;
  typedef ConditionsIOVPool::Elements _E;
  typedef RangeConditions _R;
  dd4hep_ptr<Condition::Processor> proc(processor);
  ConditionsManager manager = ConditionsManager::from(lcdd);
  const _T types = manager.iovTypesUsed();
  for( _T::const_iterator i = types.begin(); i != types.end(); ++i )    {
    const IOVType* type = *i;
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        const _E& e = pool->elements;
        if ( process_pool )  {
          printout(INFO,"CondPoolProcessor","+++ ConditionsIOVPool for type %s  [%d IOV element%s]",
                   type->str().c_str(), int(e.size()),e.size()==1 ? "" : "s");
        }
        for (_E::const_iterator j=e.begin(); j != e.end(); ++j)  {
          ConditionsPool* cp = (*j).second;
          if ( process_pool )  {
            cp->print("");
          }
          if ( process_conditions )   {
            _R rc;
            cp->select_all(rc);
            for(_R::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)  {
              if ( proc.get() )  {  (*proc)(*ic); }
            }
          }
        }
      }
    }
  }
  return 1;
}
static int ddcond_conditions_pool_process(LCDD& lcdd, int argc, char** argv)   {
  return ddcond_conditions_pool_processor(lcdd, false, true, argc, argv);
}
DECLARE_APPLY(DD4hep_ConditionsPoolProcessor,ddcond_conditions_pool_process)

// ======================================================================================
/// Plugin function: Dump of all Conditions pool with or without conditions
/**
 *  Factory: DD4hep_ConditionsPoolDump: Dump pools only
 *  Factory: DD4hep_ConditionsDump: Dump pools and conditions
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_conditions_pool_print(lcdd_t& lcdd, bool print_conditions, int argc, char** argv)   {
  if ( argc > 0 )   {
    for(int i=0; i<argc; ++i)  {
      if ( argv[i] && 0 == ::strncmp(argv[i],"-processor",3) )  {
        vector<char*> args;
        for(int j=i; j<argc && argv[j] && 0 != ::strncmp(argv[i],"-end-processor",8); ++j)
          args.push_back(argv[j]);
        args.push_back(0);
        return ddcond_conditions_pool_processor(lcdd,true,print_conditions,int(args.size()-1),&args[0]);
      }
    }
    printout(WARNING,"DDCondProcessor","++ Found arguments in plugin call, "
             "but could not make any sense of them....");
  }
  const void* args[] = { "-processor", "DD4hep_ConditionsPrinter", 0};
  return ddcond_conditions_pool_processor(lcdd,true,print_conditions,2,(char**)args);
}

static int ddcond_dump_pools(LCDD& lcdd, int argc, char** argv)   {
  return ddcond_conditions_pool_print(lcdd, false, argc, argv);
}
static int ddcond_dump_conditions(LCDD& lcdd, int argc, char** argv)   {
  return ddcond_conditions_pool_print(lcdd, true, argc, argv);
}
DECLARE_APPLY(DD4hep_ConditionsPoolDump,ddcond_dump_pools)
DECLARE_APPLY(DD4hep_ConditionsDump,ddcond_dump_conditions)

// ======================================================================================
/// Plugin function: Dump of all Conditions associated to the detector elements
/**
 *  Factory: DD4hep_DetElementConditionsDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_detelement_dump(LCDD& lcdd, int argc, char** argv)   {

  /// Internal class to perform recursive printout
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/12/2016
   */
  struct Actor : public Geometry::DetectorProcessor   {
    /// Object printer object
    ConditionsPrinter           printer;
    /// Standard constructor
    Actor() {    }
    /// Default destructor
    virtual ~Actor()   {    }
    /// Dump method.
    virtual int operator()(DetElement de,int level)   {
      const DetElement::Children& children = de.children();
      PlacedVolume place = de.placement();
      char sens = place.volume().isSensitive() ? 'S' : ' ';
      char fmt[128], tmp[32];
      ::snprintf(tmp,sizeof(tmp),"%03d/",level+1);
      ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s #Dau:%%d VolID:%%08X %%c",level+1,2*level+1);
      printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(children.size()),
               (unsigned long)de.volumeID(), sens);
      printer.setName(string(tmp)+de.name());
      if ( de.hasConditions() )  {
        printer.processElement(de);
      }
      return 1;
    }
  } actor;
  dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(lcdd,"run",1500,argc,argv));
  UserPool* pool = slice->pool().get();
  pool->print("User pool");
  actor.printer.setPool(pool);
  int ret = actor.process(lcdd.world(),0,true);
  slice->manager.clean(slice->iov().iovType, 20);
  return ret;
}
DECLARE_APPLY(DD4hep_DetElementConditionsDump,ddcond_detelement_dump)
  
// ======================================================================================
/// Plugin function: Dump of all Conditions associated to the detector elements
/**
 *  Factory: DD4hep_DetElementConditionsDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static void* ddcond_prepare_plugin(LCDD& lcdd, int argc, char** argv)   {
  dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(lcdd,"",-1,argc,argv));
  UserPool* p = slice->pool().get();
  return p && p->size() > 0 ? slice.release() : 0;
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsPrepare,ddcond_prepare_plugin)
  
// ======================================================================================
/// Plugin function: Dump of all Conditions associated to the detector elements
/**
 *  Factory: DD4hep_DetElementConditionsDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_detelement_processor(LCDD& lcdd, int argc, char** argv)   {

  /// Internal class to perform recursive printout
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/12/2016
   */
  struct Actor : public Geometry::DetElementProcessor<ConditionsProcessor>  {
    /// Standard constructor
    Actor(ConditionsProcessor* p) : DetElementProcessor<ConditionsProcessor>(p){}

    /// Default destructor
    virtual ~Actor()   {    }
    /// Dump method.
    virtual int operator()(DetElement de, int)
    {  return de.hasConditions() ? processor->processElement(de) : 1;    }
  };
  ConditionsProcessor* processor = 0;
  if ( argc > 0 )   {
    processor = createProcessor<ConditionsProcessor>(lcdd, argc, argv);
  }
  else  {
    const void* args[] = { "-processor", "DD4hepConditionsPrinter", 0};
    processor = createProcessor<ConditionsProcessor>(lcdd, 2, (char**)args);
  }
  dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(lcdd,"run",1500,argc,argv));
  UserPool* pool = slice->pool().get();
  Actor actor(processor);
  pool->print("User pool");
  processor->setPool(pool);
  int ret = Actor(processor).process(lcdd.world(),0,true);
  slice->manager.clean(pool->validity().iovType, 20);
  return ret;
}
DECLARE_APPLY(DD4hep_DetElementConditionsProcessor,ddcond_detelement_processor)

// ======================================================================================
/// Plugin entry point: Synchronize conditions according to new IOV value
/**
 *  Factory: DD4hep_ConditionsSynchronize
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_synchronize_conditions(lcdd_t& lcdd, int argc, char** argv) {
  if ( argc >= 2 )   {
    string iov_typ = argv[0];
    IOV::Key::first_type iov_key = *(IOV::Key::first_type*)argv[1];
    dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(lcdd,iov_typ,iov_key,argc,argv));
    UserPool* pool = slice->pool().get();
    pool->print("User pool");
    slice->manager.clean(pool->validity().iovType, 20);
    pool->clear();
    return 1;
  }
  except("Conditions","+++ Failed update conditions. Arguments were: '%s'",
         arguments(argc,argv).c_str());
  return 0;
}
DECLARE_APPLY(DD4hep_ConditionsSynchronize,ddcond_synchronize_conditions)

// ======================================================================================
/// Plugin entry point: Clean conditions reposiory according to maximum age
/**
 *  Factory: DD4hep_ConditionsClean
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_clean_conditions(lcdd_t& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    string iov_type = argv[0];
    int    max_age  = *(int*)argv[1];
    printout(INFO,"Conditions",
             "+++ ConditionsUpdate: Cleaning conditions... type:%s max age:%d",
             iov_type.c_str(), max_age);
    ConditionsManager manager = ConditionsManager::from(lcdd);
    const IOVType* iov_typ = manager.iovType(iov_type);
    manager.clean(iov_typ, max_age);
    return 1;
  }
  except("Conditions","+++ Failed cleaning conditions. Insufficient arguments!");
  return 0;
}
DECLARE_APPLY(DD4hep_ConditionsClean,ddcond_clean_conditions)

// ======================================================================================
/// Basic entry point to instantiate the basic DD4hep conditions/alignmants printer
/**
 *  Factory: DD4hepConditionsPrinter, DD4hepAlignmentsPrinter 
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    17/11/2016
 */
#include "DD4hep/PluginTester.h"
template <typename PRINTER>
static void* create_printer(Geometry::LCDD& lcdd, int argc,char** argv)  {
  typedef typename PRINTER::pool_type pool_t;
  string prefix = "", name = "";
  int    flags = 0, have_pool = 0, arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-prefix",argv[i],4) )
      prefix = argv[++i];
    else if ( 0 == ::strncmp("-name",argv[i],5) )
      name = argv[++i];
    else if ( 0 == ::strncmp("-flags",argv[i],5) )
      flags = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-pool",argv[i],5) )
      have_pool = 1;
    else
      arg_error = true;
  }
  if ( arg_error )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name(s)  DD4hep_ConditionsPrinter,                       \n"
      "                              DD4hep_AlignmentsPrinter                        \n"
      "                              DD4hep_AlignedVolumePrinter                     \n"
      "     -prefix <string>         Printout prefix for user customized output.     \n"
      "     -flags  <number>         Printout processing flags.                      \n"
      "     -pool                    Attach conditions user pool from                \n"
      "                              PluginTester instance attached to LCDD.       \n\n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  DetElement world = lcdd.world();
  printout(INFO,"Printer","World=%s [%p]",world.path().c_str(),world.ptr());
  PRINTER* p = (flags) ? new PRINTER(prefix,flags) : new PRINTER(prefix);
  if ( have_pool != 0 )  {
    PluginTester* test = lcdd.extension<PluginTester>();
    pool_t* pool = test->extension<pool_t>("ConditionsTestUserPool");
    if ( !name.empty() ) p->name = name;
    p->setPool(pool);
  }
  return (void*)dynamic_cast<DetElement::Processor*>(p);
}
#include "DD4hep/ConditionsPrinter.h"
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsPrinter,create_printer<Conditions::ConditionsPrinter>)
#include "DD4hep/AlignmentsPrinter.h"
DECLARE_LCDD_CONSTRUCTOR(DD4hep_AlignmentsPrinter,create_printer<Alignments::AlignmentsPrinter>)
#include "DD4hep/AlignedVolumePrinter.h"
DECLARE_LCDD_CONSTRUCTOR(DD4hep_AlignedVolumePrinter,create_printer<Alignments::AlignedVolumePrinter>)

// ======================================================================================
/// Plugin entry point: Create repository csv file from loaded conditions
/**
 *  Factory: DD4hep_ConditionsCreateRepository
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_create_repository(lcdd_t& lcdd, int argc, char** argv) {
  bool arg_error = false;
  string output = "";
  for(int i=0; i<argc && argv[i]; ++i)  {      
    if ( 0 == ::strncmp("-output",argv[i],4) )
      output = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || output.empty() )  {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_ConditionsCreateRepository             \n\n"
      "     -output <string>         Output file name.                             \n\n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  printout(INFO,"Conditions",
           "+++ ConditionsRepository: Creating %s",output.c_str());
  ConditionsManager manager = ConditionsManager::from(lcdd);
  ConditionsRepository().save(manager,output);
  return 1;
}
DECLARE_APPLY(DD4hep_ConditionsCreateRepository,ddcond_create_repository)

// ======================================================================================
/// Plugin entry point: Dump conditions repository csv file
/**
 *  Factory: DD4hep_ConditionsDumpRepository
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_dump_repository(lcdd_t& /* lcdd */, int argc, char** argv)   {
  typedef ConditionsRepository::Data Data;
  bool arg_error = false;
  string input = "";
  Data data;
  for(int i=0; i<argc && argv[i]; ++i)  {      
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )  {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_ConditionsDumpRepository               \n\n"
      "     -input <string>          Input file name.                              \n\n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  printout(INFO,"Conditions","+++ ConditionsRepository: Dumping %s",input.c_str());
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
DECLARE_APPLY(DD4hep_ConditionsDumpRepository,ddcond_dump_repository)

// ======================================================================================
/// Plugin entry point: Load conditions repository csv file into conditions manager
/**
 *  Factory: DD4hep_ConditionsDumpRepository
 *

TO BE DONE!!!

 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_load_repository(lcdd_t& /* lcdd */, int argc, char** argv) {
  if ( argc > 0 )   {
    string input = argv[0];
    printout(INFO,"Conditions","+++ ConditionsRepository: Loading %s",input.c_str());
    ConditionsRepository::Data data;
    ConditionsRepository().load(input, data);
    return 1;
  }
  except("Conditions","+++ Failed loading conditions repository. Insufficient arguments!");
  return 0;
}
DECLARE_APPLY(DD4hep_ConditionsLoadRepository,ddcond_load_repository)
// ======================================================================================
