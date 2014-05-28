#ifndef EvNavHandler_h
#define EvNavHandler_h


void next_event() ;

class EvNavHandler {
public:
  void Fwd() {
    next_event();
  }
  void Bck() {}
};


#endif


