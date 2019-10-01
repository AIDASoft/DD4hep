"""

dd4hep simulation example setup using the python configuration

@author  M.Frank
@version 1.0

"""
from __future__ import absolute_import, unicode_literals
import os, logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def run(input_file):
  import DDG4
  from DDG4 import OutputLevel as Output
  kernel = DDG4.Kernel()
  kernel.detectorDescription()
  gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/Input")
  kernel.generatorAction().adopt(gen)
  gen.Input = "Geant4EventReaderHepMC|" + input_file
  gen.OutputLevel = Output.DEBUG
  gen.HaveAbort = False
  prim_vtx = DDG4.std_vector(str('dd4hep::sim::Geant4Vertex*'))()
  parts = gen.new_particles()
  ret = 1
  evtid = 0
  while ret:
    try:
      parts.clear()
      prim_vtx.clear()
      ret = gen.readParticles(evtid, prim_vtx, parts)
      evtid = evtid + 1
    except Exception as X:
      logger.error('\nException: readParticles: %s', str(X))
      if evtid > 0: os._exit(0);
    if ret:
      for v in prim_vtx:
        logger.info('Mask:%04X (X:%8.2g,Y:%8.2g,Z:%8.2g) time:%8.2g Outgoing:%4d particles',
                    v.mask, v.x, v.y, v.z, v.time, len(v.out))
      for p in parts:
        logger.info('Mask:%04X ID:%5d PDG-id:%8d Charge:%1d Mass:%8.3g Momentum:(%8.2g,%8.2g,%8.2g) '
                    'Vertex:(%8.2g,%8.2g,%8.2g) NDau:%2d Status:%08X',
                    p.mask, p.id, p.pdgID, int(p.charge3()), p.mass, p.psx, p.psy, p.psz,
                    p.vsx, p.vsy, p.vsz, len(p.daughters), p.status)
      parts.clear()
    else:
      logger.info('*** End of recordset or read failure.....')
    logger.info(132 * '*')
  return 0


if __name__ == "__main__":
  import sys
  input_file = None
  if len(sys.argv) > 1:
    input_file = sys.argv[1]
    sys.exit(run(input_file))
  else:
    logger.error('No input file given. Try again....')
    sys.exit(2)  # ENOENT
