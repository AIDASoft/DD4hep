// $Id: Primitives.h 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DD4HEP_IOSTREAMS_H
#define DD4HEP_DD4HEP_IOSTREAMS_H

// C/C++ include files
#include <string>

// booost iostreams include files
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/detail/ios.hpp>
#include <boost/iostreams/detail/path.hpp>
#include <boost/iostreams/positioning.hpp>

class TFile;

namespace DD4hep {

  // Forward declarations
  template <typename T> class dd4hep_file_source;
  template <typename T> class dd4hep_file_sink;

  enum dd4hep_file_flags   {
    never_close_handle = 0,
    close_handle = 3
  };

  /// DD4hep file handling extension to boost::iostreams
  /**
   *  Basic file handling for data sources and data sinks.
   *  Please see boost::iostreams for details.
   *  This got inspired by boost::iostreams::device::file_descriptor
   *
   *  The final aim is to hide all this behind a TFile object,
   *  so that we get the entire network file handling for free!
   *
   *  Note: 
   *  Credits go to a large extend to the authors of boost::iostreams.
   *  Without their work I could not have done this!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   *  \see http://www.boost.org/libs/iostreams for further documentation.
   */
  template <typename T> class dd4hep_file {
  public:
    friend class dd4hep_file_source<T>;
    friend class dd4hep_file_sink<T>;
    typedef T     handle_type;
    typedef char  char_type;
    typedef boost::iostreams::stream_offset stream_offset;
    typedef boost::iostreams::detail::path detail_path;
    struct  category : boost::iostreams::seekable_device_tag, boost::iostreams::closable_tag { };

    // Default constructor
    dd4hep_file() : m_handle(0) {   }
    // Constructors taking file desciptors
    dd4hep_file(handle_type fd, dd4hep_file_flags);
    // Constructors taking file desciptors
    dd4hep_file(const char* fname, BOOST_IOS::openmode mode);

    // open overloads taking file descriptors
    void open(handle_type fd, dd4hep_file_flags flags);

    // open overload taking C-style string
    void open(const char* path, BOOST_IOS::openmode mode = BOOST_IOS::in | BOOST_IOS::out );

    bool is_open() const  {  return m_handle != 0;  }
    void close();
    std::streamsize read(char_type* s, std::streamsize n);
    std::streamsize write(const char_type* s, std::streamsize n);
    std::streampos seek(stream_offset off, BOOST_IOS::seekdir way);
    handle_type handle() const   {   return m_handle; }
  private:
    handle_type m_handle;
    dd4hep_file_flags m_flag;
  };


  /// DD4hep file source extension to boost::iostreams
  /**
   *  Basic data sources implementation.
   *  Please see boost::iostreams for details.
   *  This got inspired by boost::iostreams::device::file_descriptor
   *
   *  The final aim is to hide all this behind a TFile object,
   *  so that we get the entire network file handling for free!
   *
   *  Note: 
   *  Credits go to a large extend to the authors of boost::iostreams.
   *  Without their work I could not have done this!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   *  \see http://www.boost.org/libs/iostreams for further documentation.
   */
  template <typename T=int> class dd4hep_file_source : private dd4hep_file<T> {
  public:
    typedef dd4hep_file<T> descriptor;
    struct category : boost::iostreams::input_seekable, 
		      boost::iostreams::device_tag,
		      boost::iostreams::closable_tag      { };
    typedef typename descriptor::handle_type handle_type;
    typedef typename descriptor::char_type   char_type;
    using descriptor::is_open;
    using descriptor::close;
    using descriptor::read;
    using descriptor::seek;
    using descriptor::handle;

    /// Default Constructor
    dd4hep_file_source() : descriptor() {  }

    /// Copy constructor
    dd4hep_file_source(const dd4hep_file_source<T>& other)
      : descriptor(other)      {                     }      

    /// Constructors taking file desciptors
    explicit dd4hep_file_source(handle_type h, dd4hep_file_flags flags)
      : descriptor(h,flags)  {                       }

    /// Constructors taking file desciptors
    explicit dd4hep_file_source(const char* name, BOOST_IOS::openmode mode = BOOST_IOS::in) 
      : descriptor(name,mode) {                      }

    /// open overload taking file desciptors
    void open(handle_type h, dd4hep_file_flags flags)
    {	this->descriptor::open(h, flags);              }

    /// open overload taking C-style string
    void open(const char* path, BOOST_IOS::openmode mode = BOOST_IOS::in)   
    {	this->descriptor::open(path,mode);             }

    /// open overload taking a std::string
    void open(const std::string& path, BOOST_IOS::openmode mode = BOOST_IOS::in)
    {	open(path.c_str(), mode);                                 }

    /// open overload taking a Boost.Filesystem path
    template<typename Path> void open(const Path& path, BOOST_IOS::openmode mode = BOOST_IOS::in)
    {	open(detail_path(path), mode);                           }
  };

