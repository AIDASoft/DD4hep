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
#ifndef DDDIGI_DIGISEGMENTATIONSPLITTER_H
#define DDDIGI_DIGISEGMENTATIONSPLITTER_H

// Framework include files
#include <DDDigi/DigiActionSequence.h>
#include <DDDigi/DigiData.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// DDSegmentation namespace declaration
  namespace DDSegmentation {
    class BitFieldElement;
  }

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiSegmentContext;
    class DigiSegmentAction;
    class DigiSegmentationSplitter;

    class DigiSegmentContext  {
    public:
      DetElement             detector   { };
      IDDescriptor           idspec     { };
      const BitFieldElement* field      { nullptr };
      uint64_t               cell_mask  { ~0x0UL };
      uint64_t               det_mask   { 0UL };
      uint64_t               split_mask { 0UL };
      int32_t                offset     { 0 };
      int32_t                width      { 0 };
      int32_t                max_split  { 0 };
      uint32_t               id         { 0 };

    public:
      uint32_t split_id(uint64_t cell_id)  const  {  
	return int( (cell_id & this->split_mask) >> this->offset );
      }
      uint64_t cell_id(uint64_t cell_id)  const  {  
	return (cell_id & this->cell_mask) >> this->offset;
      }
      const std::string& name()  const  {
	return this->field->name();
      }
      const char* cname()  const  {
	return this->field->name().c_str();
      }
    };

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiSegmentAction : public DigiEventAction   {
    private:
      friend class DigiSegmentationSplitter;

      /// Implementation declaration
      class internals_t;
      /// Reference to the implementation
      std::unique_ptr<internals_t> internals;

      /// Main functional callback
      virtual void execute(DigiContext& context)  const  final;

      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSegmentAction);

    public:
      /// Standard constructor
      DigiSegmentAction(const DigiKernel& kernel, const std::string& name);

      /// Default destructor
      virtual ~DigiSegmentAction();

      /// Main functional callback
      std::map<Key::key_type, std::any> 
	handleSegment(DigiContext&              context,
		      const DigiSegmentContext& segment,
		      const DepositMapping&     deposits)  const;
    };

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiSegmentationSplitter : public DigiActionSequence   {
    protected:
      /// Implementation declaration
      class internals_t;
      /// Reference to the implementation
      std::unique_ptr<internals_t> internals;

    public:

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSegmentationSplitter);

      /// Default destructor
      virtual ~DigiSegmentationSplitter();

      /// Split actions according to the segmentation
      //void split_segments(DigiEvent& event, DataSegment& data)  const;

      void init_processors();

    public:
      /// Standard constructor
      DigiSegmentationSplitter(const DigiKernel& kernel, const std::string& name);

      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTATIONSPLITTER_H
