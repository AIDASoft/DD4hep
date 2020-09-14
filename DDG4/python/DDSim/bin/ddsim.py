#!/usr/bin/env python
"""

DD4hep simulation with some argument parsing
Based on M. Frank and F. Gaede runSim.py
   @author  A.Sailer
   @version 0.1

"""
from __future__ import absolute_import, unicode_literals
import logging
import sys

from DDSim.DD4hepSimulation import DD4hepSimulation


if __name__ == "__main__":
  logging.basicConfig(format='%(name)-16s %(levelname)s %(message)s', level=logging.INFO, stream=sys.stdout)
  logger = logging.getLogger('DDSim')

  RUNNER = DD4hepSimulation()
  RUNNER.parseOptions()

  try:
    RUNNER.run()
  except NameError as e:
    if "global name" in str(e):
      globalToSet = str(e).split("'")[1]
      logger.fatal("Unknown global variable, please add\nglobal %s\nto your steeringFile" % globalToSet)
