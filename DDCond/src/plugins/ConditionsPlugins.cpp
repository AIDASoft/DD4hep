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
#include "DD4hep/Detector.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/ConditionsProcessor.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsRepository.h"
#include "DDCond/ConditionsManagerObject.h"
#include <memory>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

/// Plugin function: Install the alignment manager as an extension to the central Detector object
/**
 *  Factory: dd4hep_ConditionsManagerInstaller
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_install_cond_mgr (Detector& description, int argc, char** argv)  {
  Handle<NamedObject>* h = 0;
  Handle<ConditionsManagerObject> mgr(description.extension<ConditionsManagerObject>(false));
  if ( !mgr.isValid() )  {
    bool arg_error = false;
    string factory = "dd4hep_ConditionsManager_Type1";
    for(int i = 0; i < argc && argv[i]; ++i)  {
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
        "     name:   factory name     dd4hep_ConditionsManagerInstaller               \n"
        "     -type   <string>         Manager type.                                   \n"
        "                              Default: ConditionsManagerObject_Type1_t        \n"
        "     -handle <pointer>        Pointer to Handle<NamedObject> to pass pointer  \n"
        "                              to the caller.                                  \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    ConditionsManagerObject* obj = createPlugin<ConditionsManagerObject>(factory,description);
    if ( !obj )  {
      except("ConditionsManagerInstaller","Failed to create manager object of type %s",
             factory.c_str());
    }
    description.addExtension<ConditionsManagerObject>(obj);
    printout(INFO,"DDCond",
             "+++ Successfully installed conditions manager instance '%s' to Detector.",
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
DECLARE_APPLY(dd4hep_ConditionsManagerInstaller,ddcond_install_cond_mgr)

/**
 *  Prepare the conditions manager for execution
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static ConditionsSlice* ddcond_prepare(Detector& description, const string& iov_typ, long iov_val, int argc, char** argv)  {
  const IOVType*    iovtype  = 0;
  long              iovvalue = iov_val;
  ConditionsManager manager  = ConditionsManager::from(description);

  for(int i = 0; i < argc; ++i)  {
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
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  unique_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  cond::fill_content(manager,*content,*iovtype);
  manager.prepare(iov, *slice);
  printout(INFO,"Conditions",
           "+++ ConditionsUpdate: Collected %ld conditions of type %s [iov-value:%ld].",
           long(slice->size()), iovtype ? iovtype->str().c_str() : "???", iovvalue);
  return slice.release();
}

// ======================================================================================
/// Plugin function: Dump of all Conditions pool with or without conditions
/**
 *  Factory: dd4hep_ConditionsPoolProcessor
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_conditions_pool_processor(Detector& description, bool process_pool, bool process_conditions, int argc, char** argv)   {
  unique_ptr<Condition::Processor> proc(createProcessor<Condition::Processor>(description,argc,argv));
  ConditionsManager manager = ConditionsManager::from(description);
  const auto types = manager.iovTypesUsed();

  if ( !proc.get() )  {
    printout(WARNING,"Conditions","+++ Conditions processor of type %s is invalid!",argv[0]);
  }
  if ( process_conditions && !proc.get() )  {
    except("Conditions","+++ Conditions processor of type %s is invalid!",argv[0]);
  }
  for( const IOVType* type : types )  {
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        const ConditionsIOVPool::Elements& e = pool->elements;
        if ( process_pool )  {
          printout(INFO,"CondPoolProcessor","+++ ConditionsIOVPool for type %s  [%d IOV element%s]",
                   type->str().c_str(), int(e.size()),e.size()==1 ? "" : "s");
        }
        for ( const auto& cp : e )  {
          if ( process_pool )  {
            cp.second->print("");
          }
          if ( process_conditions )   {
            RangeConditions rc;
            cp.second->select_all(rc);
            for( auto c : rc )  {
              if ( proc.get() )  {  (*proc)(c); }
            }
          }
        }
      }
    }
  }
  return 1;
}
static int ddcond_conditions_pool_process(Detector& description, int argc, char** argv)   {
  return ddcond_conditions_pool_processor(description, false, true, argc, argv);
}
DECLARE_APPLY(dd4hep_ConditionsPoolProcessor,ddcond_conditions_pool_process)

// ======================================================================================
/// Plugin function: Dump of all Conditions pool with or without conditions
/**
 *  Factory: dd4hep_ConditionsPoolDump: Dump pools only
 *  Factory: dd4hep_ConditionsDump: Dump pools and conditions
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_conditions_pool_print(Detector& description, bool print_conditions, int argc, char** argv)   {
  if ( argc > 0 )   {
    for(int i = 0; i < argc; ++i)  {
      if ( argv[i] && 0 == ::strncmp(argv[i],"-processor",3) )  {
        vector<char*> args;
        for(int j=i; j<argc && argv[j] && 0 != ::strncmp(argv[i],"-end-processor",8); ++j)
          args.push_back(argv[j]);
        args.push_back(0);
        return ddcond_conditions_pool_processor(description,true,print_conditions,int(args.size()-1),&args[0]);
      }
    }
    printout(WARNING,"DDCondProcessor","++ Found arguments in plugin call, "
             "but could not make any sense of them....");
  }
  const void* args[] = { "-processor", "dd4hep_ConditionsPrinter", 0};
  return ddcond_conditions_pool_processor(description,true,print_conditions,2,(char**)args);
}

static int ddcond_dump_pools(Detector& description, int argc, char** argv)   {
  return ddcond_conditions_pool_print(description, false, argc, argv);
}
static int ddcond_dump_conditions(Detector& description, int argc, char** argv)   {
  return ddcond_conditions_pool_print(description, true, argc, argv);
}
DECLARE_APPLY(dd4hep_ConditionsPoolDump,ddcond_dump_pools)
DECLARE_APPLY(dd4hep_ConditionsDump,ddcond_dump_conditions)

// ======================================================================================
/// Plugin function: Dump of all Conditions associated to the detector elements
/**
 *  Factory: dd4hep_DetElementConditionsDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_detelement_dump(Detector& description, int argc, char** argv)   {

  /// detaill class to perform recursive printout
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/12/2016
   */
  struct Actor : public DetectorProcessor   {
    ConditionsPrinter& printer;
    /// Standard constructor
    Actor(ConditionsPrinter& p) : printer(p)  {    }
    /// Default destructor
    virtual ~Actor()   {    }
    /// Dump method.
    virtual int operator()(DetElement de,int level)  const  {
      const DetElement::Children& children = de.children();
      PlacedVolume place = de.placement();
      char sens = place.volume().isSensitive() ? 'S' : ' ';
      char fmt[128], tmp[32];
      ::snprintf(tmp,sizeof(tmp),"%03d/",level+1);
      ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds %%s #Dau:%%d VolID:%%08X %%c",level+1,2*level+1);
      printout(INFO,"DetectorDump",fmt,"",de.path().c_str(),int(children.size()),
               (unsigned long)de.volumeID(), sens);
      printer.prefix = string(tmp)+de.name();
      (printer)(de, level);
      return 1;
    }
  };
  dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(description,"run",1500,argc,argv));
  ConditionsPrinter printer(slice.get(),"");
  UserPool* pool = slice->pool.get();
  pool->print("User pool");
  Actor actor(printer);
  int ret = actor.process(description.world(),0,true);
  slice->manager.clean(pool->validity().iovType, 20);
  return ret;
}
DECLARE_APPLY(dd4hep_DetElementConditionsDump,ddcond_detelement_dump)
  
