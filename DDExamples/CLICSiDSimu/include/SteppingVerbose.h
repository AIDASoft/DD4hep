#ifndef SteppingVerbose_h
#define SteppingVerbose_h 1

class SteppingVerbose;

#include "G4SteppingVerbose.hh"

class SteppingVerbose : public G4SteppingVerbose  {
 public:   
   SteppingVerbose();
  ~SteppingVerbose();

   void StepInfo();
   void TrackingStarted();

};
#endif
