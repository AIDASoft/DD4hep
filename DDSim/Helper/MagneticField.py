"""Helper object for Magnetic Field properties"""
from SystemOfUnits import mm
from DDSim.Helper.ConfigHelper import ConfigHelper

class MagneticField( ConfigHelper ):
  """MagneticField holding all field properties"""
  def __init__( self ):
    super(MagneticField, self).__init__()
    self.stepper = "HelixSimpleRunge"
    self.equation = "Mag_UsualEqRhs"
    self.eps_min = 5e-05*mm
    self.eps_max = 0.001*mm
    self.min_chord_step = 0.01*mm
    self.delta_chord = 0.25*mm
    self.delta_intersection = 1e-05*mm
    self.delta_one_step = 1e-04*mm
