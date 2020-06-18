
set(CPACK_PACKAGE_NAME "dd4hep")
set(CPACK_SET_DESTDIR TRUE)
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/AIDASoft/dd4hep")
set(CPACK_PACKAGE_DESCRIPTION "Detector Description Toolkit for HEP")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "AIDASoft")
set(CPACK_PACKAGE_VERSION ${dd4hep_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${DD4hep_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${DD4hep_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${DD4hep_VERSION_PATCH})

# required fields for .deb
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "dd4hep@cern.ch")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE_URL})

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

#--- source package settings ---------------------------------------------------
set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_BINARY_DIR}
    "~$"
    "/.git/"
    "/\\\\\\\\.git/"
    "/#"
)
set(CPACK_SOURCE_STRIP_FILES "")

#--- translate buildtype -------------------------------------------------------
string( TOLOWER "${CMAKE_BUILD_TYPE}" buildtype_lower )
if(buildtype_lower STREQUAL "release")
  set(HSF_BUILDTYPE "opt")
elseif(buildtype_lower STREQUAL "debug")
  set(HSF_BUILDTYPE "dbg")
elseif(buildtype_lower STREQUAL "relwithbebinfo")
  set(HSF_BUILDTYPE "owd")
endif()


set(CPACK_PACKAGE_RELOCATABLE True)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "")
set(CPACK_PACKAGE_FILE_NAME "DD4hep_${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}.${DD4hep_VERSION_PATCH}")

include(CPack)
