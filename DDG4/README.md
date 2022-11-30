![DDG4](../doc/logo_small.png)

DDG4: The simulation toolkit of DD4hep
======================================

Simulating the detector response is an essential tool in high energy physics
to analyze the sensitivity of an experiment to the underlying physics.
Such simulation tools require a detailed though convenient detector description as 
it is provided by the DD4hep toolkit DDG4 which uses [Geant4](https://geant4.web.cern.ch/)
to simulate the energy response of a given apparatus.
The toolkit implements a modular and flexible approach to simulation activities using Geant4.
User defined simulation applications using DDG4 can easily be configured, 
extended using specialized action routines.

The design is strongly driven by easy of use;
developers of detector descriptions and applications using
them should provide minimal information and minimal specific
code to achieve the desired result.

DDG4 also supports the fast Geant4 simulation interfaces GFlash and FastSim
to speed up certain simulation procedures.

For further details please see the user manual in the formats
[HTML](https://dd4hep.web.cern.ch/dd4hep/usermanuals/DDG4Manual/DDG4Manual.html)
and
[PDF](https://dd4hep.web.cern.ch/dd4hep/usermanuals/DDG4Manual/DDG4Manual.pdf)

Some usage examples for DDG4 can be found [here](../examples/DDG4)


![HORIZON2020](../doc/usermanuals/DD4hep/figures/AIDA-2020.png)
