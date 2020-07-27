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
import os
import sys
import errno
import optparse
import logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def printOpts(opts):
  o = eval(str(opts))
  prefix = sys.argv[0].split(os.sep)[-1]
  for name, value in o.items():
    logger.info('%s > %-18s %s  [%s]', prefix, name + ':', str(value), str(value.__class__))


def materialScan(opts):
  kernel = DDG4.Kernel()
  kernel.loadGeometry(str(opts.compact))
  DDG4.Core.setPrintFormat(str("%-32s %6s %s"))
  geant4 = DDG4.Geant4(kernel)
  # Configure UI
  geant4.setupCshUI(ui=None)
  for i in geant4.description.detectors():
    o = DDG4.DetElement(i.second.ptr())
    sd = geant4.description.sensitiveDetector(o.name())
    if sd.isValid():
      typ = sd.type()
      if typ in geant4.sensitive_types:
        geant4.setupDetector(o.name(), geant4.sensitive_types[typ])
      else:
        logger.error('+++  %-32s type:%-12s  --> Unknown Sensitive type: %s', o.name(), typ, typ)
        sys.exit(errno.EINVAL)

  geant4.setupGun("Gun",
                  Standalone=True,
                  particle='geantino',
                  energy=20 * g4units.GeV,
                  position=opts.position,
                  direction=opts.direction,
                  multiplicity=1,
                  isotrop=False)
  scan = DDG4.SteppingAction(kernel, 'Geant4GeometryScanner/GeometryScan')
  kernel.steppingAction().adopt(scan)

  # Now build the physics list:
  geant4.setupPhysics('QGSP_BERT')

  kernel.configure()
  kernel.initialize()
  kernel.NumEvents = 1
  kernel.run()
  kernel.terminate()
  return 0


parser = optparse.OptionParser()
parser.formatter.width = 132
parser.description = 'Scan Volumes along a trajectory using Geant4.'
parser.add_option('-c', '--compact', dest='compact', default=None,
                  help='Define LCCDD style compact xml input',
                  metavar='<FILE>')
parser.add_option('-P', '--print',
                  dest='print_level', default=2,
                  help='Set dd4hep print level.',
                  metavar='<int>')
parser.add_option('-p', '--position',
                  dest='position', default='0.0,0.0,0.0',
                  help='Start position of the material scan. [give tuple "x,y,z" as string]',
                  metavar='<tuple>')
parser.add_option('-d', '--direction',
                  dest='direction', default='0.0,1.0,0.0',
                  help='Direction of the material scan. [give tuple "x,y,z" as string]',
                  metavar='<tuple>')

(opts, args) = parser.parse_args()

if opts.compact is None:
  logger.info("%s", parser.format_help())
  sys.exit(1)

opts.position = eval('(' + opts.position + ')')
opts.direction = eval('(' + opts.direction + ')')
printOpts(opts)

try:
  from ROOT import gROOT
  gROOT.SetBatch(1)
except ImportError as X:
  logger.error('PyROOT interface not accessible: %s', X)
  logger.info(parser.format_help())
  sys.exit(errno.ENOENT)

try:
  import DDG4
  import g4units
except ImportError as X:
  logger.error('DDG4 python interface not accessible: %s', X)
  logger.info(parser.format_help())
  sys.exit(errno.ENOENT)
#
ret = materialScan(opts)
sys.exit(ret)
