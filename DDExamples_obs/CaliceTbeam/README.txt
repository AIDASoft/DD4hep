A driver for CALICE tesbeam box prototype.
Examples of compact for individual SiwEcal, Ahcal, DHcal and Tcmt.
Example of compact for testbeam with SiwEcal, Ahcal and Tcmt.


[1] ./bin/ILDExSimu -i  file:../DDExamples/CaliceTbeam/compact/Calice_testbeam.xml   file:../DDExamples/CaliceTbeam/compact/geant4SD.xml 
    ./bin/ILDExSimu  file:../DDExamples/CaliceTbeam/compact/Calice_testbeam.xml   file:../DDExamples/CaliceTbeam/compact/geant4SD.xml ../DDExamples/CaliceTbeam/run_g4.mac
[2] geoConverter -compact2lcdd -input Calice_testbeam.xml -output Calice_testbeam.lcdd
[2] slic -n -g Calice_testbeam.lcdd

