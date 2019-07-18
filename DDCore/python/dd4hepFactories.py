#!/bin/python
#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================

from __future__ import absolute_import
import os, sys, optparse, logging

class ComponentDumper:
    def __init__(self):
        os.environ['DD4HEP_TRACE']='ON'
        self.all_components = []

    def scanPath(self):
        ldp = os.environ['LD_LIBRARY_PATH'].split(':')
        for p in ldp:
            if len(p):
                logging.info('+== Search component directory: '+p)
                files = os.listdir(p)
                for f in files:
                    fname = p+os.sep+f
                    ext = os.path.splitext(fname)[-1].lower()
                    if ext == '.components':
                        self.readComponents(fname)
                    elif ext == '.rootmap':
                        self.dumpDictionaries(fname)

    def readComponents(self,fname):
        logging.info('+== Search component file:  '+fname)
        file = open(fname,"r")
        lines = file.readlines()
        dirname = os.path.dirname(fname)
        for l in lines:
            l = l[:-1]
            if len(l)>2 and (l[0:2] == '//' or l[0] == '#'):
                continue
            lib, comp = l.split(':')
            self.all_components.append([dirname+os.sep+lib,comp])
        file.close()
        
    def searchDuplicates(self, summary_only=False):
        entries = {}
        component_count = 0
        duplicate_count = 0
        for lib, comp in self.all_components:
            if entries.has_key(comp):
                dupl = entries[comp]
                if dupl[1] == 0:
                    if not summary_only:
                        logging.info('+'+('==='*40))
                        logging.info('| Component entry:  '+comp+'  in library:  '+dupl[0])
                    entries[comp][1] = 1
                if not summary_only:
                    logging.info('|    --> Duplicate factory declaration in: '+ lib)
                duplicate_count = duplicate_count + 1
                continue
            entries[comp] = [lib,0]
            component_count = component_count + 1
        logging.info('+'+('==='*40))
        logging.info('|      Found %d dd4hep factory entries and %d DUPLICATES in component files.'%(component_count,duplicate_count,))
        logging.info('+'+('==='*40))

    def dumpInventory(self, summary_only=False, dump=True, load=False, interactive=True):
        entries = {}
        do_load = load
        library_count = 0
        component_count = 0
        for lib, comp in self.all_components:
            if not entries.has_key(lib):
                entries[lib] = [comp]
                library_count = library_count + 1
                component_count = component_count + 1
                continue
            entries[lib].append(comp)
            component_count = component_count + 1
        if not summary_only:
            for lib, comp in entries.items():
                if dump:
                    logging.info('+==  Component library:  '+lib)
                count = 0
                for c in comp:
                    count = count + 1
                    if dump:
                        logging.info('| %-3d       Component:  %s'%(count,c,))
                if do_load:
                    ret = 'D'
                    if interactive:
                        try:
                            ret = raw_input("<CR> to DUMP the list of components \n"+
                                            "<Q>  to QUIT                        \n"+
                                            "<D>  to DUMP the list of components \n"+
                                            "<S>  to SKIP this particular library\n"+
                                            "<L>  to no longer LOAD libraries    \n")
                        except Exception,X:
                            ret = 'D'
                    if not len(ret):
                       ret = 'D'
                    if ret[0].upper() == 'Q':
                       sys.exit(0)
                    elif ret[0].upper() == 'D':
                       gSystem.Load(lib)
                    elif ret[0].upper() == 'L':
                       do_load = False
        logging.info('+'+('==='*40))
        logging.info('|      Found %d dd4hep factory libraries with %d components.'%(library_count,component_count,))
        logging.info('+'+('==='*40) )

    def dumpDictionaries(self,fname):
        pass


logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
parser = optparse.OptionParser()
parser.description = "DD4hep factory checks."
parser.formatter.width = 132
parser.add_option("-c", "--check",
                  action="store_true", dest="check", default=False,
                  help="Check the components file in the LD_LIBRARY_PATH for duplicates",
		  metavar="<boolean>")

parser.add_option("-d","--dump",
		  action="store_true", dest="dump", default=False,
                  help="Dump the content of the components files",
		  metavar="<boolean>")

parser.add_option("-l","--load",
		  action="store_true", dest="load", default=False,
                  help="Load all libraries indicated in the component files",
		  metavar="<boolean>")

parser.add_option("-n","--no-interactive",
		  action="store_false", dest="interactive", default=True,
                  help="Always load. Do NOT inquire if a library should be loaded or not",
		  metavar="<boolean>")

(opts, args) = parser.parse_args()

if not (opts.check or opts.dump or opts.load):
   logging.info("   %s",parser.format_help())
   sys.exit(0)

try:
  import ROOT
  from ROOT import gROOT
  from ROOT import gSystem
  gROOT.SetBatch(1)
except ImportError,X:
  logging.error('PyROOT interface not accessible: %s',str(X))
  sys.exit(errno.ENOENT)

try:
  import dd4hep
except ImportError,X:
  logging.error('dd4hep python interface not accessible: %s',str(X))
  sys.exit(errno.ENOENT)

dmp = ComponentDumper()
dmp.scanPath()
if opts.check:
   dmp.searchDuplicates()
if opts.dump or opts.load:
   dmp.dumpInventory(dump=opts.dump,load=opts.load,interactive=opts.interactive)
sys.exit(0)
