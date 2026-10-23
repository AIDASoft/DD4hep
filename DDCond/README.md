![DDCond](../doc/logo_small.png)

DDCond: The Conditions Data Toolkit of DD4hep
=============================================

In a high energy physics experiment the data originating from particle 
collisions (so called Event-data) in most cases require supplementary, 
mostly  environmental, calibration- or alignment data to extract the physics
content from the recorded event data. These supplementary data are 
time-dependent and typically called conditions. The ability of an 
experiment to produce correct and timely results depends on the complete 
and efficient availability of  needed conditions for each stage of data 
handling. This manual should introduce to the DDCond toolkit, which provides 
access to conditions data within the DD4hep data structures.

The DDCond extensions to the DD4hep toolkit formalize both the access and 
the management to time-dependent data necessary to process the event data.
In this manual we will shortly describe the model used to organize and manage 
the conditions data internally and then describe the user interface to
actually perform the required actions.
These conditions data typically are stored in a database. Nearly every
high energy physics experiment has strong feelings how to efficiently
read and store the conditions data in terms of tablespace organization 
and data format.
For this reason DDCond does not attempt to solve the persistency problem,
but rather defines an interface used to load missing data items from the 
persistent medium. Any persistent data representation, which can fulfill
the requirements of this interface may be adopted by the DDCond 
conditions caching and access mechanism.

At the end of this manual a walk-through of an example is discussed to 
illustrate, which steps have to be performed to use the DD4hep conditions
store within a client application.

The corresponding publication can be found here [EPJ Web of Conferences 214, 02042 (2019)](https://doi.org/10.1051/epjconf/201921402042) or
see the [presentation](https://indico.cern.ch/event/587955/contributions/2937616/attachments/1678648/2709897/DD4hep-CHEP2018.pdf).

For further details please see the user manual in the formats
[HTML](https://dd4hep.web.cern.ch/dd4hep/usermanuals/DDCondManual/DDCondManual.html)
and
[PDF](https://dd4hep.web.cern.ch/dd4hep/usermanuals/DDCondManual/DDCondManual.pdf)

Some usage examples for DDCond can be found [here](../examples/Conditions)


![HORIZON2020](../doc/usermanuals/DD4hep/figures/AIDA-2020.png)
