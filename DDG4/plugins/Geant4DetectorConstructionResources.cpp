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
//  \date   2015-11-09
//
//==========================================================================

// Framework include files
#include <DDG4/Geant4DetectorConstruction.h>
#include <unistd.h>

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Debug class to dump resources usage during detector construction
    /**
     *  Debug class to dump resources usage during detector construction
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorConstructionResources : public Geant4DetectorConstruction   {
    public:
      /// Class to store the status information of a process from /proc/<pid>/status
      /**
       *
       * \author  M.Frank
       * \version 1.0
       */
      class __attribute__((__packed__)) StatusProcess {
      public:
        char   comm[399] = "";
        char   state     { 0  };
        int    umask     { 0  };
        int    tgid      { 0  };
        int    ngid      { 0  };
        int    pid       { 0  };
        int    ppid      { 0  };
        int    uid       { 0  };
        int    gid       { 0  };
        int    utrace    { 0  };
        int    fdSize    { 0  };
        long   vmPeak    { 0L };
        long   vmSize    { 0L };
        long   vmLock    { 0L };
        long   vmPin     { 0L };
        long   vmHWM     { 0L };
        long   vmRSS     { 0L };
        long   vmRSSano  { 0L };
        long   vmRSSfil  { 0L };
        long   vmRSSshm  { 0L };
        long   vmData    { 0L };
        long   vmStack   { 0L };
        long   vmExe     { 0L };
        long   vmLib     { 0L };
        long   vmPTE     { 0L };
        long   vmSwap    { 0L };
        /// Default constructor
        StatusProcess() {}
      };
      std::unique_ptr<StatusProcess> snapshot;
      std::string when   { "geometry|sensitives" };

      void print_status(const char* tag, const StatusProcess& sp) const;
      
    public:
      /// Initializing constructor for DDG4
      Geant4DetectorConstructionResources(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4DetectorConstructionResources();
      /// Sensitive detector construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext*)  override;
      /// Sensitives construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt)  override;
    };
  }    // End namespace sim
}      // End namespace dd4hep


// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/Detector.h>

#include <DDG4/Geant4Mapping.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Factories.h>

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace dd4hep::sim;

DECLARE_GEANT4ACTION(Geant4DetectorConstructionResources)

namespace  {
  
  class SysFile  {
  public:
    class FileDescriptor    {
      /// File handle
      int m_fd;
      
    public:
      /// Initializing constructor
      FileDescriptor(int value) : m_fd(value)      {}
      /// Default destructor. Non-virtuality is intended. Do not inherit!
      ~FileDescriptor();
      /// Access file handle
      int get() const      {   return m_fd;   }
    };
    
      
  public:
    /// File name
    std::string m_name;
    /// Initializing constructor
    SysFile(const char* name) : m_name(name) {}
    /// Initializing constructor
    SysFile(const std::string& name) : m_name(name) {}
    /// Default destructor. Non-virtuality is intended. Do not inherit!
    ~SysFile() {}
    /// Read buffer from file in  one go
    int read(char* buffer, size_t len) const;
  };

  /// Default destructor. Non-virtuality is intended. Do not inherit!
  SysFile::FileDescriptor::~FileDescriptor()   {
    if (m_fd > 0)  ::close(m_fd);
    m_fd = 0;
  }

  /// Read buffer from file in  one go
  int SysFile::read(char* buf, size_t siz) const  {
    FileDescriptor fd(::open(m_name.c_str(),O_RDONLY));  
    if( fd.get() < 0 )  {
      std::string err = "Failed to open "+m_name+" ";
      throw std::runtime_error(err+std::make_error_code(std::errc(errno)).message());
    }
    std::size_t tmp = 0;
    while ( tmp < siz )  {
      int sc = ::read(fd.get(),buf+tmp,siz-tmp);
      if ( sc >  0 ) {
        tmp += sc;
      }
      else if ( sc == 0 )  {
        buf[tmp] = 0;
        return tmp;
      }
      else if ( errno == EINTR )  {
        printf("EINTR~!!!!\n");
        continue;
      }
      else  {
        break;
      }
    }
    if ( tmp != siz )  {
      std::string err = "Read of system file "+m_name+" failed:";
      throw std::runtime_error(err+std::make_error_code(std::errc(errno)).message());
    }
    return tmp;
  }

