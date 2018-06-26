This is an example how to implement a specialized sensitive detector in DDG4.
The sensitive detector is called MyTrackerSD.
As a hit class it uses the Geant4TrackerHit class.

1) The example should illustrate how to setup the factory for a specialized sensitive detector action
2) In the scripts area there is a simple example to illustrate how to setup a small
   simulation and use this factory. (scripts/MyTrackerSD_sim.py)


For issues concerning the MC truth and the (consistent) persistency
please visit the MyTrackerHit include file.

After running DDG4 with the command line:
$> python $DD4hepExamplesINSTALL/examples/DDG4_MySensDet/scripts/MyTrackerSD_siy \
  --compact=file:$DD4hepExamplesINSTALL/examples/ClientTests/compact/SiliconBlock.xml

You can then directly inspect the produced hits and the energy deposits:

$> root.exe
   ------------------------------------------------------------
  | Welcome to ROOT 6.12/06                http://root.cern.ch |
  |                               (c) 1995-2017, The ROOT Team |
  | Built for linuxx8664gcc                                    |
  | From tag v6-12-06, 9 February 2018                         |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

root [0] gSystem->Load("libDDG4_MySensDet.so")
(int) 0
root [1] gSystem->Load("libDDG4Plugins.so")
(int) 0
root [2] TFile* f=TFile::Open("MySD_2018-06-26_19-03.root")
(TFile *) 0x41a4ba0
root [3] TBrowser b
(TBrowser &) Name: Browser Title: ROOT Object Browser
root [4] 
root [4] 

Then click through the browser into the file....
