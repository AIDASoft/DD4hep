// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_CONDITIONVALIDATOR_H
#define DD4HEP_GEOMETRY_CONDITIONVALIDATOR_H

// Framework includes
#include "DDCond/Condition.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

//DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionObject);

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class ConditionValidator  ConditionValidator.h DD4hep/ConditionValidator.h
     * 
     * @author  M.Frank
     * @version 1.0
     */
    class ConditionValidator  {
    public:
  
      /// Standard constructor
      ConditionValidator();
      /// Validate detector subtree and return conditions out of date
      bool validate(DetElement subtree, std::vector<Condition>& tobe_updated);

    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITIONVALIDATOR_H    */
