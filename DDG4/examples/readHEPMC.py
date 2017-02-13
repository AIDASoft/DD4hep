"""

DD4hep simulation example setup using the python configuration

@author  M.Frank
@version 1.0

"""
def run(input_file):
  import DDG4
  from DDG4 import OutputLevel as Output
  kernel = DDG4.Kernel()
  lcdd = kernel.lcdd()
  gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/Input")
  kernel.generatorAction().adopt(gen)
  gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/hepmc_geant4.dat"
  gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/"
  gen.Input = "Geant4EventReaderHepMC|"+input_file
  gen.OutputLevel = Output.DEBUG
  gen.HaveAbort = False
  prim_vtx = DDG4.std_vector('DD4hep::Simulation::Geant4Vertex*')()
  parts = gen.new_particles()
  ret = 1
  while ret:
    try:
      ret = gen.readParticles(0,prim_vtx,parts)
    except Exception,X:
      print '\nException: readParticles:',str(X)
      ret = None
    if ret:
      for v in prim_vtx:
        print 'Mask:%04X (X:%8.2g,Y:%8.2g,Z:%8.2g) time:%8.2g Outgoing:%4d particles'%\
              (v.mask,v.x,v.y,v.z,v.time,len(v.out),)
      for p in parts:
        print 'Mask:%04X ID:%5d PDG-id:%8d Charge:%1d Mass:%8.3g Momentum:(%8.2g,%8.2g,%8.2g) '\
              'Vertex:(%8.2g,%8.2g,%8.2g) NDau:%2d Status:%08X'%\
              (p.mask,p.id,p.pdgID,int(p.charge3()),p.mass,p.psx,p.psy,p.psz,\
               p.vsx,p.vsy,p.vsz,len(p.daughters),p.status,)
      parts.clear()
    else:
      print '*** End of recordset or read failure.....'
    print 132*'*'
  return 0
  
if __name__ == "__main__":
  import sys
  input_file = None
  if len(sys.argv) > 1:
    input_file = sys.argv[1]
    sys.exit(run(input_file))
  else:
    print 'No input file given. Try again....'
    sys.exit(2)  # ENOENT
