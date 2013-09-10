// $Id: Primitives.h 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DD4HEP_PRIMITIVES_H
#define DD4HEP_DD4HEP_PRIMITIVES_H

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Put here global basic type defintiions derived from primitive types of the DD4hep namespace
  typedef unsigned long long int VolumeID;

  /// Helper to delete objects from heap and reset the pointer. Saves many many lines of code
  template <typename T> inline void deletePtr(T*& p) {
    if ( 0 != p ) delete p;
    p = 0;
  }
  
  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Put here global basic type defintiions derived from primitive types of the Geometry namespace

  }    // End namespace Geometry
}      // End namespace DD4hep
#endif // DD4HEP_DD4HEP_PRIMITIVES_H
