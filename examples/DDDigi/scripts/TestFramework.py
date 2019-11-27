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
from __future__ import absolute_import, unicode_literals
import os
import DDDigi


def make_input(kernel):
  input_1 = DDDigi.TestAction(kernel, 'input_01', 100)
  input_2 = DDDigi.TestAction(kernel, 'input_02', 200)
  input_3 = DDDigi.TestAction(kernel, 'input_03', 150)
  input_4 = DDDigi.TestAction(kernel, 'input_04', 60)
  seq  = kernel.inputAction()
  seq.adopt(input_1)
  seq.adopt(input_2)
  seq.adopt(input_3)
  seq.adopt(input_4)
  return seq


def make_subdetector(kernel, name):
  action_1 = DDDigi.TestAction(kernel, name+'_deposits',  150)
  action_2 = DDDigi.TestAction(kernel, name+'_rndmNoise', 100)
  action_3 = DDDigi.TestAction(kernel, name+'_deadChan',  100)
  action_4 = DDDigi.TestAction(kernel, name+'_noiseChan',  50)
  action_5 = DDDigi.TestAction(kernel, name+'_merge',     200)
  seq = DDDigi.ActionSequence(kernel, 'DigiActionSequence/'+name+'_sequence', True)
  seq.adopt(action_1)
  seq.adopt(action_2)
  seq.adopt(action_3)
  seq.adopt(action_4)
  seq.adopt(action_5)
  return seq


def run():
  # import pdb
  # pdb.set_trace()
  DDDigi.setPrintFormat(str('%-32s %5s %s'))
  kernel = DDDigi.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  fname = "file:" + install_dir + "/examples/ClientTests/compact/MiniTel.xml"
  kernel.loadGeometry(str(fname))
  kernel.printProperties()
  digi = DDDigi.Digitize(kernel)
  digi.printDetectors()

  event_processor = DDDigi.Synchronize(kernel, 'DigiSynchronize/MainDigitizer', True)
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
  output = DDDigi.TestAction(kernel, 'output_01', 200)
  kernel.outputAction().adopt(output)

  DDDigi.setPrintLevel(DDDigi.OutputLevel.DEBUG)
  kernel.numThreads = 0   # = number of concurrent threads
  kernel.numEvents = 10
  kernel.maxEventsParallel = 3
  kernel.run()
  DDDigi.setPrintLevel(DDDigi.OutputLevel.INFO)


if __name__ == '__main__':
  run()
