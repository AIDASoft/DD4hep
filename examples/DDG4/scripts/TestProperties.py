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
import DDG4
import os
from g4units import GeV, m
#
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():

  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/SiliconBlock.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)

  geant4 = DDG4.Geant4(kernel)
  ui = geant4.setupCshUI(vis=None)

  # Now the calorimeters
  act = DDG4.Action(kernel, str('PropertyTestAction/Test'))
  act.prop_str = 'Hello World!'
  act.prop_bool = True
  act.prop_int = 1234
  act.prop_long = 3456
  act.prop_ulong = 4567
  act.prop_float = 1234567.8
  act.prop_double = 1234567.8
  act.prop_XYZPoint = (1, 2, 3)
  act.prop_XYZVector = (1 * m, 2 * m, 3 * m)
  act.prop_PxPyPzEVector = (1 * GeV, 2 * GeV, 3 * GeV, 4 * GeV)

  act.map_str_str = {'a': 'AA', 'b': 'BB', 'c': 'CC'}
  act.map_str_bool = {'a': 1, 'b': 0, 'c': 1}
  act.map_str_int = {'a': 11, 'b': 22, 'c': 33}
  act.map_str_long = {'a': 111, 'b': 222, 'c': 333}
  #  act.map_str_ulong = {'a': 1111, 'b': 2222, 'c': 3333}
  act.map_str_float = {'a': 11.11, 'b': 22.22, 'c': 33.33}
  act.map_str_double = {'a': 11.111, 'b': 22.222, 'c': 33.333}

  act.map_int_str = {100: 'AA', 200: 'BB', 300: 'CC'}
  act.map_int_bool = {100: 1, 200: 0, 300: 1}
  act.map_int_int = {100: 11, 200: 22, 300: 33}
  act.map_int_long = {100: 111, 200: 222, 300: 333}
  #  act.map_int_ulong = {100: 1111, 200: 2222, 300: 3333}
  act.map_int_float = {100: 11.11, 200: 22.22, 300: 33.33}
  act.map_int_double = {100: 11.111, 200: 22.222, 300: 33.333}

  act.map_int_str = {100 * 10: 'AA', 200 * 10: 'BB', 300 * 10: 'CC'}
  act.map_int_bool = {100 * 10: 1, 200 * 10: 0, 300 * 10: 1}
  act.map_int_int = {100 * 10: 11, 200 * 10: 22, 300 * 10: 33}
  act.map_int_long = {100 * 10: 111, 200 * 10: 222, 300 * 10: 333}
  #  act.map_int_ulong = {100 * 10: 1111, 200 * 10: 2222, 300 * 10: 3333}
  act.map_int_float = {100 * 10: 11.11, 200 * 10: 22.22, 300 * 10: 33.33}
  act.map_int_double = {100 * 10: 11.111, 200 * 10: 22.222, 300 * 10: 33.333}

  act.set_str = ['aa', 'bb', 'cc' ,'dd']
  act.set_bool = [0,0,0,1,1,1]
  act.set_int = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.set_long = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  #  act.set_ulong = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.set_float = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.set_double = [0 * m, 1 * m, 2 * m, 3 * m, 4 * m, 5 * m, 6 * m, 7 * m, 8 * m, 8 * m, 8 * m]
  act.set_XYZPoint = [(1,2,3),(11,22,33),(111,222,333),(1111,2222,3333)]
  act.set_XYZVector = [(1,2,3),(11,22,33),(111,222,333),(1111,2222,3333)]
  act.set_PxPyPzEVector = [(1 * GeV,2 * GeV,3 * GeV,4 * GeV),
                           (11 * GeV,22 * GeV,33 * GeV,44 * GeV),
                           (111 * GeV,222 * GeV,333 * GeV,444 * GeV)]

  act.list_str = ['aa', 'bb', 'cc' ,'dd']
  act.list_bool = [0,0,0,1,1,1]
  act.list_int = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.list_long = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.list_ulong = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.list_float = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.list_double = [0 * m, 1 * m, 2 * m, 3 * m, 4 * m, 5 * m, 6 * m, 7 * m, 8 * m, 8 * m, 8 * m]
  act.list_XYZPoint = [(1,2,3),(11,22,33),(111,222,333),(1111,2222,3333)]
  act.list_XYZVector = [(1,2,3),(11,22,33),(111,222,333),(1111,2222,3333)]
  act.list_PxPyPzEVector = [(1 * GeV, 2 * GeV, 3 * GeV,4 * GeV),
                            (11 * GeV,22 * GeV,33 * GeV,44 * GeV),
                            (111 * GeV,222 * GeV,333 * GeV,444 * GeV)]

  act.vector_str = ['aa', 'bb', 'cc' ,'dd']
  act.vector_bool = [0,0,0,1,1,1]
  act.vector_int = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.vector_long = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.vector_ulong = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.vector_float = [0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8]
  act.vector_double = [0 * m, 1 * m, 2 * m, 3 * m, 4 * m, 5 * m, 6 * m, 7 * m, 8 * m, 8 * m, 8 * m]
  act.vector_XYZPoint = [(1,2,3),(11,22,33),(111,222,333),(1111,2222,3333)]
  act.vector_XYZVector = [(1,2,3),(11,22,33),(111,222,333),(1111,2222,3333)]
  act.vector_PxPyPzEVector = [(1 * GeV,2 * GeV,3 * GeV,4 * GeV),
                              (11 * GeV,22 * GeV,33 * GeV,44 * GeV),
                              (111 * GeV,222 * GeV,333 * GeV,444 * GeV)]

  #  Check read access:
  print('--> {value}'.format(value='-------------------------------------------------------'))
  print('--> {value}'.format(value=str(act.prop_str)))              #     'Hello World!'
  print('--> {value}'.format(value=str(act.prop_bool)))             #     True
  print('--> {value}'.format(value=str(act.prop_int)))              #     1234
  print('--> {value}'.format(value=str(act.prop_float)))            #     1234567.8
  print('--> {value}'.format(value=str(act.prop_double)))           #     1234567.8
  print('--> {value}'.format(value=str(act.prop_XYZPoint)))         #     (1,2,3)
  print('--> {value}'.format(value=str(act.prop_XYZVector)))        #     (1 * m,2 * m,3 * m)
  print('--> {value}'.format(value=str(act.prop_PxPyPzEVector)))    #     (1 * GeV,2 * GeV,3 * GeV,4 * GeV)
  print('--> {value}'.format(value='-------------------------------------------------------'))

  act.enableUI()

  phys = geant4.setupPhysics('FTFP_BERT')
  phys.dump()
  ui.Commands = [
    '/ddg4/Test/show',
    '/ddg4/Test/dumpProperties',
    '/ddg4/UI/exit'
  ]
  kernel.NumEvents = 0
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
