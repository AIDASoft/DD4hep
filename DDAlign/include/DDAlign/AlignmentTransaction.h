// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTTRANSACTION_H
#define DD4HEP_ALIGNMENT_ALIGNMENTTRANSACTION_H

// Framework include files
#include "DD4hep/Primitives.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  namespace XML { class Handle_t; }

  /*
   *   Alignment namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class AlignmentCache;

    /** @class AlignmentTransaction
     *
     *  Manage alignment transaction to the cache for a given LCDD instance
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    01/04/2014
     */
    struct AlignmentTransaction {
      /// Internal flag to remember transaction contexts
      bool flag;
      /// Reference to the current LCDD instance
      LCDD& lcdd;
      /// Reference to the alignment cache
      AlignmentCache* m_cache;
      
      /// Default constructor
      AlignmentTransaction(LCDD& l, const XML::Handle_t& e);
      /// Default destructor
      ~AlignmentTransaction();
    };

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_ALIGNMENT_ALIGNMENTTRANSACTION_H       */
