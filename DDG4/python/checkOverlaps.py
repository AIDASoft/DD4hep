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
import sys, errno, optparse, logging

logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

parser = optparse.OptionParser()
parser.formatter.width = 132
parser.description = "Check TGeo geometries for overlaps."
parser.add_option("-c", "--compact", dest="compact", default=None,
                  help="Define LCCDD style compact xml input",
		  metavar="<FILE>")
parser.add_option("-p", "--print",
		  dest="print_overlaps", default=True,
                  help="Print overlap information to standard output (default:True)",
		  metavar="<boolean>")
parser.add_option("-q", "--quiet", 
                  action="store_false", dest="print_overlaps",
                  help="Do not print (disable --print)")
parser.add_option("-t", "--tolerance", dest="tolerance", default=0.1,
                  help="Overlap checking tolerance. Unit is in [mm]. (default:0.1 mm)",
		  metavar="<double number>")
parser.add_option("-o", "--option", dest="option", default='',
                  help="Overlap checking option ('' or 's')",
		  metavar="<string>")

(opts, args) = parser.parse_args()

if opts.compact is None:
  logger.info("    %s",parser.format_help())
  sys.exit(1)

try:
  import ROOT
  from ROOT import gROOT
  gROOT.SetBatch(1)
except ImportError,X:
  logger.error('PyROOT interface not accessible: %s',str(X))
  logger.error("%s",parser.format_help())
  sys.exit(errno.ENOENT)

try:
  import dd4hep
except ImportError,X:
  logger.error('dd4hep python interface not accessible: %s',str(X))
  logger.error("%s",parser.format_help())
  sys.exit(errno.ENOENT)
#
#
opts.tolerance = float(opts.tolerance)
dd4hep.setPrintLevel(dd4hep.OutputLevel.ERROR)
logger.info('+++%s\n+++ Loading compact geometry:%s\n+++%s',120*'=',opts.compact,120*'=')
description = dd4hep.Detector.getInstance()
description.fromXML(opts.compact)
logger.info('+++%s\n+++ Checking overlaps of geometry:%s tolerance:%f option:%s\n+++%s',120*'=',opts.compact,opts.tolerance,opts.option,120*'=')
description.manager().CheckOverlaps(opts.tolerance,opts.option)
#
#
if opts.print_overlaps:
  logger.info('+++%s\n+++ Printing overlaps of geometry:%s\n+++%s',120*'=',opts.compact,120*'=')
  description.manager().PrintOverlaps()
#
#
logger.info('+++ Execution finished...')
sys.exit(0)
