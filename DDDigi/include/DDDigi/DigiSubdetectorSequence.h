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
#ifndef DD4HEP_DDDIGI_DIGISUBDETECTORSEQUENCE_H
#define DD4HEP_DDDIGI_DIGISUBDETECTORSEQUENCE_H

// Framework include files
#include <DDDigi/DigiActionSequence.h>
#include <DDDigi/DigiSegmentation.h>
#include <DD4hep/DetElement.h>
#include <DD4hep/Any.h>

/// C/C++ include files
#include <functional>
#include <map>

class TClass;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class CartesianGridXY;
  class CartesianGridXYZ;
  
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiCellScanner;
    class CellDataBase;
    class DigiEventAction;
    class DigiSubdetectorSequence;

    /// Concrete implementation of the Digitization event action sequence
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSubdetectorSequence : public DigiActionSequence {
    protected:
      struct Context  {
        DetElement detector;
        VolumeID   detector_id;
        VolumeID   reverse_id;
        VolumeID   detector_mask;
        std::shared_ptr<DigiCellScanner> scanner;
        Context(DetElement de, VolumeID vid, VolumeID rid, VolumeID mask)
          : detector(de), detector_id(vid), reverse_id(rid), detector_mask(mask) {}
        Context() = delete;
        Context(Context&& copy) = default;
        Context(const Context& copy) = default;
        Context& operator=(Context&& copy) = default;
        Context& operator=(const Context& copy) = default;
      };

      typedef std::map<std::pair<const TClass*,Segmentation>, std::shared_ptr<DigiCellScanner> > Scanners;
      std::string                    m_detectorName;
      std::string                    m_segmentName;
      SensitiveDetector              m_sensDet;
      DetElement                     m_detector;
      IDDescriptor                   m_idDesc;
      Segmentation                   m_segmentation;
      std::map<DetElement, VolumeID> m_parallelDet;
      std::map<VolumeID, Context>    m_parallelVid;
      Scanners                       m_scanners;

      std::function<void(const DigiCellScanner&, const CellDataBase&)> m_cellHandler;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSubdetectorSequence);

      /// Access subdetector from the detector description
      DetElement subdetector(const std::string& name)   const;
      
      /// Access sensitive detector from the detector description
      SensitiveDetector sensitiveDetector(const std::string& nam)   const;

      typedef PlacedVolume::VolIDs             VolIDs;

      void process_cell(const DigiCellScanner& , const CellDataBase& data)  const;
      void scan_detector(DetElement de, VolumeID vid, VolumeID mask);
      void scan_sensitive(PlacedVolume pv, VolumeID vid, VolumeID mask);
      void process_context(const Context& c, PlacedVolume pv, VolumeID vid, VolumeID mask)   const;
      
    public:
      /// Standard constructor
      DigiSubdetectorSequence(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiSubdetectorSequence();
      /// Iniitalize subdetector sequencer
      virtual void initialize();
      /// Begin-of-event callback
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGISUBDETECTORSEQUENCE_H
