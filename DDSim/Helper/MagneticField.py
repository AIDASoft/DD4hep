"""Helper object for Magnetic Field properties"""
from SystemOfUnits import mm, m
from DDSim.Helper.ConfigHelper import ConfigHelper

class MagneticField( ConfigHelper ):
  """Configuration for the magnetic field (stepper)"""
  def __init__( self ):
    super(MagneticField, self).__init__()
    self.stepper = "G4ClassicalRK4"
    self.equation = "Mag_UsualEqRhs"
    self.eps_min = 5e-05*mm
    self.eps_max = 0.001*mm
    self.min_chord_step = 0.01*mm
    self.delta_chord = 0.25*mm
    self.delta_intersection = 0.001*mm
    self.delta_one_step = 0.01*mm
    self.largest_step = 10*m
