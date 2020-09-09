//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#ifndef DDREC_IMATERIAL_H
#define DDREC_IMATERIAL_H

#include <string>
#include <ostream>

namespace dd4hep { namespace rec {
  
  /** Interface for material description for tracking. 
   *
   * @author F.Gaede, DESY
   * @version $Id$
   * @date Apr 6 2014
   */
  
  class IMaterial {

  protected:    
    /// Assignment operator
    IMaterial& operator=(const IMaterial&) { return *this; }
    
  public:
    
    /// Destructor
    virtual ~IMaterial() {}

    /// material name
    virtual std::string name() const =0 ;

    /// averaged atomic number
    virtual double A() const =0 ;

    /// averaged proton number
    virtual double Z() const =0 ;
    
    /// density - units ?
    virtual double density() const =0 ;
    
    /// radiation length - units ?
    virtual double radiationLength() const =0 ;
    
    /// interaction length - units ?
    virtual double interactionLength() const =0 ;

  };

 /// dump IMaterial operator 
  inline std::ostream& operator<<( std::ostream& os , const IMaterial& m ) {

    os << "  " << m.name() << ", A: " << m.A()  << ", Z: " << m.Z() << ", density: " << m.density() << ", radiationLength: " <<  m.radiationLength() 
       << ", interactionLength: " << m.interactionLength() ;

    return os ;
  }

  } } /* namespace rec */



#endif // DDREC_IMATERIAL_H
