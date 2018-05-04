import os, sys
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
def run():
  from MiniTelSetup import Setup as MiniTel
  m = MiniTel()

  if len(sys.argv) >= 2 and sys.argv[1] =="batch":
    m.kernel.UI = ''

  m.configure()
  m.defineOutput()
  m.setupGun()
  m.setupGenerator()
  m.setupPhysics()
  m.run()

if __name__ == "__main__":
  run()
