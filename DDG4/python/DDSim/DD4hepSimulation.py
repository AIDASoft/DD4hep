"""

DD4hep simulation with some argument parsing
Based on M. Frank and F. Gaede runSim.py
   @author  A.Sailer
   @version 0.1

"""
from __future__ import absolute_import, unicode_literals, division, print_function

import os
import sys
import traceback
from DDSim.Helper.Meta import Meta
from DDSim.Helper.LCIO import LCIO
from DDSim.Helper.HepMC3 import HepMC3
from DDSim.Helper.GuineaPig import GuineaPig
from DDSim.Helper.Physics import Physics
from DDSim.Helper.Filter import Filter
from DDSim.Helper.Random import Random
from DDSim.Helper.Action import Action
from DDSim.Helper.Output import Output, outputLevel, outputLevelType
from DDSim.Helper.OutputConfig import OutputConfig
from DDSim.Helper.InputConfig import InputConfig
from DDSim.Helper.ConfigHelper import ConfigHelper
from DDSim.Helper.MagneticField import MagneticField
from DDSim.Helper.ParticleHandler import ParticleHandler
from DDSim.Helper.Gun import Gun
import argparse
import ddsix as six
import logging
from io import open

logger = logging.getLogger('DDSim')

try:
  import argcomplete
  ARGCOMPLETEENABLED = True
except ImportError:
  ARGCOMPLETEENABLED = False

POSSIBLEINPUTFILES = (".stdhep", ".slcio", ".HEPEvt", ".hepevt", ".hepmc", ".hepmc3", ".pairs")


