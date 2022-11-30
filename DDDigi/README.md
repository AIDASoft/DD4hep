![DDDigi](../doc/logo_small.png)

DDDigi: The digitization tools of DD4hep
========================================

DDDigi is supposed to interprete the results of the detector simulation phase
e.g. using the simulation toolket [DDG4](../DDG4) with its command line interface
[ddsim](../DDG4/python/DDSim).
This is the final phase of data processing to obtain data from the simulation 
branch which are as close as possible to the response of the front-end electronics
of a real experimental setup.

The simulation phase ejects for all subdetectors and the individual 
sensitive elements contained the energy deposits created by particles from a 
primary interaction passing the sensitive volume.
These data have to processed for taking into accound imperfections of the 
apparatus such as 

- uncertainties in cable lengths resulting in shifts of time of signal arriving.
- uncertainties in the position of sensitive volumes.
- dead or hot readout channels.
- conversion of the final signal data into the equivalent of ADC counts.
- ...

In addition these effects should be computed in the presence of

- multiple interactions in the same beam-crossing
- spillover signal from previous or following interactions due to the
  relaxation time of the apparatus.

DDDigi addresses these issues in a very modular way and implements a multi-threaded approach
to perform such data processing.
To support multi-threading and to avoid any complication for users due to data races 
the following basic assumptions were made:

- The signals from disjunct subdetectors are generally independent
  This means the detector response of any tracking subdetector is uncorrelated with
  the detector response of e.g. a calorimeter.
- Within a subdetector the response of disjunct units are uncorrelated. This means that 
  e.g. hits on one layer of a layered tracking device are uncorrelated 
  with the hits produced in another layer.
- Within one such a layered device the response of individual sensors is tyically uncorrelated.

These assumptions however are not strict, but rather require input from the designer of such
a digitization application for a given appratus. These basic assumption only show the maximal
level of parallelization possible when processing data signals.
The parallelization can be configured for each subdetector at each level according to 
boundary conditions e.g. given by cross-talk or the data volume arising from the
energy deposits of the simulation.



![HORIZON2020](../doc/usermanuals/DD4hep/figures/AIDAinnova.png)