// ======================================================================================
/// Plugin function: Dump of all Conditions associated to the detector elements
/**
 *  Factory: dd4hep_DetElementConditionsDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static void* ddcond_prepare_plugin(Detector& description, int argc, char** argv)   {
  dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(description,"",-1,argc,argv));
  UserPool* p = slice->pool.get();
  return p && p->size() > 0 ? slice.release() : 0;
}
DECLARE_Detector_CONSTRUCTOR(dd4hep_ConditionsPrepare,ddcond_prepare_plugin)
#if 0
// ======================================================================================
/// Plugin function: Dump of all Conditions associated to the detector elements
/**
 *  Factory: dd4hep_DetElementConditionsDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int ddcond_detelement_processor(Detector& description, int argc, char** argv)   {

  /// detaill class to perform recursive printout
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/12/2016
   */
  struct Actor : public DetElementProcessor<ConditionsProcessor>  {
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
    processor = createProcessor<ConditionsProcessor>(description, argc, argv);
  }
  else  {
    const void* args[] = { "-processor", "dd4hepConditionsPrinter", 0};
    processor = createProcessor<ConditionsProcessor>(description, 2, (char**)args);
  }
  dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(description,"run",1500,argc,argv));
  UserPool* pool = slice->pool.get();
  Actor actor(processor);
  pool->print("User pool");
  processor->setPool(pool);
  int ret = Actor(processor).process(description.world(),0,true);
  slice->manager.clean(pool->validity().iovType, 20);
  return ret;
}
DECLARE_APPLY(dd4hep_DetElementConditionsProcessor,ddcond_detelement_processor)
#endif
// ======================================================================================
/// Plugin entry point: Synchronize conditions according to new IOV value
/**
 *  Factory: dd4hep_ConditionsSynchronize
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_synchronize_conditions(Detector& description, int argc, char** argv) {
  if ( argc >= 2 )   {
    string iov_typ = argv[0];
    IOV::Key::first_type iov_key = *(IOV::Key::first_type*)argv[1];
    dd4hep_ptr<ConditionsSlice> slice(ddcond_prepare(description,iov_typ,iov_key,argc,argv));
    UserPool* pool = slice->pool.get();
    pool->print("User pool");
    slice->manager.clean(pool->validity().iovType, 20);
    pool->clear();
    return 1;
  }
  except("Conditions","+++ Failed update conditions. Arguments were: '%s'",
         arguments(argc,argv).c_str());
  return 0;
}
DECLARE_APPLY(dd4hep_ConditionsSynchronize,ddcond_synchronize_conditions)

// ======================================================================================
/// Plugin entry point: Clean conditions reposiory according to maximum age
/**
 *  Factory: dd4hep_ConditionsClean
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_clean_conditions(Detector& description, int argc, char** argv) {
  if ( argc > 0 )   {
    string iov_type = argv[0];
    int    max_age  = *(int*)argv[1];
    printout(INFO,"Conditions",
             "+++ ConditionsUpdate: Cleaning conditions... type:%s max age:%d",
             iov_type.c_str(), max_age);
    ConditionsManager manager = ConditionsManager::from(description);
    const IOVType* iov_typ = manager.iovType(iov_type);
    manager.clean(iov_typ, max_age);
    return 1;
  }
  except("Conditions","+++ Failed cleaning conditions. Insufficient arguments!");
  return 0;
}
DECLARE_APPLY(dd4hep_ConditionsClean,ddcond_clean_conditions)

// ======================================================================================
/// Basic entry point to instantiate the basic dd4hep conditions/alignmants printer
/**
 *  Factory: dd4hepConditionsPrinter, dd4hepAlignmentsPrinter 
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    17/11/2016
 */
