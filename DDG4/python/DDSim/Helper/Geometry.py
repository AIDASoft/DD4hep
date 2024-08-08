"""Helper object for Geant4 Geometry conversion."""

from DDSim.Helper.ConfigHelper import ConfigHelper


class Geometry(ConfigHelper):
  """Configuration for the Detector Construction."""

  def __init__(self):
    super(Geometry, self).__init__()

    self._enableDebugMaterials_EXTRA = {"help": "Print Debug information about Materials"}
    self.enableDebugMaterials = False
    self._enableDebugElements_EXTRA = {"help": "Print Debug information about Elements"}
    self.enableDebugElements = False
    self._enableDebugVolumes_EXTRA = {"help": "Print Debug information about Volumes"}
    self.enableDebugVolumes = False
    self._enableDebugShapes_EXTRA = {"help": "Print Debug information about Shapes"}
    self.enableDebugShapes = False
    self._enableDebugPlacements_EXTRA = {"help": "Print Debug information about Placements"}
    self.enableDebugPlacements = False
    self._enableDebugReflections_EXTRA = {"help": "Print Debug information about Reflections"}
    self.enableDebugReflections = False
    self._enableDebugRegions_EXTRA = {"help": "Print Debug information about Regions"}
    self.enableDebugRegions = False
    self._enableDebugSurfaces_EXTRA = {"help": "Print Debug information about Surfaces"}
    self.enableDebugSurfaces = False
    self._dumpHierachy_EXTRA = {"help": "If larger than 0, the depth up to which detector hierarchy is dumped"}
    self.dumpHierarchy = 0

    self._enablePrintPlacements_EXTRA = {"help": "Print information about placements"}
    self.enablePrintPlacements = False
    self._enablePrintSensitives_EXTRA = {"help": "Print information about Sensitives"}
    self.enablePrintSensitives = False

    self._dumpDGDML_EXTRA = {"help": "If not empty, filename to dump the Geometry as GDML"}
    self.dumpGDML = ""

    self._regexSDDetectorList = []

    self._closeProperties()

  @property
  def regexSensitiveDetector(self):
    """Configure a sensitive detector for a given detector matched by regular expression (regex).

    'Detector' and 'Match' are mandatory elements of the dictionary, other Keys are assigned as property to the object.

    >>> SIM.geometry.regexSensitiveDetector = {'Detector': 'DRcalo',
                                               'Match': ['(core|clad)'],
                                               'OutputLevel': 3,
                                              }

    This can be assigned repeatedly to add multiple RegexSDs
    """
    return self._regexSDDetectorList

  @regexSensitiveDetector.setter
  def regexSensitiveDetector(self, val):
    if not val:
      return
    if isinstance(val, dict):
      self.__checkRegexKeys(val)
      self._regexSDDetectorList.append(val)
    elif isinstance(val, list):
      for value in val:
        self.__checkRegexKeys(value)
        self._regexSDDetectorList.append(value)
    raise RuntimeError(f"Unsupported type for regexSensitiveDetector: {val!r}")

  @staticmethod
  def __checkRegexKeys(val):
    """Check the regex SD arguments for required keys."""
    requiredKeys = ('Detector', 'Match')
    if not all(key in val for key in requiredKeys):
      raise RuntimeError(f"RegexSD configuration {val} is missing mandatory key(s): {', '.join(requiredKeys)}")

  def constructGeometry(self, kernel, geant4, geoPrintLevel=2, numberOfThreads=1):
    """Construct Geant4 geometry."""
    from DDG4 import DetectorConstruction

    seq, act = geant4.addDetectorConstruction('Geant4DetectorGeometryConstruction/ConstructGeo')
    act.DebugMaterials = self.enableDebugMaterials
    act.DebugElements = self.enableDebugElements
    act.DebugVolumes = self.enableDebugVolumes
    act.DebugShapes = self.enableDebugShapes
    act.DebugPlacements = self.enableDebugPlacements
    act.DebugReflections = self.enableDebugReflections
    act.DebugRegions = self.enableDebugRegions
    act.DebugSurfaces = self.enableDebugSurfaces
    act.PrintPlacements = self.enablePrintPlacements
    act.PrintSensitives = self.enablePrintSensitives
    act.GeoInfoPrintLevel = geoPrintLevel
    act.DumpHierarchy = self.dumpHierarchy
    act.DumpGDML = self.dumpGDML

    # Apply sensitive detectors
    sensitives = DetectorConstruction(kernel, str('Geant4DetectorSensitivesConstruction/ConstructSD'))
    sensitives.enableUI()
    seq.adopt(sensitives)

    for index, regexDetectors in enumerate(self._regexSDDetectorList):
      seq, act = geant4.addDetectorConstruction(f'Geant4RegexSensitivesConstruction/ConstrSDRegEx_{index}')
      # this will set Match and Detector, and other properties if possible
      for key, value in regexDetectors.items():
        setattr(act, key, value)
