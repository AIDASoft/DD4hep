// $Id: $
/* Module Describtion for Telescope
 * Christoph Hombach
 */
// Include files
#include "DD4hep/Detector.h"


namespace DD4hep {
  struct TestBox : public Geometry::DetElement 
  {
    typedef Geometry::Ref_t Ref_t;
    TestBox(const Ref_t& e) : Geometry::DetElement(e) {}
    TestBox(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id);
    
    

  }
    ;
}
