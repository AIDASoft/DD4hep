//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_GEOMETRY_ALIGNMENTSNOMINALMAP_H
#define DD4HEP_GEOMETRY_ALIGNMENTSNOMINALMAP_H

// Framework include files
#include "DD4hep/ConditionsMap.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// An implementation of the ConditionsMap interface to fall back to nominal alignment.
  /**
   *  The AlignmentsNominalMap is not a conditions cache per se. This implementation 
   *  behaves like a conditionsmap, but it shall not return real conditions to the user,
   *  but rather return the default alignment objects (which at the basis are conditions 
   *  as well) to the user. These alignments are taken from the DetElement in question
   *  Alignment DetElement::nominal().
   *
   *  The basic idea is to enable users to write code "as if" there would be conditions
   *  present. This is important to ease in the lifetime of the experiment the step from
   *  the design phase (where obviously no conditions are taken into account) to a more
   *  mature phase, where alignment studies etc. actually are part of the 
   *  "bread and butter work".
   *
   *  See dd4hep/ConditionsMap.h for further information.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class AlignmentsNominalMap : public ConditionsMap   {
  public:
    /// Reference to the top detector element
    DetElement  world;
  public:
    /// Standard constructor
    AlignmentsNominalMap(DetElement wrld);
    /// Standard destructor
    virtual ~AlignmentsNominalMap() = default;
    /// Insert a new entry to the map. The detector element key and the item key make a unique global alignments key
    virtual bool insert(DetElement detector, Condition::itemkey_type key, Condition condition)  override;
    /// Interface to access alignments by hash value. The detector element key and the item key make a unique global alignments key
    virtual Condition get(DetElement detector, Condition::itemkey_type key) const  override;
    /// Interface to scan data content of the alignments mapping
    virtual void scan(const Condition::Processor& processor) const  override;

    /** Partial implementations for utilities accessing DetElement alignments      */

    /// No AlignmentsMap overload: Access all alignments within a key range in the interval [lower,upper]
    /** Note: This default implementation uses 
     *        std::vector<Condition> get(DetElement detector,
     *                                  itemkey_type lower,
     *                                  itemkey_type upper)
     *        The performance depends on the concrete implementation of the scan method!
     */
    virtual std::vector<Condition> get(DetElement detector,
                                       Condition::itemkey_type lower,
                                       Condition::itemkey_type upper)  const  override
    {  return this->ConditionsMap::get(detector, lower, upper);                   }
      
    /// Interface to partially scan data content of the alignments mapping
    /** Note: This default implementation assumes unordered containers and hence is
     *        not the most efficient implementation!
     *        Internaly it uses "scan(Processor& processor)"
     *        the subselection hence is linearly depending of the number of elements.
     *        
     *        Using ordered maps with "lower_bound(key)" this can be greatly improved.
     *        See the concrete implementations below.
     */
    virtual void scan(DetElement       detector,
                      Condition::itemkey_type     lower,
                      Condition::itemkey_type     upper,
                      const Condition::Processor& processor) const  override;
  };
}         /* End namespace dd4hep                   */
#endif    /* DD4HEP_GEOMETRY_ALIGNMENTSNOMINALMAP_H        */
