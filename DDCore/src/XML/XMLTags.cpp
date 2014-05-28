#include <iostream>
#include "XML/XMLElements.h"
#ifndef __TIXML__
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"

namespace {
  struct __Init {
    __Init() {
      try {
        xercesc::XMLPlatformUtils::Initialize();
      }
      catch (const xercesc::XMLException& e) {
        std::cout << "Xerces-c error in initialization:" << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
      }
    }
    ~__Init() {
      xercesc::XMLPlatformUtils::Terminate();
    }
  };
  __Init  __In__;
}
#else
namespace {
  struct __Init {
    __Init() {
    }
  };
  __Init __In__;
}
#endif

#define UNICODE(x)        extern const Tag_t Unicode_##x (#x)
#include "XML/UnicodeValues.h"

namespace DD4hep {
  namespace XML {

    extern const Tag_t Unicode_NULL("0");
    extern const Tag_t Unicode_empty("");
    extern const Tag_t Unicode_star("*");
    extern const Tag_t Unicode_PI("3.14159265358979323846");
    extern const Tag_t Unicode_TWOPI("6.28318530717958647692");

    void tags_init() {
      static __Init i;
    }

  }
}
