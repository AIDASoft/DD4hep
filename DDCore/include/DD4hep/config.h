// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_CONFIG_H
#define DD4HEP_CONFIG_H

#define DD4HEP_INSTANCE_COUNTS
#ifdef DD4HEP_INSTANCE_COUNTS
#define INCREMENT_COUNTER InstanceCount::increment(this)
#define DECREMENT_COUNTER InstanceCount::decrement(this)
#else
#define INCREMENT_COUNTER 
#define DECREMENT_COUNTER 
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

  } /* End namespace Geometry  */
} /*   End namespace DD4hep    */
#endif    /* DD4HEP_CONFIG_H         */
