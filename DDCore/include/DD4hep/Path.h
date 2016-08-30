//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-08-30
//  \version 1.0
//
//==========================================================================
// $Id$
#ifndef DD4HEP_DDCORE_PATH_H
#define DD4HEP_DDCORE_PATH_H

// Framework include files

// C/C++ include files
#include <string>

// Forward declartions

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {


  /// Path handling class.
  /**
   *  Normally this could as well be replaced by 
   *  boost::filesystem::path.
   *
   *  However, the LC folks do not like the additional link-dependency.
   *  Hence this small extraction.
   *  The main code comes from the boost implementation.
   *
   *  See http://www.boost.org/LICENSE_1_0.txt for the license and
   *  the filesystem library home page: http://www.boost.org/libs/filesystem
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class Path : public std::string {
  public:
    /// Default constructor
    Path() : std::string()                      {                    }
    /// Initializing constructor
    Path(const std::string& c) : std::string(c) {                    }
    /// Copy constructor
    Path(const Path& c) : std::string(c)    {                        }
    /// Assigning constructor
    template <class Iter> Path(Iter _begin,Iter _end)  {
      if ( _begin != _end )  {
        std::string s(_begin, _end);
        this->std::string::operator=(s);
      }
    }
    /// Default destructor
    ~Path() {}
    /// Assignment operator from Path object
    Path& operator=(const Path& c)          {
      this->std::string::operator=(c);
      return *this;
    }
    /// Assignment operator from string object
    Path& operator=(const std::string& c)   {
      this->std::string::operator=(c);
      return *this;
    }
    /// Append operation
    Path& append(const std::string& c);
    /// Append operation
    Path& operator/=(const Path& c)         {    return append(c);   }
    /// Append operation
    Path& operator/=(const std::string& c)  {    return append(c);   }
    /// Normalize path name
    Path  normalize()  const;
    /// Parent's path
    Path  parent_path() const;
    /// The file name of the path
    Path  filename() const;
    /// The full file path of the object
    Path  file_path() const;
    /// Manipulator: remove the file name part. Leaves the parent path
    Path& remove_filename();
    
    /// String representation of thre Path object
    const std::string& native() const       {    return *this;       }
    /// String representation of thre Path object
    const char* string_data() const { return this->std::string::c_str(); }
    /// Index of the parent's path end
    size_t parent_path_end() const;
    /// Helpers
    class detail  {
    public:
      /// Path representing "."
      static const Path& dot_path();
      /// Path representing ".."
      static const Path& dot_dot_path();
    };
  };
} /* End namespace DD4hep           */
#endif    /* DD4HEP_DDCORE_PATH_H   */