class DD4hepSimulation(object):
  """Class to hold all the parameters and functions to run simulation"""

  def __init__(self):
    self.steeringFile = None
    self.compactFile = []
    self.inputFiles = []
    self.outputFile = "dummyOutput.slcio"
    self.runType = "batch"
    self.printLevel = 3

    self.numberOfEvents = 0
    self.skipNEvents = 0
    self.physicsList = None  # deprecated use physics.list
    self.crossingAngleBoost = 0.0
    self.macroFile = ''
    self.enableGun = False
    self.enableG4GPS = False
    self.enableG4Gun = False
    self._g4gun = None
    self._g4gps = None
    self.vertexSigma = [0.0, 0.0, 0.0, 0.0]
    self.vertexOffset = [0.0, 0.0, 0.0, 0.0]
    self.enableDetailedShowerMode = False

    self._errorMessages = []
    self._dumpParameter = False
    self._dumpSteeringFile = False

    # objects for extended configuration option
    self.output = Output()
    self.random = Random()
    self.gun = Gun()
    self.part = ParticleHandler()
    self.field = MagneticField()
    self.action = Action()
    self.outputConfig = OutputConfig()
    self.inputConfig = InputConfig()
    self.guineapig = GuineaPig()
    self.lcio = LCIO()
    self.hepmc3 = HepMC3()
    self.meta = Meta()

    self.filter = Filter()
    self.physics = Physics()

    self._argv = None

  def readSteeringFile(self):
    """Reads a steering file and sets the parameters to that of the
    DD4hepSimulation object present in the steering file.
    """
    globs = {}
    locs = {}
    if not self.steeringFile:
      return
    sFileTemp = self.steeringFile
    exec(compile(open(self.steeringFile).read(), self.steeringFile, 'exec'), globs, locs)
    for _name, obj in locs.items():
      if isinstance(obj, DD4hepSimulation):
        self.__dict__ = obj.__dict__
    self.steeringFile = os.path.abspath(sFileTemp)

  def parseOptions(self, argv=None):
    """parse the command line options"""

    if argv is None:
      self._argv = list(sys.argv)

    parser = argparse.ArgumentParser("Running DD4hep Simulations:",
                                     formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument("--steeringFile", "-S", action="store", default=self.steeringFile,
                        help="Steering file to change default behaviour")

    # first we parse just the steering file, but only if we don't want to see the help message
    if not any(opt in self._argv for opt in ('-h', '--help')):
      parsed, _unknown = parser.parse_known_args()
      self.steeringFile = parsed.steeringFile
      self.readSteeringFile()

    # readSteeringFile will set self._argv to None if there is a steering file
    if self._argv is None:
      self._argv = list(argv) if argv else list(sys.argv)

    parser.add_argument("--compactFile", nargs='+', action="store",
                        default=ConfigHelper.makeList(self.compactFile), type=str,
                        help="The compact XML file, or multiple compact files, if the last one is the closer.")

    parser.add_argument("--runType", action="store", choices=("batch", "vis", "run", "shell", "qt"),
                        default=self.runType,
                        help="The type of action to do in this invocation"  # Note: implicit string concatenation
                        "\nbatch: just simulate some events, needs numberOfEvents, and input file or gun"
                        "\nvis: enable visualisation, run the macroFile if it is set"
                        "\nqt: enable visualisation in Qt shell, run the macroFile if it is set"
                        "\nrun: run the macroFile and exit"
                        "\nshell: enable interactive session")

    parser.add_argument("--inputFiles", "-I", nargs='+', action="store", default=self.inputFiles,
                        help="InputFiles for simulation %s files are supported" % ", ".join(POSSIBLEINPUTFILES))

    parser.add_argument("--outputFile", "-O", action="store", default=self.outputFile,
                        help="Outputfile from the simulation: .slcio, edm4hep.root and .root"
                        " output files are supported")

    parser.add_argument("-v", "--printLevel", action="store", default=self.printLevel, dest="printLevel",
                        choices=(1, 2, 3, 4, 5, 6, 7, 'VERBOSE', 'DEBUG',
                                 'INFO', 'WARNING', 'ERROR', 'FATAL', 'ALWAYS'),
                        type=outputLevelType,
                        help="Verbosity use integers from 1(most) to 7(least) verbose"
                        "\nor strings: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS")

    parser.add_argument("--numberOfEvents", "-N", action="store", dest="numberOfEvents", default=self.numberOfEvents,
                        type=int, help="number of events to simulate, used in batch mode")

    parser.add_argument("--skipNEvents", action="store", dest="skipNEvents", default=self.skipNEvents, type=int,
                        help="Skip first N events when reading a file")

    parser.add_argument("--physicsList", action="store", dest="physicsList", default=self.physicsList,
                        help="Physics list to use in simulation")

    parser.add_argument("--crossingAngleBoost", action="store", dest="crossingAngleBoost",
                        default=self.crossingAngleBoost,
                        type=float, help="Lorentz boost for the crossing angle, in radian!")

    parser.add_argument("--vertexSigma", nargs=4, action="store", dest="vertexSigma",
                        default=self.vertexSigma, metavar=('X', 'Y', 'Z', 'T'),
                        type=float, help="FourVector of the Sigma for the Smearing of the Vertex position: x y z t")

    parser.add_argument("--vertexOffset", nargs=4, action="store", dest="vertexOffset",
                        default=self.vertexOffset, metavar=('X', 'Y', 'Z', 'T'),
                        type=float, help="FourVector of translation for the Smearing of the Vertex position: x y z t")

    parser.add_argument("--macroFile", "-M", action="store", dest="macroFile", default=self.macroFile,
                        help="Macro file to execute for runType 'run' or 'vis'")

    parser.add_argument("--enableGun", "-G", action="store_true", dest="enableGun", default=self.enableGun,
                        help="enable the DDG4 particle gun")

    parser.add_argument("--enableG4GPS", action="store_true", dest="enableG4GPS", default=self.enableG4GPS,
                        help="enable the Geant4 GeneralParticleSource. Needs a macroFile (runType run)"
                        "or use it with the shell (runType shell)")

    parser.add_argument("--enableG4Gun", action="store_true", dest="enableG4Gun", default=self.enableG4Gun,
                        help="enable the Geant4 particle gun. Needs a macroFile (runType run)"
                        " or use it with the shell (runType shell)")

    parser.add_argument("--dumpParameter", "--dump", action="store_true", dest="dumpParameter",
                        default=self._dumpParameter, help="Print all configuration Parameters and exit")

    parser.add_argument("--enableDetailedShowerMode", action="store_true", dest="enableDetailedShowerMode",
                        default=self.enableDetailedShowerMode,
                        help="use detailed shower mode")

    parser.add_argument("--dumpSteeringFile", action="store_true", dest="dumpSteeringFile",
                        default=self._dumpSteeringFile, help="print an example steering file to stdout")

    # output, or do something smarter with fullHelp only for example
    ConfigHelper.addAllHelper(self, parser)
    # now parse everything. The default values are now taken from the
    # steeringFile if they were set so that the steering file parameters can be
    # overwritten from the command line
    if ARGCOMPLETEENABLED:
      argcomplete.autocomplete(parser)
    parsed = parser.parse_args()

    self._dumpParameter = parsed.dumpParameter
    self._dumpSteeringFile = parsed.dumpSteeringFile

    self.compactFile = ConfigHelper.makeList(parsed.compactFile)
    self.inputFiles = parsed.inputFiles
    self.inputFiles = self.__checkFileFormat(self.inputFiles, POSSIBLEINPUTFILES)
    self.outputFile = parsed.outputFile
    self.__checkFileFormat(self.outputFile, ('.root', '.slcio'))
    self.runType = parsed.runType
    self.printLevel = self.__checkOutputLevel(parsed.printLevel)

    self.numberOfEvents = parsed.numberOfEvents
    self.skipNEvents = parsed.skipNEvents
    self.physicsList = parsed.physicsList
    self.crossingAngleBoost = parsed.crossingAngleBoost
    self.macroFile = parsed.macroFile
    self.enableGun = parsed.enableGun
    self.enableG4Gun = parsed.enableG4Gun
    self.enableG4GPS = parsed.enableG4GPS
    self.enableDetailedShowerMode = parsed.enableDetailedShowerMode
    self.vertexOffset = parsed.vertexOffset
    self.vertexSigma = parsed.vertexSigma

    self._consistencyChecks()

    if self.printLevel <= 2:  # VERBOSE or DEBUG
      logger.setLevel(logging.DEBUG)

    # self.__treatUnknownArgs( parsed, unknown )
    self.__parseAllHelper(parsed)
    if self._errorMessages and not (self._dumpParameter or self._dumpSteeringFile):
      parser.epilog = "\n".join(self._errorMessages)
      parser.print_help()
      exit(1)

    if self._dumpParameter:
      from pprint import pprint
      logger.info("=" * 80)
      pprint(vars(self))
      logger.info("=" * 80)
      exit(1)

    if self._dumpSteeringFile:
      self.__printSteeringFile(parser)
      exit(1)

  def getDetectorLists(self, detectorDescription):
    ''' get lists of trackers and calorimeters that are defined in detectorDescription (the compact xml file)'''
    import DDG4
    trackers, calos, unknown = [], [], []
    for i in detectorDescription.detectors():
      det = DDG4.DetElement(i.second.ptr())
      name = det.name()
      sd = detectorDescription.sensitiveDetector(name)
      if sd.isValid():
        detType = sd.type()
        logger.info('getDetectorLists - found active detector %s type: %s', name, detType)
        if any(pat.lower() in detType.lower() for pat in self.action.trackerSDTypes):
          trackers.append(det.name())
        elif any(pat.lower() in detType.lower() for pat in self.action.calorimeterSDTypes):
          calos.append(det.name())
        else:
          logger.warning('Unknown sensitive detector type: %s', detType)
          unknown.append(det.name())

    return trackers, calos, unknown

# ==================================================================================

  def run(self):
    """setup the geometry and dd4hep and geant4 and do what was asked to be done"""
    import ROOT
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    import DDG4
    import dd4hep

    self.printLevel = getOutputLevel(self.printLevel)

    kernel = DDG4.Kernel()
    dd4hep.setPrintLevel(self.printLevel)

    for compactFile in self.compactFile:
      kernel.loadGeometry(str("file:" + compactFile))
    detectorDescription = kernel.detectorDescription()

    DDG4.importConstants(detectorDescription)

  # ----------------------------------------------------------------------------------

    # simple = DDG4.Geant4( kernel, tracker='Geant4TrackerAction',calo='Geant4CalorimeterAction')
    # simple = DDG4.Geant4( kernel, tracker='Geant4TrackerCombineAction',calo='Geant4ScintillatorCalorimeterAction')
    simple = DDG4.Geant4(kernel, tracker=self.action.tracker, calo=self.action.calo)

    simple.printDetectors()

    if self.runType == "vis":
      simple.setupUI(typ="tcsh", vis=True, macro=self.macroFile)
    elif self.runType == "qt":
      simple.setupUI(typ="qt", vis=True, macro=self.macroFile)
    elif self.runType == "run":
      simple.setupUI(typ="tcsh", vis=False, macro=self.macroFile, ui=False)
    elif self.runType == "shell":
      simple.setupUI(typ="tcsh", vis=False, macro=None, ui=True)
    elif self.runType == "batch":
      simple.setupUI(typ="tcsh", vis=False, macro=None, ui=False)
    else:
      logger.error("unknown runType")
      exit(1)

    kernel.NumEvents = self.numberOfEvents

    # -----------------------------------------------------------------------------------
    # setup the magnetic field:
    self.__setMagneticFieldOptions(simple)

    # ----------------------------------------------------------------------------------

    # Configure Run actions
    run1 = DDG4.RunAction(kernel, 'Geant4TestRunAction/RunInit')
    kernel.registerGlobalAction(run1)
    kernel.runAction().add(run1)

    # Configure the random seed, do it before the I/O because we might change the seed!
    self.random.initialize(DDG4, kernel, self.output.random)

    # Configure I/O
    if callable(self.outputConfig._userPlugin):
      self.outputConfig._userPlugin(self)
    elif self.outputFile.endswith(".slcio"):
      lcOut = simple.setupLCIOOutput('LcioOutput', self.outputFile)
      lcOut.RunHeader = self.meta.addParametersToRunHeader(self)
      eventPars = self.meta.parseEventParameters()
      lcOut.EventParametersString, lcOut.EventParametersInt, lcOut.EventParametersFloat = eventPars
      lcOut.RunNumberOffset = self.meta.runNumberOffset if self.meta.runNumberOffset > 0 else 0
      lcOut.EventNumberOffset = self.meta.eventNumberOffset if self.meta.eventNumberOffset > 0 else 0
    elif self.outputFile.endswith("edm4hep.root"):
      e4Out = simple.setupEDM4hepOutput('EDM4hepOutput', self.outputFile)
      eventPars = self.meta.parseEventParameters()
      e4Out.EventParametersString, e4Out.EventParametersInt, e4Out.EventParametersFloat = eventPars
      e4Out.RunNumberOffset = self.meta.runNumberOffset if self.meta.runNumberOffset > 0 else 0
      e4Out.EventNumberOffset = self.meta.eventNumberOffset if self.meta.eventNumberOffset > 0 else 0
    elif self.outputFile.endswith(".root"):
      simple.setupROOTOutput('RootOutput', self.outputFile)

    actionList = []

    if self.enableGun:
      gun = DDG4.GeneratorAction(kernel, "Geant4ParticleGun/" + "Gun")
      self.gun.setOptions(gun)
      gun.Standalone = False
      gun.Mask = 1
      actionList.append(gun)
      self.__applyBoostOrSmear(kernel, actionList, 1)
      logger.info("++++ Adding DD4hep Particle Gun ++++")

    if self.enableG4Gun:
      # GPS Create something
      self._g4gun = DDG4.GeneratorAction(kernel, "Geant4GeneratorWrapper/Gun")
      self._g4gun.Uses = 'G4ParticleGun'
      self._g4gun.Mask = 2
      logger.info("++++ Adding Geant4 Particle Gun ++++")
      actionList.append(self._g4gun)

    if self.enableG4GPS:
      # GPS Create something
      self._g4gps = DDG4.GeneratorAction(kernel, "Geant4GeneratorWrapper/GPS")
      self._g4gps.Uses = 'G4GeneralParticleSource'
      self._g4gps.Mask = 3
      logger.info("++++ Adding Geant4 General Particle Source ++++")
      actionList.append(self._g4gps)

    start = 4
    for index, plugin in enumerate(self.inputConfig.userInputPlugin, start=start):
      gen = plugin(self)
      gen.Mask = index
      start = index + 1
      actionList.append(gen)
      self.__applyBoostOrSmear(kernel, actionList, index)
      logger.info("++++ Adding User Plugin %s ++++", gen.Name)

    for index, inputFile in enumerate(self.inputFiles, start=start):
      if inputFile.endswith(".slcio"):
        gen = DDG4.GeneratorAction(kernel, "LCIOInputAction/LCIO%d" % index)
        gen.Parameters = self.lcio.getParameters()
        gen.Input = "LCIOFileReader|" + inputFile
      elif inputFile.endswith(".stdhep"):
        gen = DDG4.GeneratorAction(kernel, "LCIOInputAction/STDHEP%d" % index)
        gen.Input = "LCIOStdHepReader|" + inputFile
      elif inputFile.endswith(".HEPEvt"):
        gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/HEPEvt%d" % index)
        gen.Input = "Geant4EventReaderHepEvtShort|" + inputFile
      elif inputFile.endswith(".hepevt"):
        gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/hepevt%d" % index)
        gen.Input = "Geant4EventReaderHepEvtLong|" + inputFile
      elif inputFile.endswith(".hepmc"):
        if self.hepmc3.useHepMC3:
          gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/hepmc%d" % index)
          gen.Parameters = self.hepmc3.getParameters()
          gen.Input = "HEPMC3FileReader|" + inputFile
        else:
          gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/hepmc%d" % index)
          gen.Input = "Geant4EventReaderHepMC|" + inputFile
      elif inputFile.endswith(".hepmc3"):
        if self.hepmc3.useHepMC3:
          gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/hepmc%d" % index)
          gen.Parameters = self.hepmc3.getParameters()
          gen.Input = "HEPMC3FileReader|" + inputFile
        else:
          raise RuntimeError("HepMC3 input file support not enabled")
      elif inputFile.endswith(".pairs"):
        gen = DDG4.GeneratorAction(kernel, "Geant4InputAction/GuineaPig%d" % index)
        gen.Input = "Geant4EventReaderGuineaPig|" + inputFile
        gen.Parameters = self.guineapig.getParameters()
      else:
        # this should never happen because we already check at the top, but in case of some LogicError...
        raise RuntimeError("Unknown input file type: %s" % inputFile)
      gen.Sync = self.skipNEvents
      gen.Mask = index
      actionList.append(gen)
      self.__applyBoostOrSmear(kernel, actionList, index)

    if actionList:
      self._buildInputStage(simple, actionList, output_level=self.output.inputStage,
                            have_mctruth=self._enablePrimaryHandler())

    # ================================================================================================

    # And handle the simulation particles.
    part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
    kernel.generatorAction().adopt(part)
    # part.SaveProcesses = ['conv','Decay']
    part.SaveProcesses = self.part.saveProcesses
    part.MinimalKineticEnergy = self.part.minimalKineticEnergy
    part.KeepAllParticles = self.part.keepAllParticles
    part.PrintEndTracking = self.part.printEndTracking
    part.PrintStartTracking = self.part.printStartTracking
    part.MinDistToParentVertex = self.part.minDistToParentVertex
    part.OutputLevel = self.output.part
    part.enableUI()

    if self.part.enableDetailedHitsAndParticleInfo:
      self.part.setDumpDetailedParticleInfo(kernel, DDG4)

    self.part.setupUserParticleHandler(part, kernel, DDG4)

    # =================================================================================

    # Setup global filters for use in sensitive detectors
    try:
      self.filter.setupFilters(kernel)
    except RuntimeError as e:
      logger.error("%s", e)
      exit(1)

    # =================================================================================
    # get lists of trackers and calorimeters in detectorDescription

    trk, cal, unk = self.getDetectorLists(detectorDescription)

    for detectors, function, defFilter, abort in [(trk, simple.setupTracker, self.filter.tracker, False),
                                                  (cal, simple.setupCalorimeter, self.filter.calo, False),
                                                  (unk, simple.setupDetector, None, True),
                                                  ]:
      try:
        self.__setupSensitiveDetectors(detectors, function, defFilter, abort)
      except Exception as e:
        logger.error("Failed setting up sensitive detector %s", e)
        raise

  # =================================================================================
    # Now build the physics list:
    _phys = self.physics.setupPhysics(kernel, name=self.physicsList)

    # add the G4StepLimiterPhysics to activate the max step limits in volumes
    ph = DDG4.PhysicsList(kernel, 'Geant4PhysicsList/Myphysics')
    ph.addPhysicsConstructor(str('G4StepLimiterPhysics'))
    _phys.add(ph)

    dd4hep.setPrintLevel(self.printLevel)

    kernel.configure()
    kernel.initialize()

    # GPS
    if self._g4gun is not None:
      self._g4gun.generator()
    if self._g4gps is not None:
      self._g4gps.generator()

    startUpTime, _sysTime, _cuTime, _csTime, _elapsedTime = os.times()

    kernel.run()
    kernel.terminate()

    totalTimeUser, totalTimeSys, _cuTime, _csTime, _elapsedTime = os.times()
    if self.printLevel <= 3:
      logger.info("DDSim            INFO  Total Time:   %3.2f s (User), %3.2f s (System)" %
                  (totalTimeUser, totalTimeSys))
      if self.numberOfEvents != 0:
        eventTime = totalTimeUser - startUpTime
        perEventTime = eventTime / self.numberOfEvents
        logger.info("DDSim            INFO  StartUp Time: %3.2f s, Event Processing: %3.2f s (%3.2f s/Event) "
                    % (startUpTime, eventTime, perEventTime))

  def __setMagneticFieldOptions(self, simple):
    """ create and configure the magnetic tracking setup """
    field = simple.addConfig('Geant4FieldTrackingSetupAction/MagFieldTrackingSetup')
    field.stepper = self.field.stepper
    field.equation = self.field.equation
    field.eps_min = self.field.eps_min
    field.eps_max = self.field.eps_max
    field.min_chord_step = self.field.min_chord_step
    field.delta_chord = self.field.delta_chord
    field.delta_intersection = self.field.delta_intersection
    field.delta_one_step = self.field.delta_one_step
    field.largest_step = self.field.largest_step

  def __checkFileFormat(self, fileNames, extensions):
    """check if the fileName is allowed, note that the filenames are case
    sensitive, and in case of hepevt we depend on this to identify short and long versions of the content
    """
    if isinstance(fileNames, six.string_types):
      fileNames = [fileNames]
    if not all(fileName.endswith(extensions) for fileName in fileNames):
      self._errorMessages.append("ERROR: Unknown fileformat for file: %s" % fileNames)
    return fileNames

  def __applyBoostOrSmear(self, kernel, actionList, mask):
    """apply boost or smearing for given mask index"""
    import DDG4
    if self.crossingAngleBoost:
      lbo = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexBoost")
      lbo.Angle = self.crossingAngleBoost
      lbo.Mask = mask
      actionList.append(lbo)

    if any(self.vertexSigma) or any(self.vertexOffset):
      vSmear = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexSmear")
      vSmear.Offset = self.vertexOffset
      vSmear.Sigma = self.vertexSigma
      vSmear.Mask = mask
      actionList.append(vSmear)

  def __parseAllHelper(self, parsed):
    """ parse all the options for the helper """
    parsedDict = vars(parsed)
    for name, obj in six.iteritems(vars(self)):
      if isinstance(obj, ConfigHelper):
        for var in obj.getOptions():
          key = "%s.%s" % (name, var)
          if key in parsedDict:
            try:
              obj.setOption(var, parsedDict[key])
            except RuntimeError as e:
              self._errorMessages.append("ERROR: %s " % e)
              if logger.level <= logging.DEBUG:
                self._errorMessages.append(traceback.format_exc())

  def __checkOutputLevel(self, level):
    """return outputlevel as int so we don't have to import anything for faster startup"""
    try:
      return outputLevel(level)
    except ValueError:
      self._errorMessages.append("ERROR: printLevel is neither integer nor string")
      return -1
    except KeyError:
      self._errorMessages.append("ERROR: printLevel '%s' unknown" % level)
      return -1

  def __setupSensitiveDetectors(self, detectors, setupFuction, defaultFilter=None,
                                abortForMissingAction=False,
                                ):
    """Attach sensitive detector actions for all subdetectors.

    Can be steered with the `Action` ConfigHelpers

    :param detectors: list of detectors
    :param setupFunction: function used to register the sensitive detector
    :param defaultFilter: default filter to apply for given types
    :param abortForMissingAction: if true end program if there is no action found
    """
    for det in detectors:
      logger.info('Setting up SD for %s', det)
      action = None
      for pattern in self.action.mapActions:
        if pattern.lower() in det.lower():
          action = self.action.mapActions[pattern]
          logger.info('       replace default action with : %s', action)
          break
      if abortForMissingAction and action is None:
        logger.error('Cannot find Action for detector %s. You have to extend "action.mapAction"', det)
        raise RuntimeError("Cannot find Action")
      seq, act = setupFuction(det, type=action)
      self.filter.applyFilters(seq, det, defaultFilter)

      # set detailed hit creation mode for this
      if self.enableDetailedShowerMode:
        if isinstance(act, list):
          for a in act:
            a.HitCreationMode = 2
        else:
          act.HitCreationMode = 2

  def __printSteeringFile(self, parser):
    """print the parameters formated as a steering file"""

    steeringFileBase = """from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import mm, GeV, MeV
SIM = DD4hepSimulation()

"""
    optionDict = parser._option_string_actions
    parameters = vars(self)
    for parName, parameter in sorted(list(parameters.items()), key=sortParameters):
      if parName.startswith("_"):
        continue
      if isinstance(parameter, ConfigHelper):
        steeringFileBase += "\n\n"
        steeringFileBase += "################################################################################\n"
        steeringFileBase += "## %s \n" % "\n## ".join(parameter.__doc__.splitlines())
        steeringFileBase += "################################################################################\n"
        options = parameter.getOptions()
        for opt, optionsDict in sorted(six.iteritems(options), key=sortParameters):
          if opt.startswith("_"):
            continue
          parValue = optionsDict['default']
          if isinstance(optionsDict.get('help'), six.string_types):
            steeringFileBase += "\n## %s\n" % "\n## ".join(optionsDict.get('help').splitlines())
          # add quotes if it is a string
          if isinstance(parValue, six.string_types):
            steeringFileBase += "SIM.%s.%s = \"%s\"\n" % (parName, opt, parValue)
          else:
            steeringFileBase += "SIM.%s.%s = %s\n" % (parName, opt, parValue)
      else:
        # get the docstring from the command line parameter
        optionObj = optionDict.get("--" + parName, None)
        if isinstance(optionObj, argparse._StoreAction):
          steeringFileBase += "## %s\n" % "\n## ".join(optionObj.help.splitlines())
        # add quotes if it is a string
        if isinstance(parameter, six.string_types):
          steeringFileBase += "SIM.%s = \"%s\"" % (parName, str(parameter))
        else:
          steeringFileBase += "SIM.%s = %s" % (parName, str(parameter))
        steeringFileBase += "\n"
    for line in steeringFileBase.splitlines():
      print(line)

  def _consistencyChecks(self):
    """Check if the requested setup makes sense, or if there is something preventing it from working correctly

    Appends error messages to self._errorMessages

    :returns: None
    """

    if not self.compactFile:
      self._errorMessages.append("ERROR: No geometry compact file provided")

    if self.runType == "batch":
      if not self.numberOfEvents:
        self._errorMessages.append("ERROR: Batch mode requested, but did not set number of events")
      if not self.inputFiles and not self.enableGun:
        self._errorMessages.append("ERROR: Batch mode requested, but did not set inputFile(s) or gun")

    if self.inputFiles and (self.enableG4Gun or self.enableG4GPS):
      self._errorMessages.append("ERROR: Cannot use both inputFiles and Geant4Gun or GeneralParticleSource")

    if self.enableGun and (self.enableG4Gun or self.enableG4GPS):
      self._errorMessages.append("ERROR: Cannot use both DD4hepGun and Geant4 Gun or GeneralParticleSource")

    if self.numberOfEvents < 0 and not self.inputFiles:
      self._errorMessages.append("ERROR: Negative number of events only sensible for inputFiles")

  def _enablePrimaryHandler(self):
    """ the geant4 Gun or GeneralParticleSource cannot be used together with the PrimaryHandler.
        Particles would be simulated multiple times

    :returns: True or False
    """
    enablePrimaryHandler = not (self.enableG4Gun or self.enableG4GPS)
    if enablePrimaryHandler:
      logger.info("Enabling the PrimaryHandler")
    else:
      logger.info("Disabling the PrimaryHandler")
    return enablePrimaryHandler

  def _buildInputStage(self, simple, generator_input_modules, output_level=None, have_mctruth=True):
    """
    Generic build of the input stage with multiple input modules.
    Actions executed are:
    1) Register Generation initialization action
    2) Append all modules to build the complete input record
    These modules are readers/particle sources, boosters and/or smearing actions.
    3) Merge all existing interaction records
    4) Add the MC truth handler
    """
    from DDG4 import GeneratorAction
    ga = simple.kernel().generatorAction()

    # Register Generation initialization action
    gen = GeneratorAction(simple.kernel(), "Geant4GeneratorActionInit/GenerationInit")
    if output_level is not None:
      gen.OutputLevel = output_level
    ga.adopt(gen)

    # Now append all modules to build the complete input record
    # These modules are readers/particle sources, boosters and/or smearing actions
    for gen in generator_input_modules:
      gen.enableUI()
      if output_level is not None:
        gen.OutputLevel = output_level
      ga.adopt(gen)

    # Merge all existing interaction records
    gen = GeneratorAction(simple.kernel(), "Geant4InteractionMerger/InteractionMerger")
    gen.enableUI()
    if output_level is not None:
      gen.OutputLevel = output_level
    ga.adopt(gen)

    # Finally generate Geant4 primaries
    if have_mctruth:
      gen = GeneratorAction(simple.kernel(), "Geant4PrimaryHandler/PrimaryHandler")
      gen.RejectPDGs = ConfigHelper.makeString(self.physics.rejectPDGs)
      gen.ZeroTimePDGs = ConfigHelper.makeString(self.physics.zeroTimePDGs)
      gen.enableUI()
      if output_level is not None:
        gen.OutputLevel = output_level
      ga.adopt(gen)
    # Puuuhh! All done.
    return None


################################################################################
# MODULE FUNCTIONS GO HERE
################################################################################


def sortParameters(key):
  from functools import cmp_to_key

  def _sortParameters(parA, parB):
    """sort the parameters by name: first normal parameters, then set of
    parameters based on ConfigHelper objects
    """
    parTypeA = parA[1]
    parTypeB = parB[1]
    if isinstance(parTypeA, ConfigHelper) and isinstance(parTypeB, ConfigHelper):
      return 1 if str(parA[0]) > str(parB[0]) else -1
    elif isinstance(parTypeA, ConfigHelper):
      return 1
    elif isinstance(parTypeB, ConfigHelper):
      return -1
    else:
      return 1 if str(parA[0]) > str(parB[0]) else -1

  return cmp_to_key(_sortParameters)(key)


def getOutputLevel(level):
  """return output.LEVEL"""
  from DDG4 import OutputLevel
  levels = {1: OutputLevel.VERBOSE,
            2: OutputLevel.DEBUG,
            3: OutputLevel.INFO,
            4: OutputLevel.WARNING,
            5: OutputLevel.ERROR,
            6: OutputLevel.FATAL,
            7: OutputLevel.ALWAYS}
  return levels[level]
