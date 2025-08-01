#include <cerrno>
#include <DD4hep/Detector.h>

int main(int argc, char** argv) {
  if( argc > 1 )  {
    dd4hep::Detector& desc = dd4hep::Detector::getInstance();
    desc.fromXML( argv[1] );
    auto& det = dd4hep::Detector::getInstance();
    delete &det;
    return 0;
  }
  ::printf("Invalid argumants. Usage: testLoad <geometry XML file>\n\n");
  return EINVAL;
}
