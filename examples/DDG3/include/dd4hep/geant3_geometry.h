//==========================================================================
//  ALEPH software suite
//--------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
//
//  For the licensing terms see OnlineSys/LICENSE.
//
//--------------------------------------------------------------------------
//
//  Author     : Markus Frank
//==========================================================================
#ifndef ALPHA_GEANT3_GEOMETRY_H
#define ALPHA_GEANT3_GEOMETRY_H 1

/// DD4hep include files
#include <DD4hep/Shapes.h>
#include <DD4hep/Volumes.h>
#include <DD4hep/Objects.h>
#include <DD4hep/Detector.h>
#include <DD4hep/DD4hepUnits.h>

/// ROOT include files
#include <TGeoSystemOfUnits.h>
#include <TGeant4SystemOfUnits.h>

/// C/C++ include files
#include <map>
#include <vector>
#include <string>
#include <cstdint>

/// dd4hep namespace declaration
namespace dd4hep   {

  /// Base class implementing all helpers to import Geant3 geometries
  /**
   *   \author  M.Frank
   *   \version 1.0
   */
  class geant3_geometry  {
  public:
    struct mixture_entry { double Z, A, fraction; };
    
    class units  {
    public:
      constexpr static const double cm     = TGeant4Unit::cm;
      constexpr static const double degree = 1.0 / TGeant4Unit::radian;
    };

    using params_t = std::vector<double>;

  public:

    // 
    enum processing_steps_t  {
      PROCESSING_STARTING,
      PROCESSING_MATERIALS,
      PROCESSING_MATRICES,
      PROCESSING_VOLUMES,
      PROCESSING_DIVISIONS,
      PROCESSING_PLACEMENTS,
      PROCESSING_FINISHED,
    };

    //
    struct det_def {
      std::string name, parent, sensitive, item, volid, readout;
      int         id;
    };
    using det_defs = std::map<std::string, det_def>;

    Detector& description;
    std::map<std::string, PlacedVolume> placement_pathes     {         };
    std::map<int32_t,     PlacedVolume> placements           {         };
    std::map<std::string, Volume>       volumes              {         };
    std::map<std::string, TGeoVolume*>  divisions            {         };
    std::map<int, Rotation3D>           rotations            {         };
    std::map<int, Material>             media                {         };
    std::map<int, TGeoMaterial*>        materials            {         };
    std::map<int, std::string>          materials_z          {         };

    std::string                        debug_detector        {         };
    bool                               debug_materials       {   false };
    bool                               debug_media           {   false };
    bool                               debug_volumes         {   false };
    bool                               debug_placements      {   false };
    bool                               debug_divisions       {   false };
    bool                               debug_structure       {   false };
    bool                               debug_enhancements    {   false };
    bool                               debug_sensitive       {   false };

    int32_t resolve_materials  { 1 };
    int32_t processing_step          { PROCESSING_STARTING };

    Detector::PropertyValues           material_aliases      {         };
    
    det_defs                           detelement_placements {         };
    det_defs                           detelement_volumes    {         };
    std::map<std::string,std::string>  sensitive_volumes     {         };


    
    /// Get the DetElement parent from the path
    DetElement get_detelement_parent(const std::string& )  const;
    /// Add DetElement to structure tree
    DetElement add_detelement(const det_def& def, PlacedVolume pv)  const;
    /// Set volume sensitive
    void set_sensitive( const std::string& sens_det, PlacedVolume pv )  const;
    /// Enhance PlaceVolume
    void enhance(const std::string& path, PlacedVolume node)  const;


    /// Access TGeo material to build volumes
    TGeoMaterial* get_material( const std::string& name )  const;
    /// Clone existing material with new name and density
    TGeoMaterial* clone_material( TGeoMaterial* mat, const std::string& name, double density );
    /// Access material name with possibility of approximation
    std::pair<bool, std::string> get_material_z( double Z )  const;
    bool add_material_z( int index, TGeoMaterial* mat );
    