  int read_info(Geant4DetectorConstructionResources::StatusProcess& proc, int proc_id) {
    char buff[2048], *ptr=buff;
    std::string fn = "/proc/"+std::to_string(proc_id)+"/status";
    int nitem=0, cnt=SysFile(fn.c_str()).read(buff,sizeof(buff));
    if(cnt>0)  {
      int ival;
      long int lval;

      while(ptr && ptr<(buff+cnt)) {
        char* p   = ::strchr(ptr,'\t');
        char* end = ::strchr(ptr,'\n');

        ptr = (end) ? end+1 : 0;
        if ( 0 == p ) continue;
        ++p;
        switch(++nitem) {
        case 1:   ::sscanf(p,"%s",proc.comm);                  break;
        case 2:   ::sscanf(p,"%d",&ival); proc.umask = ival;   break;
        case 3:   ::sscanf(p,"%c",&proc.state);                break;
        case 4:   ::sscanf(p,"%d",&ival); proc.tgid    = ival; break;
        case 5:   ::sscanf(p,"%d",&ival); proc.ngid    = ival; break;
        case 6:   ::sscanf(p,"%d",&ival); proc.pid     = ival; break;
        case 7:   ::sscanf(p,"%d",&ival); proc.ppid    = ival; break;
        case 8:   ::sscanf(p,"%d",&ival); proc.utrace  = ival; break;
        case 9:   ::sscanf(p,"%d",&ival); proc.uid     = ival; break;
        case 10:  ::sscanf(p,"%d",&ival); proc.gid     = ival; break;
        case 11:  ::sscanf(p,"%d",&ival); proc.fdSize  = ival; break;
        case 17:  ::sscanf(p,"%ld",&lval); proc.vmPeak  = lval; break;
        case 18:  ::sscanf(p,"%ld",&lval); proc.vmSize  = lval; break;
        case 19:  ::sscanf(p,"%ld",&lval); proc.vmLock  = lval; break;
        case 20:  ::sscanf(p,"%ld",&lval); proc.vmPin   = lval; break;
        case 21:  ::sscanf(p,"%ld",&lval); proc.vmHWM   = lval; break;
        case 22:  ::sscanf(p,"%ld",&lval); proc.vmRSS   = lval; break;
        case 23:  ::sscanf(p,"%ld",&lval); proc.vmRSSano= lval; break;
        case 24:  ::sscanf(p,"%ld",&lval); proc.vmRSSfil= lval; break;
        case 25:  ::sscanf(p,"%ld",&lval); proc.vmRSSshm= lval; break;
        case 26:  ::sscanf(p,"%ld",&lval); proc.vmData  = lval; break;
        case 27:  ::sscanf(p,"%ld",&lval); proc.vmStack = lval; break;
        case 28:  ::sscanf(p,"%ld",&lval); proc.vmExe   = lval; break;
        case 29:  ::sscanf(p,"%ld",&lval); proc.vmLib   = lval; break;
        case 30:  ::sscanf(p,"%ld",&lval); proc.vmPTE   = lval; break;
        case 31:  ::sscanf(p,"%ld",&lval); proc.vmSwap  = lval; break;
        case 32:  return 1;
        default:                                                break;
        }
      }
      return 1;
    }
    return 0;
  }
}

