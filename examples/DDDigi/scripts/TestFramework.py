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
import dddigi
import os


def make_input(kernel):
  input_1 = dddigi.TestAction(kernel, 'input_01', 50)
  input_2 = dddigi.TestAction(kernel, 'input_02', 100)
  input_3 = dddigi.TestAction(kernel, 'input_03', 75)
  input_4 = dddigi.TestAction(kernel, 'input_04', 30)
  seq = kernel.inputAction()
  seq.adopt(input_1)
  seq.adopt(input_2)
  seq.adopt(input_3)
  seq.adopt(input_4)
  return seq


def make_subdetector(kernel, name):
  action_1 = dddigi.TestAction(kernel, name + '_deposits', 75)
  action_2 = dddigi.TestAction(kernel, name + '_rndmNoise', 50)
  action_3 = dddigi.TestAction(kernel, name + '_deadChan', 50)
  action_4 = dddigi.TestAction(kernel, name + '_noiseChan', 25)
  action_5 = dddigi.TestAction(kernel, name + '_merge', 60)
  seq = dddigi.Action(kernel, 'DigiActionSequence/' + name + '_sequence', parallel=True)
  seq.adopt(action_1)
  seq.adopt(action_2)
  seq.adopt(action_3)
  seq.adopt(action_4)
  seq.adopt(action_5)
  return seq


def run():
  # import pdb
  # pdb.set_trace()
  dddigi.setPrintFormat(str('%-32s %5s %s'))
  kernel = dddigi.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  fname = "file:" + install_dir + "/examples/ClientTests/compact/MiniTel.xml"
  kernel.loadGeometry(str(fname))
  digi = dddigi.Digitize(kernel)
  digi.printDetectors()

  event_processor = dddigi.Action(kernel, 'DigiSynchronize/MainDigitizer')
  event_processor.parallel = True
  # input
  make_input(kernel)
  # Subdetector digitization
  dets = digi.activeDetectors()
  for d in dets:
    seq = make_subdetector(kernel, d['name'])
    event_processor.adopt(seq)
  kernel.eventAction().adopt(event_processor)
  # Output
  output = dddigi.TestAction(kernel, 'output_01', 50)
  kernel.outputAction().adopt(output)

  dddigi.setPrintLevel(dddigi.OutputLevel.DEBUG)
  kernel.numThreads = 0   # = number of concurrent threads
  kernel.numEvents = 5
  kernel.maxEventsParallel = 3
  kernel.run()
  dddigi.setPrintLevel(dddigi.OutputLevel.INFO)


if __name__ == '__main__':
  run()
