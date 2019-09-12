#!/usr/env python
"""
This file will automaticcaly create the cpp files for the parsers
for pod and std::containers of pods, and some other maps

This reduces the maximum required memory and allows faster compilation due to
higher parallelisation of the build process
This needs only to be run if additional parsers are neccessary.
In this case copy the file to DDCore/src/parsers and run the program.

python CreateParsers.py

"""

from __future__ import absolute_import, unicode_literals
import os
from io import open

LICENSE = """// $Id$
//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
"""


def createParsers():
  """ make files for all parsers"""

  listOfTypes = ['int',
                 'long',
                 'char',
                 'bool',
                 'short',
                 'float',
                 'double',
                 'long long',
                 'long double',
                 'unsigned int',
                 'unsigned long',
                 'unsigned char',
                 'unsigned short',
                 'unsigned long long',
                 'std::string',
                 'signed char',
                 ]
  listOfContainers = ['std::vector', 'std::list', 'std::set', 'std::deque']
  listOfMaps = ['int', 'unsigned long', 'std::string', ]

  for typ in listOfTypes:
    for cont in listOfContainers:
      createContainerFile(typ, cont)
    for mtype in listOfMaps:
      createMapFile(typ, mtype)

    createMappedFile(typ)


def createMappedFile(typ):
  """ create file for mapped parsers """
  tName = typ[5:] if typ.startswith("std::") else typ
  filename = "ParserStandardList_Mapped_%s.cpp" % (tName.replace(" ", ""))
  fileContent = """
#include "ParsersStandardListCommon.h"
namespace dd4hep{ namespace Parsers{
IMPLEMENT_MAPPED_PARSERS(pair,%(type)s)
  }}
""" % {"type": typ}
  fileContent = LICENSE + fileContent
  if os.path.exists(filename):
    os.remove(filename)
  with open(filename, "w") as parseFile:
    parseFile.write(fileContent)


def createContainerFile(typ, cont):
  """create file to make container parser"""
  tName = typ[5:] if typ.startswith("std::") else typ
  filename = "ParserStandardList_%s_%s.cpp" % (cont[5:], tName.replace(" ", ""))
  fileContent = """
#include "ParsersStandardListCommon.h"
namespace dd4hep{ namespace Parsers{
IMPLEMENT_STL_PARSER(%(cont)s,%(type)s)
  }}
""" % {"cont": cont, "type": typ}
  fileContent = LICENSE + fileContent
  if os.path.exists(filename):
    os.remove(filename)
  with open(filename, "w") as parseFile:
    parseFile.write(fileContent)


def createMapFile(typ, mtype):
  """ create file to make map parser"""
  mName = mtype[5:] if mtype.startswith("std::") else mtype
  tName = typ[5:] if typ.startswith("std::") else typ
  filename = "ParserStandardList_Map%s_%s.cpp" % (mName.replace(" ", ""), tName.replace(" ", ""))
  fileContent = """
#include "ParsersStandardListCommon.h"
namespace dd4hep{ namespace Parsers{
IMPLEMENT_STL_MAP_PARSER(std::map,%(mtype)s,%(type)s)
  }}
""" % {"mtype": mtype, "type": typ}
  fileContent = LICENSE + fileContent
  if os.path.exists(filename):
    os.remove(filename)
  with open(filename, "w") as parseFile:
    parseFile.write(fileContent)


if __name__ == "__main__":
  createParsers()
