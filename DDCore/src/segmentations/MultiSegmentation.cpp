//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  Created: Jun 28, 2013
//  Author:  Christian Grefe, CERN
//
//==========================================================================

/// Framework include files
#include <DDSegmentation/MultiSegmentation.h>
#include <DD4hep/Printout.h>

/// C/C++ include files
#include <iomanip>
#include <stdexcept>

namespace dd4hep {

  namespace DDSegmentation {

    /// default constructor using an encoding string
    MultiSegmentation::MultiSegmentation(const std::string& cellEncoding)
      :	Segmentation(cellEncoding), m_discriminator(0), m_debug(0)
    {
      // define type and description
      _type        = "MultiSegmentation";
      _description = "Multi-segmenation wrapper segmentation";
      //registerParameter<int>("debug", "Debug flag", m_debug, 0);
      registerParameter<std::string>("key", "Diskriminating field", m_discriminatorId, "");
    }

    /// Default constructor used by derived classes passing an existing decoder
    MultiSegmentation::MultiSegmentation(const BitFieldCoder* decode)
      :	Segmentation(decode), m_discriminator(0), m_debug(0)
    {
      // define type and description
      _type        = "MultiSegmentation";
      _description = "Multi-segmenation wrapper segmentation";
      //registerParameter<int>("debug", "Debug flag", m_debug, 0);
      registerParameter<std::string>("key", "Diskriminating field", m_discriminatorId, "");
    }

    /// destructor
    MultiSegmentation::~MultiSegmentation() {
      for(Segmentations::iterator i=m_segmentations.begin(); i!=m_segmentations.end(); ++i)
        delete (*i).segmentation;
      m_segmentations.clear();
    }

    /// Add subsegmentation. Call only valid for Multi-segmentations. Default implementation throws an exception
    void MultiSegmentation::addSubsegmentation(long key_min, long key_max, Segmentation* entry)  {
      Entry e;
      e.key_min = key_min;
      e.key_max = key_max;
      e.segmentation = entry;
      m_segmentations.emplace_back(e);
    }

    /// Set the underlying decoder
    void MultiSegmentation::setDecoder(const BitFieldCoder* newDecoder) {
      this->Segmentation::setDecoder(newDecoder);
      for(Segmentations::iterator i=m_segmentations.begin(); i != m_segmentations.end(); ++i)
        (*i).segmentation->setDecoder(newDecoder);
      m_discriminator = &((*_decoder)[m_discriminatorId]);
    }

    /// 
    const Segmentation& MultiSegmentation::subsegmentation(const CellID& cID)   const  {
      if ( m_discriminator )  {
        long seg_id = m_discriminator->value(cID);
        for(Segmentations::const_iterator i=m_segmentations.begin(); i != m_segmentations.end(); ++i)  {
          const Entry& e = *i; 
          if ( e.key_min<= seg_id && e.key_max >= seg_id )  {
            Segmentation* s = e.segmentation;
            if ( m_debug > 0 )  {
              printout(ALWAYS,"MultiSegmentation","Id: %04X %s", seg_id, s->name().c_str());
              const Parameters& pars = s->parameters();
              for( const auto* p : pars )  {
                printout(ALWAYS,"MultiSegmentation"," Param  %s = %s",
                         p->name().c_str(), p->value().c_str());
              }
            }
            return *s;
          }
        }
      }
      except("MultiSegmentation", "Invalid sub-segmentation identifier!");
      throw std::string("Invalid sub-segmentation identifier!");
    }
     
    /// determine the position based on the cell ID
    Vector3D MultiSegmentation::position(const CellID& cID) const {
      return subsegmentation(cID).position(cID);
    }

    /// determine the cell ID based on the position
    CellID MultiSegmentation::cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& vID) const {
      return subsegmentation(vID).cellID(localPosition, globalPosition, vID);
    }

    std::vector<double> MultiSegmentation::cellDimensions(const CellID& cID) const {
      return subsegmentation(cID).cellDimensions(cID);
    }

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
