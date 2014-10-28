DDG4 setup
----------
Please note:
examples, which are regularly checked and should work
are 
-- CLICSidAClick.C
-- CLIDSidXML.C
-- CLICSidSimu.py

Other AClick and python files are for temporary tests and are not supposed 
to work 'out of the box'.


Starting Geant4 CLICSiD example as ROOT AClick
----------------------------------------------
- First adapt the initAClick script to fit your include/library pathes
- load the script in root: .x initAClick.C
- Then start the configuration in compiled mode in ROOT
  .L CLICSidAClick.C+
- See also instructions in CLICSidAClick.C

Other AClick files are for temporary tests and are not supposed 
to work under all circumstances.

Once you get the Geant4 interactive prompt, enter:
/run/beamOn 1
1 = number of events to be simulated.


Starting Geant4 CLICSiD example from XML
----------------------------------------
- First adapt the initAClick script to fit your include/library pathes
- load the script in root: .x initAClick.C
- Then start the configuration in compiled mode in ROOT
  .L CLICSidXML.C+
- See also instructions in CLICSidXML.C

Once you get the Geant4 interactive prompt, enter:
/run/beamOn 1
1 = number of events to be simulated.

If you do not want to run from the ROOT prompt, you could
also compile and link this script to a standalone main program.


Starting Geant4 CLICSiD example from python
--------------------------------------------
$> python <installation-directory>/examples/DDG4/examples/CLICSidSimu.py

Once you get the Geant4 interactive prompt, enter:
/run/beamOn 1
1 = number of events to be simulated.