  /// DD4hep file sink extension to boost::iostreams
  /**
   *  Basic data sink implementation.
   *  Please see boost::iostreams for details.
   *  This got inspired by boost::iostreams::device::file_descriptor
   *
   *  The final aim is to hide all this behind a TFile object,
   *  so that we get the entire network file handling for free!
   *
   *  Note: 
   *  Credits go to a large extend to the authors of boost::iostreams.
   *  Without their work I could not have done this!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   *  \see http://www.boost.org/libs/iostreams for further documentation.
   */
  template <typename T>
  class  dd4hep_file_sink : private dd4hep_file<T> {
  public:
    typedef dd4hep_file<T> descriptor;
    struct category : boost::iostreams::output_seekable, 
      boost::iostreams::device_tag, 
      boost::iostreams::closable_tag  { };
    typedef typename descriptor::handle_type handle_type;
    typedef typename descriptor::char_type   char_type;
    using descriptor::is_open;
    using descriptor::close;
    using descriptor::write;
    using descriptor::seek;
    using descriptor::handle;

    /// Default constructor
    dd4hep_file_sink()  {                        }

    /// Copy constructor
    dd4hep_file_sink(const dd4hep_file_sink<T>& other)
      : descriptor(other) {                     }

    /// Constructors taking file desciptors
    explicit dd4hep_file_sink(handle_type fd, dd4hep_file_flags flags)   
      : descriptor(fd, flags)      {            }

    /// Constructor taking a std::string
    explicit dd4hep_file_sink(const std::string& path, BOOST_IOS::openmode mode = BOOST_IOS::out)
      : descriptor(path.c_str(), mode)     {    }

    /// Constructor taking a C-style string
    explicit dd4hep_file_sink(const char* path, BOOST_IOS::openmode mode = BOOST_IOS::out )
      : descriptor(path, mode)     {            }

    /// Constructor taking a Boost.Filesystem path
    template<typename Path>
    explicit dd4hep_file_sink(const Path& path, BOOST_IOS::openmode mode = BOOST_IOS::out )
      : descriptor(detail_path(path), mode)  { }

    /// open overloads taking file descriptors
    void open(handle_type fd, dd4hep_file_flags flags)
    {	this->descriptor::open(fd,flags);         }

    /// open overload taking a std::string
    void open(const std::string& path, BOOST_IOS::openmode mode = BOOST_IOS::out )   
    {  open(path.c_str(),mode);                            }

    /// open overload taking C-style string
    void open(const char* path, BOOST_IOS::openmode mode = BOOST_IOS::out )
    {  this->descriptor::open(path, mode);      }

    /// open overload taking a Boost.Filesystem path
    template<typename Path> void open(const Path& path, BOOST_IOS::openmode mode = BOOST_IOS::out )
    {  open(detail_path(path), mode);                     }
  };
}   // End namespace boost
#endif // DD4HEP_DD4HEP_IOSTREAMs_H
