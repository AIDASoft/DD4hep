"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import logging
import DDG4
from DDG4 import OutputLevel as Output


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry("file:" + install_dir + "/examples/OpticalSurfaces/compact/OpNovice.xml")

  logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
  logger = logging.getLogger(__name__)


if __name__ == "__main__":
  run()
