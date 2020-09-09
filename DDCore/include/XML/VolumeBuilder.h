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
#ifndef XML_VOLUMEBUILDER_H
#define XML_VOLUMEBUILDER_H

// Framework include files
#include "XML/XMLElements.h"
#include "XML/XMLDetector.h"
#include "DD4hep/Detector.h"

// C/C++ include files
#include <set>
#include <map>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {

    /// Declaration of the XML tools namespace
    namespace tools  {

      /// Helper class to build volume and shapes according to ertain patterns in the XML tree.
      /**
       *   1) Shape builder pattern (buildShape): <br>
           @verbatim
           <parent_element>
             <shape name="Shape1" type="Box" dx="Bls:ScintSSizeX" dy="Bls:ScintSSizeY" dz="Bls:ScintSSizeZ"/>
             <shape name="Shape2" type="Box" dx="Bls:ScintMSizeX" dy="Bls:ScintMSizeY" dz="Bls:ScintMSizeZ"/>
             <shape name="Shape3" type="Box" dx="Bls:ScintLSizeX" dy="Bls:ScintLSizeY" dz="Bls:ScintLSizeZ"/>
             ....
             <shape name="ShapeN" type="Trapezoid" z="Bls:FiberCoverH"
                   x1="Bls:ScintLSizeX" y1="Bls:ScintSSizeZ" x2="Bls:ScintSSizeX" y2="Bls:ScintSSizeZ"/>
           <parent_element>

           @endverbatim
       *
       *   2) Volume builder pattern
       *   2.a)  Regular pattern using anonymouns shapes
       *         Call: size_t buildVolumes(xml_h handle);
       *   In the example handle references <detector>
       *
           @verbatim
           <detector id="Bls:ID" name="Bls" type="LHCb_Bcm" 
                     parent="${Bls:Parent}"

             <volume name="lvBlsScintS" material="Bls/BlsSc" vis="Bls:ScintillatorVis">
               <shape type="Box" dx="Bls:ScintSSizeX" dy="Bls:ScintSSizeY" dz="Bls:ScintSSizeZ"/>
             </volume>
             <volume name="lvBlsScintM" material="Bls/BlsSc" vis="Bls:ScintillatorVis">
               <shape type="Box" dx="Bls:ScintMSizeX" dy="Bls:ScintMSizeY" dz="Bls:ScintMSizeZ"/>
             </volume>
             ....
           </detector>
           @endverbatim
       *
       *   2.b)  Regular pattern referencing named shapes (created and registered by buildShapes)
       *         Call: size_t buildVolumes(xml_h handle);
       *   In the example handle references <detector>
       *
           @verbatim
           <detector>
             <volume name="lvBlsScintS1" material="Bls/BlsSc" vis="Bls:ScintillatorVis" shape="Shape1">
             <volume name="lvBlsScintS2" material="Bls/BlsSc" vis="Bls:ScintillatorVis" shape="Shape2">
             <volume name="lvBlsScintS3" material="Bls/BlsSc" vis="Bls:ScintillatorVis" shape="Shape3">
             ....
             <volume name="lvBlsScintSN" material="Bls/BlsSc" vis="Bls:ScintillatorVis" shape="ShapeN">
           </detector>
           @endverbatim
       *   2.c)  Pattern to create named assembly volumes
       *         Call: size_t buildVolumes(xml_h handle);
       *   In the example handle references <detector>
       *
           @verbatim
             <volume name="lvBlsScintS1" vis="Bls:ScintillatorVis" assemply="1">
             <volume name="lvBlsScintS2" vis="Bls:ScintillatorVis" assemply="1">
             <volume name="lvBlsScintS3" vis="Bls:ScintillatorVis" assemply="1">
             ....
             <volume name="lvBlsScintSN" vis="Bls:ScintillatorVis" assemply="1">
           </detector>
           @endverbatim
       *   Alternative
           @verbatim
           <volume name="lvBlsScintS1" vis="Bls:ScintillatorVis">
              <assembly/>
           </volume>
           @endverbatim
       *
       *   Volume placement patterns
       *   3.c)  Pattern to place daughter volumes into parent
       *         void VolumeBuilder::placeDaughters(DetElement parent, Volume vol, xml_h handle) 
       *   In the example handle references <assembly>
       *
           @verbatim
           <assembly name="lvBcmDownMount">
             <physvol logvol="lvBcmDownMountFront">
               <position/>
             </physvol>
             <physvol logvol="lvBcmDownMountBackSection">
               <position z="Bcm:DownMountFrontLength/2 + Bcm:DownMountBackSectionLength/2 + 0.01*mm"/>
               <rotation z="45*degree"/>
             </physvol>
             <physvol logvol="lvBcmDownMountBackSection">
               <position z="Bcm:DownMountFrontLength/2 + Bcm:DownMountBackSectionLength/2 + 0.01*mm"/>
               <rotation z="135*degree"/>
             </physvol>
             <physvol logvol="lvBcmDownMountBackSection">
               <position z="Bcm:DownMountFrontLength/2 + Bcm:DownMountBackSectionLength/2 + 0.01*mm"/>
               <rotation z="225*degree"/>
             </physvol>
             <physvol logvol="lvBcmDownMountBackSection">
               <position z="Bcm:DownMountFrontLength/2 + Bcm:DownMountBackSectionLength/2 + 0.01*mm"/> 
               <rotation z="315*degree"/>
             </physvol>
           </assembly>
           @endverbatim
       *
       *
       *   Please Note:
       *   It is the user's responsability to ensure all reated entities are 
       *   properly registered to the detector description (and TGeo).
       *   No cleanup and referencing is done here!
       *
       *   \author  M.Frank
       *   \version 1.0
       *   \date    12/10/2018
       */
      class VolumeBuilder   {
      public:        
        typedef ::dd4hep::DetElement DetElement;
        typedef ::dd4hep::xml::DetElement xml_det_h;
        typedef std::map<std::string,std::pair<Handle_t,Solid> >       Shapes;
        typedef std::map<std::string,std::pair<Handle_t,Volume> >      Volumes;
        typedef std::map<std::string,Material>                         Materials;
        typedef std::map<std::string,std::pair<Handle_t,Transform3D> > Transformations;
        std::map<std::string, std::unique_ptr<xml::DocumentHolder> > included_docs;
        Detector&             description;
        xml_det_h             x_det;
        int                   id = -1;
        std::string           name;
        DetElement            detector;
        SensitiveDetector     sensitive;
        DetectorBuildType     buildType;
        Shapes                shapes;
        Volumes               volumes;
        Materials             materials;
        Transformations       transformations;
        std::set<std::string> shape_veto, vol_veto;
        bool                  debug = false;

      protected:
        /// Place single volumes
        void _placeSingleVolume(DetElement de, Volume vol, Handle_t c);
        /// Place parametrized volumes
        void _placeParamVolumes(DetElement de, Volume vol, Handle_t c);

      public:
        /// Inhibit default constructor
        VolumeBuilder() = delete;
        /// Inhibit move constructor
        VolumeBuilder(VolumeBuilder&& copy) = delete;
        /// Inhibit copy constructor
        VolumeBuilder(const VolumeBuilder& copy) = delete;
        /// Initializing constructor
        VolumeBuilder(Detector& dsc,
                      Handle_t x_parent,
                      SensitiveDetector sd=SensitiveDetector());
        /// Default constructor
        virtual ~VolumeBuilder() {}
        /// Inhibit move assignment
        VolumeBuilder& operator=(VolumeBuilder&& copy) = delete;
        /// Inhibit copy assignment
        VolumeBuilder& operator=(const VolumeBuilder& copy) = delete;

        /// Collect a set of materials from the leafs of an xml tag
        size_t collectMaterials(Handle_t element);
        
        /// Access element from shape cache by name. Invalid returns means 'veto'. Otherwise exception
        Solid getShape(const std::string& nam)  const;
        /// Create a new shape from the information given in the xml handle
        Solid makeShape(Handle_t handle);
        /// Access a registered volume by name
        Volume volume(const std::string& nam)  const;
        /// Register shape to map
        void registerShape(const std::string& nam, Solid shape);
        /// Register volume to map
        void registerVolume(const std::string& nam, Volume volume);
        /// Build all <shape/> identifiers in the passed parent xml element
        size_t buildShapes(Handle_t handle);
        /// Build all <volume/> identifiers in the passed parent xml element
        size_t buildVolumes(Handle_t handle);
        /// Build all <transformation/> identifiers in the passed parent xml element
        size_t buildTransformations(Handle_t handle);
        /// Load include tags contained in the passed XML handle
        size_t load(Handle_t element, const std::string& tag);
        /// Build all <physvol/> identifiers as PlaceVolume daughters. Ignores structure
        VolumeBuilder& placeDaughters(Volume vol, Handle_t handle);
        /// Build all <physvol/> identifiers as PlaceVolume daughters. Also handles structure
        VolumeBuilder& placeDaughters(DetElement parent, Volume vol, Handle_t handle);
        /// Place the detector object into the mother volume returned by the Detector instance
        PlacedVolume placeDetector(Volume vol);
        PlacedVolume placeDetector(Volume vol, Handle_t handle);
      };
    }     /* End namespace tools            */    
  }       /* End namespace xml              */
}         /* End namespace dd4hep           */
#endif // XML_VOLUMEBUILDER_H
