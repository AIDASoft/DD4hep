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

#include "DD4hep/Path.h"
#include <climits>
#include <cstring>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace DD4hep;

namespace {
  const char dot = '.';
  const char separator = '/';
  const char* const separators = "/";
  const char colon = ':';

  
  inline bool is_separator(char c)  { return c == separator;  }

  bool is_root_separator(const string& str, size_t pos)
  // pos is position of the separator
  {
    if ( str.empty() || is_separator(str[pos]) ) {
      throw runtime_error("precondition violation");
    }
    // subsequent logic expects pos to be for leftmost slash of a set
    while (pos > 0 && is_separator(str[pos-1]))
      --pos;

    //  "/" [...]
    if (pos == 0)  
      return true;
    //  "//" name "/"
    if (pos < 3 || !is_separator(str[0]) || !is_separator(str[1]))
      return false;

    return str.find_first_of(separators, 2) == pos;
  }

  size_t filename_pos(const string& str,size_t end_pos)
  // return 0 if str itself is filename (or empty)
  {
    // case: "//"
    if (end_pos == 2 
        && is_separator(str[0])
        && is_separator(str[1])) return 0;

    // case: ends in "/"
    if (end_pos && is_separator(str[end_pos-1]))
      return end_pos-1;
    
    // set pos to start of last element
    size_t pos(str.find_last_of(separators, end_pos-1));

    return (pos == string::npos // path itself must be a filename (or empty)
            || (pos == 1 && is_separator(str[0]))) // or net
      ? 0 // so filename is entire string
      : pos + 1; // or starts after delimiter
  }

  // return npos if no root_directory found
  size_t root_directory_start(const string& path, size_t size)  {
    // case "//"
    if (size == 2
        && is_separator(path[0])
        && is_separator(path[1])) return string::npos;
    // case "//net {/}"
    if (size > 3
        && is_separator(path[0])
        && is_separator(path[1])
        && !is_separator(path[2]))
    {
      string::size_type pos(path.find_first_of(separators, 2));
      return pos < size ? pos : string::npos;
    }
    
    // case "/"
    if (size > 0 && is_separator(path[0])) return 0;
    return string::npos;
  }
}

const Path& Path::detail::dot_path()   {
  static Path p(".");
  return p;
}
const Path& Path::detail::dot_dot_path()  {
  static Path p("..");
  return p;
}

Path& Path::append(const std::string& c)   {
  insert(end(),separator);
  insert(end(),c.begin(),c.end());
  return *this;
}

Path Path::normalize()  const {
  if (empty())
    return *this;

  vector<string> pathes;
  char tmp[PATH_MAX];
  ::strncpy(tmp, string_data(), sizeof(tmp));
  tmp[sizeof(tmp)-1] = 0;
  char *token, *save=0;
  token = ::strtok_r(tmp,separators,&save);
  while(token)  {
    pathes.push_back(token);
    token = ::strtok_r(0,separators,&save);
  }
  Path temp;
  vector<string>::const_iterator start(pathes.begin());
  vector<string>::const_iterator last(pathes.end());
  vector<string>::const_iterator stop(last--);
  for (vector<string>::const_iterator itr(start); itr != stop; ++itr)  {
    // ignore "." except at start and last
    Path itr_path(*itr);
    if (itr_path.native().size() == 1
        && (itr_path.native())[0] == dot
        && itr != start
        && itr != last) continue;

    // ignore a name and following ".."
    if ( temp.empty() && itr_path.find(colon) != string::npos )  {
      temp = itr_path;
      continue;
    }
    else if (!temp.empty()
        && itr_path.native().size() == 2
        && (itr_path.native())[0] == dot
        && (itr_path.native())[1] == dot) // dot dot
    {
      string lf(temp.filename().native());  
      if (lf.size() > 0
          && (lf.size() != 1 || (lf[0] != dot && lf[0] != separator))
          && (lf.size() != 2 || (lf[0] != dot && lf[1] != dot))   )
      {
        temp.remove_filename();
        //// if not root directory, must also remove "/" if any
        //if (temp.native().size() > 0
        //  && temp.native()[temp.native().size()-1]
        //    == separator)
        //{
        //  string::size_type rds(
        //    root_directory_start(temp.native(), temp.native().size()));
        //  if (rds == string::npos
        //    || rds != temp.native().size()-1) 
        //  {
        //    temp.m_pathname.erase(temp.native().size()-1);
        //  }
        //}

        vector<string>::const_iterator next(itr);
        if (temp.empty() && ++next != stop && next == last && *last == detail::dot_path())  {
          temp /= detail::dot_path();
        }
        continue;
      }
    }
    temp /= *itr;
  };

  if (temp.empty())
    temp /= detail::dot_path();
  return temp;
}

size_t Path::parent_path_end() const  {
  size_t end_pos(filename_pos(native(),this->size()));
  bool filename_was_separator(this->size() && is_separator(at(end_pos)));

  // skip separators unless root directory
  size_t root_dir_pos(root_directory_start(native(), end_pos));
  for (; end_pos > 0
         && (end_pos-1) != root_dir_pos
         && is_separator(this->at(end_pos-1))
         ;
       --end_pos) {}

  return (end_pos == 1 && root_dir_pos == 0 && filename_was_separator) ? string::npos : end_pos;
}


Path& Path::remove_filename()   {
  this->erase(this->parent_path_end());
  return *this;
}

Path  Path::parent_path() const  {
  size_t end_pos(parent_path_end());
  return end_pos == string::npos ? Path() : Path(string_data(), string_data() + end_pos);
}

Path Path::filename() const
{
  size_t pos(filename_pos(native(), native().size()));
  return (native().size()
          && pos
          && is_separator(at(pos))
          && !is_root_separator(native(), pos))
    ? detail::dot_path()
    : Path(string_data() + pos);
}

Path  Path::file_path() const  {
  size_t pos(filename_pos(native(), native().size()));
  return (native().size()
          && pos
          && is_separator(at(pos))
          && !is_root_separator(native(), pos))
    ? detail::dot_path()
    : Path(string_data() + pos);
}
