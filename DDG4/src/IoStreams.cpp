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

// Framework includes
#include "DDG4/IoStreams.h"

// C/C++ include files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>

// ROOT include files
#include "TFile.h"

using namespace DD4hep;

namespace {
  /// Anonymous cast class to get access to protected members of TFile ;-)
  class MyTFile : public TFile  {
  private:
    /// Destructor (unused!)
    virtual ~MyTFile() {}
  public:
    /// Basic write call
    virtual Int_t SysWrite(Int_t fd, const void* buf, Int_t len)  { return TFile::SysWrite(fd, buf, len);  }
    /// Basic read call
    virtual Int_t SysRead(Int_t fd, void* buf, Int_t len)         { return TFile::SysRead(fd,buf,len);     }
    /// Basic seek call
    virtual Long64_t SysSeek(Int_t fd, Long64_t off, Int_t way)   { return TFile::SysSeek(fd, off, way);   }
  };
}

namespace DD4hep {

  /// Specialization for standard file descriptor files according to the posix standard
  template<> void dd4hep_file<int>::open(const char* path, BOOST_IOS::openmode mode)  {
    if ( m_handle ) ::close(m_handle);
    m_handle = ::open(path,mode);
  }

  /// Specialization for standard file descriptor files according to the posix standard
  template<> dd4hep_file<int>::dd4hep_file(handle_type fd, dd4hep_file_flags flags)
    : m_handle(fd), m_flag(flags)   {                   }

  /// Specialization for standard file descriptor files according to the posix standard
  template<> dd4hep_file<int>::dd4hep_file(const char* fn, BOOST_IOS::openmode mode)
    : m_handle(0), m_flag(close_handle) {open(fn,mode); }

  /// Specialization for standard file descriptor files according to the posix standard
  template<> std::streamsize dd4hep_file<int>::read(char_type* s, std::streamsize n)
  {      return ::read(m_handle,s,n);                   }

  /// Specialization for standard file descriptor files according to the posix standard
  template<> std::streamsize dd4hep_file<int>::write(const char_type* s, std::streamsize n)
  {      return ::write(m_handle,s,n);                  }

  /// Specialization for standard file descriptor files according to the posix standard
  template<> std::streampos dd4hep_file<int>::seek(stream_offset off, BOOST_IOS::seekdir way)
  {      return ::lseek(m_handle,off,way);              }

  /// Specialization for standard file descriptor files according to the posix standard
  template<> void dd4hep_file<int>::close()  {
    if ( m_handle ) ::close(m_handle);
    m_handle = 0;
  }

  /// Specialization for the usage of TFile structures
  template<> void dd4hep_file<TFile*>::open(const char* path, BOOST_IOS::openmode mode)  {
    if ( m_handle )   {
      m_handle->Close();
      delete m_handle;
    }
    std::string p = path;
    p += "?filetype=raw";
    if ( mode&BOOST_IOS::out )
      m_handle = TFile::Open(p.c_str(),"RECREATE","ROOT");
    else
      m_handle = TFile::Open(p.c_str());
    if ( m_handle->IsZombie() )   {
      delete m_handle;
      m_handle = 0;
      throw 1;
    }
  }
#define _p(x) (reinterpret_cast<MyTFile*>(x))

  /// Specialization for the usage of TFile structures
  template<> dd4hep_file<TFile*>::dd4hep_file(handle_type fd, dd4hep_file_flags flags)
    : m_handle(fd), m_flag(flags)    {                                         }

  /// Specialization for the usage of TFile structures
  template<> dd4hep_file<TFile*>::dd4hep_file(const char* fname, BOOST_IOS::openmode mode)
    : m_handle(0), m_flag(close_handle)  { open(fname,mode);                   }

  /// Specialization for the usage of TFile structures
  template<> std::streamsize dd4hep_file<TFile*>::read(char_type* s, std::streamsize n)  {
    if ( m_handle )   {
      Long64_t nb1 = m_handle->GetBytesRead();
      Bool_t   res = _p(m_handle)->ReadBuffer(s,nb1,n);
      if ( res )  {
        Long64_t nb2 = m_handle->GetBytesRead();
        return nb2-nb1;
      }
    }
    return -1;
  }

  /// Specialization for the usage of TFile structures
  template<> std::streamsize dd4hep_file<TFile*>::write(const char_type* s, std::streamsize n)
  { return m_handle ? _p(m_handle)->SysWrite(m_handle->GetFd(),s,n) : -1;      }

  /// Specialization for the usage of TFile structures
  template<> std::streampos dd4hep_file<TFile*>::seek(stream_offset off, BOOST_IOS::seekdir way)
  { return m_handle ? _p(m_handle)->SysSeek(m_handle->GetFd(),off,way) : -1;   }

  /// Specialization for the usage of TFile structures
  template<> void dd4hep_file<TFile*>::close()
  { if ( m_handle )  { m_handle->Close(); delete m_handle; m_handle=0; }       }

}
