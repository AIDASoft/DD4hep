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

from __future__ import absolute_import, unicode_literals
import sys, errno, optparse, logging

logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

def dumpData( det ):
  try:
    dat = DDRec.FixedPadSizeTPCData(det)
    logger.info(dat.toString())
  except:
    pass
  try:
    dat = DDRec.ZPlanarData(det)
    logger.info(dat.toString())
  except:
    pass
  try:
    dat = DDRec.ZDiskPetalsData(det)
    logger.info(dat.toString())
  except:
    pass
  try:
    dat = DDRec.ConicalSupportData(det)
    logger.info(dat.toString())
  except:
    pass
  try:
    dat = DDRec.LayeredCalorimeterData(det)
    logger.info(dat.toString())
  except:
    pass
  try:
    dat = DDRec.NeighbourSurfacesStruct(det)
    logger.info(dat.toString())
  except:
    pass


parser = optparse.OptionParser()
parser.formatter.width = 132
parser.description = "Dump detector data objects from DDRec"
parser.add_option("-c", "--compact", dest="compact", default=None,
                  help="Define LCCDD style compact xml input",
		  metavar="<FILE>")

(opts, args) = parser.parse_args()

if opts.compact is None:
  logger.info("    %s",parser.format_help())
  sys.exit(1)

try:
  import ROOT
  from ROOT import gROOT
  gROOT.SetBatch(1)
except ImportError as X:
  logger.error('PyROOT interface not accessible: %s',str(X))
  logger.error("%s",parser.format_help())
  sys.exit(errno.ENOENT)

try:
  import dd4hep
except ImportError as X:
  logger.error('dd4hep python interface not accessible: %s',str(X))
  logger.error("%s",parser.format_help())
  sys.exit(errno.ENOENT)
#
try:
  import DDRec
except ImportError as X:
  logger.error('ddrec python interface not accessible: %s',str(X))
  logger.error("%s",parser.format_help())
  sys.exit(errno.ENOENT)
#

dd4hep.setPrintLevel(dd4hep.OutputLevel.ERROR)
logger.info('+++%s\n+++ Loading compact geometry:%s\n+++%s',120*'=',opts.compact,120*'=')



description = dd4hep.Detector.getInstance()
description.fromXML(opts.compact)


## ------ loop over detectors and print their detector data objects

for n,d in description.detectors():

    logger.info("")
    logger.info(" ------------- detector :  %s" , d.name())
    logger.info("")

    det = description.detector( n ) 

    dumpData(det)

##-----------------------------------------------------------------


logger.info('+++ Execution finished...')
sys.exit(0)