#include "DD4hep/PluginTester.h"
template <typename WRAPPER,typename PRINTER>
static void* create_printer(Detector& description, int argc,char** argv)  {
  PrintLevel print_level = INFO;
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
    else if ( 0 == ::strncmp("-print",argv[i],5) )
      print_level = dd4hep::printLevel(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name(s)  dd4hep_ConditionsPrinter,                       \n"
      "                              dd4hep_AlignmentsPrinter                        \n"
      "                              dd4hep_AlignedVolumePrinter                     \n"
      "     -prefix <string>         Printout prefix for user customized output.     \n"
      "     -flags  <number>         Printout processing flags.                      \n"
      "     -pool                    Attach conditions user pool from                \n"
      "                              PluginTester's slice instance attached.       \n\n"
      "     -print <value>           Printout level for the printer object.          \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  DetElement world = description.world();
  printout(INFO,"Printer","World=%s [%p]",world.path().c_str(),world.ptr());
  ConditionsSlice* slice = 0;
  if ( have_pool )   {
    PluginTester*    test  = description.extension<PluginTester>();
    slice = test->extension<ConditionsSlice>("ConditionsTestSlice");
  }
  PRINTER* p = (flags) ? new PRINTER(slice, prefix, flags) : new PRINTER(slice, prefix);
  p->printLevel = print_level;
  if ( !name.empty() ) p->name = name;
  return (void*)dynamic_cast<WRAPPER*>(createProcessorWrapper(p));
}
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/AlignmentsPrinter.h"
static void* create_cond_printer(Detector& description, int argc,char** argv)
{  return create_printer<Condition::Processor,ConditionsPrinter>(description,argc,argv);  }
                                                                        
DECLARE_Detector_CONSTRUCTOR(dd4hep_ConditionsPrinter,create_cond_printer)
//DECLARE_Detector_CONSTRUCTOR(dd4hep_AlignmentsPrinter,create_printer<AlignmentsPrinter>)
//DECLARE_Detector_CONSTRUCTOR(dd4hep_AlignedVolumePrinter,create_printer<AlignedVolumePrinter>)

// ======================================================================================
/// Plugin entry point: Create repository csv file from loaded conditions
/**
 *  Factory: dd4hep_ConditionsCreateRepository
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_create_repository(Detector& description, int argc, char** argv) {
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
      "     name:   factory name     dd4hep_ConditionsCreateRepository             \n\n"
      "     -output <string>         Output file name.                             \n\n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  printout(INFO,"Conditions",
           "+++ ConditionsRepository: Creating %s",output.c_str());
  ConditionsManager manager = ConditionsManager::from(description);
  ConditionsRepository().save(manager,output);
  return 1;
}
DECLARE_APPLY(dd4hep_ConditionsCreateRepository,ddcond_create_repository)

// ======================================================================================
/// Plugin entry point: Dump conditions repository csv file
/**
 *  Factory: dd4hep_ConditionsDumpRepository
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_dump_repository(Detector& /* description */, int argc, char** argv)   {
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
      "     name:   factory name     dd4hep_ConditionsDumpRepository               \n\n"
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
      printout(INFO,"Repository","%16llX  %s",e.key,e.name.c_str());
      printout(INFO,"Repository","          -> %s",e.address.c_str());
    }
  }
  return 1;
}
DECLARE_APPLY(dd4hep_ConditionsDumpRepository,ddcond_dump_repository)

// ======================================================================================
/// Plugin entry point: Load conditions repository csv file into conditions manager
/**
 *  Factory: dd4hep_ConditionsDumpRepository
 *

TO BE DONE!!!

 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static long ddcond_load_repository(Detector& /* description */, int argc, char** argv) {
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
DECLARE_APPLY(dd4hep_ConditionsLoadRepository,ddcond_load_repository)
// ======================================================================================
