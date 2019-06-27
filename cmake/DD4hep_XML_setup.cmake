if(DD4HEP_USE_XERCESC)
  find_package( XercesC REQUIRED )
  set_target_properties(XercesC::XercesC
    PROPERTIES
    COMPILE_DEFINITIONS DD4HEP_USE_XERCESC
    INTERFACE_COMPILE_DEFINITIONS DD4HEP_USE_XERCESC
    )
  set(XML_LIBRARIES XercesC::XercesC)
else()
  set ( DD4HEP_USE_XERCESC OFF )
  add_definitions(-DDD4HEP_USE_TINYXML)
  set(XML_LIBRARIES)
endif()
