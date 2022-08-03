#!/usr/bin/env python
# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================

from __future__ import absolute_import, unicode_literals
import sys
import errno
import argparse
import logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

parser = argparse.ArgumentParser()
parser.description = "Check TGeo geometries for overlaps."
parser.add_argument("-c", "--compact", dest="compact", default=None, nargs='+',
                    help="Define LCCDD style compact xml input",
                    metavar="<FILE>")
parser.add_argument("-p", "--print",
                    dest="print_overlaps", default=True,
                    help="Print overlap information to standard output (default:True)",
                    metavar="<boolean>")
parser.add_argument("-q", "--quiet",
                    action="store_false", dest="print_overlaps",
                    help="Do not print (disable --print)")
parser.add_argument("-t", "--tolerance", dest="tolerance", default=0.1,
                    type=float,
                    help="Overlap checking tolerance. Unit is in [mm]. (default:0.1 mm)",
                    metavar="<double number>")
parser.add_argument("-o", "--option", dest="option", default='',
                    help="Overlap checking option ('' or 's')",
                    metavar="<string>")

args = parser.parse_args()

if args.compact is None:
  logger.info("    %s", parser.format_help())
  sys.exit(1)

try:
  from ROOT import gROOT
  gROOT.SetBatch(1)
except ImportError as X:
  logger.error('PyROOT interface not accessible: %s', str(X))
  logger.error("%s", parser.format_help())
  sys.exit(errno.ENOENT)

try:
  import dd4hep
except ImportError as X:
  logger.error('dd4hep python interface not accessible: %s', str(X))
  logger.error("%s", parser.format_help())
  sys.exit(errno.ENOENT)
#
#
dd4hep.setPrintLevel(dd4hep.OutputLevel.ERROR)
logger.info('+++%s', 120 * '=')
logger.info('+++ Loading compact geometry:')
for xmlfile in args.compact:
  logger.info('+++ ' + xmlfile)
logger.info('+++ tolerance: %f, option: %s', args.tolerance, args.option)
logger.info('+++%s\n', 120 * '=')
description = dd4hep.Detector.getInstance()
for xmlfile in args.compact:
  description.fromXML(xmlfile)

print(description.constants())

description.manager().CheckOverlaps(args.tolerance, args.option)
#
#
if args.print_overlaps:
  logger.info('+++%s', 120 * '=')
  logger.info('+++ Printing overlaps of geometry:')
  for xmlfile in args.compact:
    logger.info('+++ %s', xmlfile)
  logger.info('+++%s\n', 120 * '=')
  description.manager().PrintOverlaps()
#
#
logger.info('+++ Execution finished...')
sys.exit(0)