/// Initializing constructor for other clients
Geant4DetectorConstructionResources::Geant4DetectorConstructionResources(Geant4Context* ctxt, const std::string& nam)
: Geant4DetectorConstruction(ctxt,nam)
{
  declareProperty("When", this->when);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4DetectorConstructionResources::~Geant4DetectorConstructionResources() {
  InstanceCount::decrement(this);
}

/// Sensitive detector construction callback. Called at "Construct()"
void Geant4DetectorConstructionResources::constructGeo(Geant4DetectorConstructionContext*)   {
  if ( this->when.find("geometry") != std::string::npos )  {
    this->snapshot = std::make_unique<StatusProcess>();
    read_info(*this->snapshot, ::getpid());
    this->print_status("ConstructGeo: ", *this->snapshot);
  }
}

void Geant4DetectorConstructionResources::print_status(const char* tag, const StatusProcess& sp) const {
  this->always("%s Name:     \t%s", tag, sp.comm);
  this->always("%s State:    \t%c", tag, sp.state);
  this->always("%s Umask:    \t%8d", tag, sp.umask);
  this->always("%s Tgid:     \t%8d", tag, sp.tgid);
  this->always("%s Pid:      \t%8d", tag, sp.pid);
  this->always("%s PPid:     \t%8d", tag, sp.ppid);
  this->always("%s utrace:   \t%8d", tag, sp.utrace);
  this->always("%s Uid:      \t%8d", tag, sp.uid);
  this->always("%s Gid:      \t%8d", tag, sp.gid);
  this->always("%s FDSize:   \t%8d", tag, sp.fdSize);
  this->always("%s VmPeak:   \t%8ld kB", tag, sp.vmPeak);
  this->always("%s VmSize:   \t%8ld kB", tag, sp.vmSize);
  this->always("%s VmLck:    \t%8ld kB", tag, sp.vmLock);
  this->always("%s VmHWM:    \t%8ld kB", tag, sp.vmHWM);
  this->always("%s VmRSS:    \t%8ld kB", tag, sp.vmRSS);
  this->always("%s VmRSS     anon:    \t%8ld kB", tag, sp.vmRSSano);
  this->always("%s VmRSS     file:    \t%8ld kB", tag, sp.vmRSSfil);
  this->always("%s VmRSS     shm:     \t%8ld kB", tag, sp.vmRSSshm);
  this->always("%s VmData:   \t%8ld kB", tag, sp.vmData);
  this->always("%s VmStk:    \t%8ld kB", tag, sp.vmStack);
  this->always("%s VmExe:    \t%8ld kB", tag, sp.vmExe);
  this->always("%s VmLib:    \t%8ld kB", tag, sp.vmLib);
  this->always("%s VmPTE:    \t%8ld kB", tag, sp.vmPTE);
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4DetectorConstructionResources::constructSensitives(Geant4DetectorConstructionContext*)   {
  if ( this->when.find("sensitives") != std::string::npos )  {
    StatusProcess rd;
    read_info(rd, ::getpid());
    this->print_status("ConstructSD:  ", rd);
#if 0
    if ( snapshot )   {
      const auto& snap = *this->snapshot;
       this->always("   --> DIFFERENCE:  FDSize:   \t%8d", rd.fdSize - snap.fdSize);
       this->always("   --> DIFFERENCE:  VmPeak:   \t%8ld kB", rd.vmPeak - snap.vmPeak);
       this->always("   --> DIFFERENCE:  VmSize:   \t%8ld kB", rd.vmSize - snap.vmSize);
       this->always("   --> DIFFERENCE:  VmLck:    \t%8ld kB", rd.vmLock - snap.vmLock);
       this->always("   --> DIFFERENCE:  VmHWM:    \t%8ld kB", rd.vmHWM - snap.vmHWM);
       this->always("   --> DIFFERENCE:  VmRSS:    \t%8ld kB", rd.vmRSS - snap.vmRSS);
       this->always("   --> DIFFERENCE:  VmRSS     anon:    \t%8ld kB", rd.vmRSSano - snap.vmRSSano);
       this->always("   --> DIFFERENCE:  VmRSS     file:    \t%8ld kB", rd.vmRSSfil - snap.vmRSSfil);
       this->always("   --> DIFFERENCE:  VmRSS     shm:     \t%8ld kB", rd.vmRSSshm - snap.vmRSSshm);
       this->always("   --> DIFFERENCE:  VmData:   \t%8ld kB", rd.vmData - snap.vmData);
       this->always("   --> DIFFERENCE:  VmStk:    \t%8ld kB", rd.vmStack - snap.vmStack);
       this->always("   --> DIFFERENCE:  VmExe:    \t%8ld kB", rd.vmExe - snap.vmExe);
       this->always("   --> DIFFERENCE:  VmLib:    \t%8ld kB", rd.vmLib - snap.vmLib);
       this->always("   --> DIFFERENCE:  VmPTE:    \t%8ld kB", rd.vmPTE - snap.vmPTE);
    }
#endif
  }
  snapshot.reset();
}