    int resolve_material(       int32_t            index,
                                const std::string& name,
                                double             Z,
                                double             A,
                                double             density);
    int approximate_material(   int32_t            index,
                                const std::string& name,
                                double             Z,
                                double             A,
                                double             density);
    int make_material(          int32_t            index,
                                const std::string& name,
                                double             Z,
                                double             A,
                                double             density,
                                double             intlen,
                                double             radlen);
    int approximate_mixture(    int32_t            index,
                                const std::string& name,
                                double             Z,
                                double             A,
                                double             density,
                                const std::vector<mixture_entry>& mixture);
    int make_mixture(           int32_t            index,
                                const std::string& name,
                                double             Z,
                                double             A,
                                double             density,
                                const std::vector<mixture_entry>& mixture);


    
  public:
    /// Determine debug mode for a given subdetectors
    bool have_debug( char val )  const  {
      return debug_detector.find(::toupper(val)) != std::string::npos;
    }

    /// Create copy of an existing solid with new parameters
    Solid create_solid(        const TClass* type, const params_t& params);
    
    /// Check if material is supposed to be aliased
    bool have_material_alias(  const std::string& name)  const;

    /// Access material to build volumes
    Material get_medium(       const std::string& name)  const;
                     
    /// Add rotation matrix to build placement transformations
    void add_rotation(         int32_t index, const Rotation3D& rotation );

    /// Add material to geometry inventory
    int add_material(          int32_t            index,
                               const std::string& name,
                               double             Z,
                               double             A,
                               double             density,
                               double             intlen,
                               double             radlen);

    int add_mixture(           int32_t            index,
                               const std::string& name,
                               double             Z,
                               double             A,
                               double             density,
                               double             intlen,
                               double             radlen,
                               const std::vector<mixture_entry>& mix);

    /// Add volume medium to geometry inventory
    int add_medium(            int32_t            index,
                               const std::string& name,
                               const std::string& material);

    /// Add volume to geometry inventory
    void add_volume(           int32_t            index,
                               const std::string& name,
                               const std::string& material,
                               Solid              solid,
                               const std::string& option);

    /// Add volume medium to geometry inventory
    void add_placement(        int32_t            index,
                               const std::string& mother,
                               const std::string& volume,
                               int32_t            copy_nr,
                               const Position&    position,
                               int32_t            index_rotation,
                               const params_t&    params,
                               const std::string& option);
    
    /// Add volume division to geometry inventory
    void add_division(         const std::string& name,
                               const std::string& volume,
                               int32_t            axis,
                               int32_t            ndiv,
                               double             start,
                               double             step,
                               int32_t            idx_medium,
                               const char*        opt);

  public:
    /// Initializing constructor
    geant3_geometry(Detector& desc);
    
    /// Default destructor
    virtual ~geant3_geometry();

    void construct(const std::string& top_volume);
    
    /// Handle the conversion of the Geant3 materials:
    virtual void handle_materials()  = 0;

    /// Handle the conversion of the Geant3 media
    virtual void handle_media()      = 0;

    /// Handle the conversion of the Geant3 volumes
    virtual void handle_volumes()    = 0;

    /// Handle the conversion of the Geant3 volume divisions
    virtual void handle_divisions()  = 0;

    /// Handle the conversion of the Geant3 transformation matrices
    virtual void handle_rotations()  = 0;
    
    /// Handle the conversion of the Geant3 placements
    virtual void handle_placements() = 0;
  };   // End class geant3_geometry
  
  /// Concrete implementation class to import Geant3 geometries
  /**
   *   The missing entry points are generated by the
   *   analysis of the sniffer output to the Geant3
   *   simulation program 'extract_geometry.py'
   *
   *
   *   \author  M.Frank
   *   \version 1.0
   */
  class geant3_geometry_imp : public geant3_geometry  {
  public:
    /// Initializing constructor
    geant3_geometry_imp(Detector& desc);
    /// Default destructor
    virtual ~geant3_geometry_imp();
    /// Handle the conversion of the Geant3 materials:
    virtual void handle_materials()   override;
    /// Handle the conversion of the Geant3 media
    virtual void handle_media()       override;
    /// Handle the conversion of the Geant3 transformation matrices
    virtual void handle_rotations()   override;
    /// Handle the conversion of the Geant3 volumes
    virtual void handle_volumes()     override;
    /// Handle the conversion of the Geant3 volume divisions
    virtual void handle_divisions()   override;
    /// Handle the conversion of the Geant3 placements
    virtual void handle_placements()  override;
  };   // End class geant3_geometry_imp
}      // End namespace dd4hep
#endif // ALPHA_GEANT3_GEOMETRY_H
