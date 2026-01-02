"""Helper object for random number generator objects"""

from DDSim.Helper.ConfigHelper import ConfigHelper
import random
import logging
import threading

logger = logging.getLogger(__name__)

# Thread-local counter for EventSeeder naming
_eventseed_counter = threading.local()
# Thread-local counter for Geant4Random naming in workers
_random_counter = threading.local()

def _get_eventseed_id():
  """Get a unique ID for EventSeeder in this thread"""
  if not hasattr(_eventseed_counter, 'value'):
    _eventseed_counter.value = 0
  else:
    _eventseed_counter.value += 1
  return _eventseed_counter.value

def _get_random_id():
  """Get a unique ID for Geant4Random in this thread"""
  if not hasattr(_random_counter, 'value'):
    _random_counter.value = 0
  else:
    _random_counter.value += 1
  return _random_counter.value


class Random (ConfigHelper):
  """Properties for the random number generator"""

  def __init__(self):
    super(Random, self).__init__()
    self.seed = None
    self.type = None
    self.luxury = 1
    self.replace_gRandom = True
    self.file = None
    self._random = None
    self._eventseed = None

    self._enableEventSeed_EXTRA = {'help': "If True, calculate random seed for each event based"
                                           "on eventID and runID\nAllows reproducibility even when"
                                           "SkippingEvents"}
    self.enableEventSeed = False
    self._closeProperties()

  def initialize(self, DDG4, kernel, output):
    """ initialize the random generator

    :param DDG4: DDG4 module
    :param kernel: Geant4 kernel
    :param int output: output level
    :returns: Geant4Random instance
    """
    if self._random:
      return self._random
    self._random = DDG4.Action(kernel, 'Geant4Random/R1')

    if self.seed is None:
      # System provided random source, truely random according to documentation
      self.seed = random.SystemRandom().randint(0, 2**31 - 1)
      logger.info("Choosing random seed for you: %s", self.seed)
    else:
      # Ensure seed is an integer (may be string from command line)
      self.seed = int(self.seed)

    self._random.Seed = self.seed
    self._random.Luxury = self.luxury

    if self.type is not None:
      self._random.Type = self.type

    self._random.initialize()

    # EventSeeder initialization is deferred to setupEventSeeder()
    # which is called during worker initialization in MT mode
    # or can be called immediately in ST mode

    # Needs to be called after initilisation
    if output <= 3:
      self._random.showStatus()
    return self._random

  def setupEventSeeder(self, DDG4, kernel):
    """Setup EventSeeder action for per-event random seeding.
    
    This method should be called during worker initialization in MT mode,
    or after initialize() in ST mode. In MT mode, this is called once per
    worker thread, creating an independent EventSeeder for each worker.
    
    :param DDG4: DDG4 module
    :param kernel: Geant4 kernel (master kernel in ST mode, worker kernel in MT mode)
    """
    if self.seed is not None and self.enableEventSeed:
      # Only create EventSeeder once (check if already created)
      if self._eventseed is not None:
        logger.debug("EventSeeder already created, skipping")
        return self._eventseed
      
      # Create EventSeeder - use shared=True for MT mode to avoid multiple instances
      seeder_name = 'Geant4EventSeed/EventSeeder'
      eventseed = DDG4.RunAction(kernel, seeder_name, shared=True)
      # Explicitly add to run action sequence
      kernel.runAction().add(eventseed)
      logger.info("EventSeeder created and added to run action")
      # Store reference
      self._eventseed = eventseed
      return eventseed
    return None

  def initializeWorker(self, DDG4, kernel, output):
    """Initialize Geant4Random for a worker thread in MT mode.
    
    In MT mode, each worker thread needs its own Geant4Random instance.
    We use the same initial seed as the master so that EventSeeder computes
    the same per-event seeds across all workers.
    
    :param DDG4: DDG4 module
    :param kernel: Worker kernel
    :param int output: output level
    :returns: Geant4Random instance for this worker
    """
    # Create a Geant4Random wrapper for this worker
    random_id = _get_random_id()
    random_name = 'Geant4Random/R_worker_%d' % random_id
    worker_random = DDG4.Action(kernel, random_name)
    
    # Use the same seed as master so EventSeeder computes identical event seeds
    worker_random.Seed = self.seed
    worker_random.Luxury = self.luxury
    worker_random.Replace_gRandom = self.replace_gRandom
    
    if self.type is not None:
      worker_random.Type = self.type
    
    # Initialize the worker's random engine
    worker_random.initialize()
    
    if output <= 3:
      worker_random.showStatus()
    
    logger.info("Initialized Geant4Random for worker (thread-local ID %d) with seed %d", 
                random_id, self.seed)
    return worker_random
