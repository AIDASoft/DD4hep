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

/// Framework include files
#include <dd4hep/geant3_geometry.h>

/// DD4hep include files
#include <DD4hep/Printout.h>

/// ROOT include files
#include <TClass.h>

/// C/C++ include files
#include <iomanip>
#include <memory>

using namespace dd4hep;

namespace  {

  static UInt_t unique_mat_id = 0xBFFEFEED;

}

#define Z_FACTOR 100
#define Z_REL_ACCURRACY  0.05

void geant3_print_mat(TGeoManager& mgr)  {
  TIter next(mgr.GetListOfMaterials());
  TGeoMaterial *material;
  while ((material = (TGeoMaterial*)next()))  {
    material->Print();
  }
}

/// Default constructor
geant3_geometry::geant3_geometry(Detector& desc)
  : description(desc)
{
  TGeoManager&      mgr = description.manager();
  TGeoElementTable* tab = mgr.GetElementTable();
  for( int i=0; i<tab->GetNelements(); ++i )  {
    int iz = tab->GetElement(i)->Z() * Z_FACTOR;
    this->materials_z[iz] = tab->GetElement(i)->GetName();
  }
  TIter next(mgr.GetListOfMaterials());
  TGeoMaterial *m;
  while ((m = (TGeoMaterial*)next()))  {
    int iz = ::round( m->GetZ() * Z_FACTOR );
    this->materials_z[iz] = m->GetName();
  }
  // tab->Print();
}

/// Default destructor
geant3_geometry::~geant3_geometry()
{
}

/// Access material to build volumes
Material geant3_geometry::get_medium(const std::string& name)  const  {
  auto iv = this->material_aliases.find(name);
  if( iv != this->material_aliases.end() )  {
    auto mat = this->description.material(iv->second);
    if( mat.isValid() )  {
      return mat;
    }
    printout(ALWAYS,"geant3_geometry",
             "+++ Undefined dd4hep medium: %s", name.c_str());
  }
  return this->description.material(name);
}

/// Access TGeo material to build volumes
TGeoMaterial* geant3_geometry::get_material( const std::string& name )  const  {
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial( name.c_str() );
  if( !mat )   {
    auto iv = this->material_aliases.find(name);
    if( iv != this->material_aliases.end() )  {
      mat = mgr.GetMaterial(iv->second.c_str() );
    }
  }
  return mat;
}

/// Access material name with possibility of approximation
std::pair<bool, std::string>
geant3_geometry::get_material_z( double Z )  const  {
  TGeoManager& mgr = description.manager();
  TIter next(mgr.GetListOfMaterials());
  TGeoMaterial *material;
  double      zdelta, delta = 1000.0;
  std::string acc;
  while ((material = (TGeoMaterial*)next()))  {
    zdelta = std::abs(Z - material->GetZ());
    if( zdelta/Z < delta ) {
      delta = zdelta/Z;
      acc = material->GetName();
    }
    if ( zdelta/Z < 1e-5 )  {
      return { false, { material->GetName() } };
    }
  }
  if( this->resolve_materials )  {
    if( delta < Z_REL_ACCURRACY )  {
      return { true, acc };
    }
  }
  
  int  iz = int(::round(Z * Z_FACTOR));
  auto im = this->materials_z.find(iz);
  auto ie = this->materials_z.end();

  if( im != ie )  {
    return { false , im->second };
  }

  if( this->resolve_materials )  {
    for( const auto& mat : this->materials_z )  {
      if( mat.first > 0 )  {
        zdelta = std::abs(double(mat.first-iz))/double(mat.first);
        if( zdelta < Z_REL_ACCURRACY )  {
          if( zdelta < delta ) {
            delta = zdelta;
            acc = mat.second;
          }
        }
      }
    }
    if( !acc.empty() )  {
      return { true, acc };
    }
  }
  return { false, { } };
}

bool geant3_geometry::add_material_z( int index, TGeoMaterial* mat )  {
  if( index > 0 )  {
    if( this->materials.find(index) == this->materials.end() )  {
      this->materials[index] = mat;
    }
  }
  int iz = int(::round(mat->GetZ() * Z_FACTOR));
  if( this->materials_z.find(iz) != this->materials_z.end() )  {
    this->materials_z[iz] = mat->GetName();
    return true;
  }
  return false;
}

/// Check if material is supposed to be aliased
bool geant3_geometry::have_material_alias(const std::string& name)  const  {
  auto iv = this->material_aliases.find(name);
  return (iv != this->material_aliases.end());
}

/// Add rotation matrix to build placement transformations
void geant3_geometry::add_rotation( int32_t index, const Rotation3D& rot )  {
  if( this->rotations.find(index) != this->rotations.end() )  {
    except("geant3_geometry", "Rotation with index %d is already registered.", index);
  }
  this->rotations[index] = rot;
}

/// Clone existing material with new name and density
TGeoMaterial*
geant3_geometry::clone_material(TGeoMaterial* mat, const std::string& name, double density)  {
  if( mat->IsMixture() )  {
    auto* omix = (TGeoMixture*)mat;
    auto* mix  = new TGeoMixture( name.c_str(), omix->GetNelements(), density );
    const double* weight = omix->GetWmixt();

    for( int i=0; i<omix->GetNelements(); ++i)
      mix->AddElement(omix->GetElement(i), weight[i]);
    mix->ComputeDerivedQuantities();
    return mix;
  }
  auto* mix = new TGeoMixture( name.c_str(), 1, density );
  mix->AddElement( mat->GetElement(), 1.0 );
  mix->ComputeDerivedQuantities();
  return mix;
}

/// Add material to geometry inventory
int geant3_geometry::add_material( int32_t            index,
                                   const std::string& name,
                                   double             Z,
                                   double             A,
                                   double             density,
                                   double             intlen,
                                   double             radlen)
{
  if( this->resolve_materials == 0 )
    return this->resolve_material( index, name, Z, A, density );
  else if( this->resolve_materials == 1 )
    return this->approximate_material( index, name, Z, A, density );
  return this->make_material( index, name, Z, A, density, intlen, radlen );
}

/// Add material to geometry inventory
int geant3_geometry::add_mixture(  int32_t            index,
                                   const std::string& name,
                                   double             Z,
                                   double             A,
                                   double             density,
                                   double         /*  intlen  */,
                                   double         /*  radlen  */,
                                   const std::vector<mixture_entry>& mixture)
{
  if( this->resolve_materials == 0 )
    return this->resolve_material( index, name, Z, A, density );
  else if( this->resolve_materials == 1 )
    return this->approximate_mixture( index, name, Z, A, density, mixture );
  return this->make_mixture( index, name, Z, A, density, mixture );
}

int geant3_geometry::resolve_material( int32_t            index,
                                       const std::string& name,
                                       double             Z,
                                       double             A,
                                       double             density)
{
  static int recurses = 0;
  struct _recurse_count {
  public:
    _recurse_count()  { ++recurses; }
    ~_recurse_count() { ++recurses; }
  };
  _recurse_count count;
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial( name.c_str() );

  if( nullptr != mat )  {
    printout(  this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
              "add_material: %-14s %-24s %-24s index:%3d Z:%8.3f/%8.3f A:%5.1f dens:%7.5f g/cm3",
              "EXISTING", name.c_str(), mat->GetName(), index, Z, mat->GetZ(), A, density);
    this->materials[index] = mat;
    return 1;
  }
  if( recurses > 10 )  {
    return 0;
  }
  auto iv = this->material_aliases.find( name );
  if( iv != this->material_aliases.end() )  {
    if( this->resolve_material( index, iv->second, Z, A, density ) )  {
      mat = mgr.GetMaterial( name.c_str() );
    }
  }
  if( nullptr != mat )  {
    auto* mix = this->clone_material( mat, name, density );
    if( mix )  {
      printout(  this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
                 "add_material: %-14s %-24s %-24s index:%3d Z:%8.3f/%8.3f A:%5.1f dens:%7.5f g/cm3",
                 "EXISTING.ALIAS", name.c_str(), mix->GetName(), index, Z, mix->GetZ(), A, density);
      this->materials[index] = mix;
      return 1;
    }
  }
  return 0;
}

int geant3_geometry::approximate_material( int32_t            index,
                                           const std::string& name,
                                           double             Z,
                                           double             A,
                                           double             density)
{
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial(name.c_str());
  if( nullptr == mat )  {
    std::string alias, type  = "ALIAS";
    auto iv = this->material_aliases.find(name);
    if( iv == this->material_aliases.end() )  {
      auto [approx, z_alias] = this->get_material_z( Z );
      type = approx ? "APPROXIMATE" : "ALIAS(Z)";
      alias = z_alias;
    }
    else  {
      alias = iv->second;
    }
    if( !alias.empty() )  {
      mat = mgr.GetMaterial(alias.c_str());
      if( mat )  {
        printout(  this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
                  "add_material: %-11s    %-24s %-24s index:%3d Z:%8.3f/%8.3f A:%5.1f dens:%7.5f g/cm3",
                  type.c_str(), name.c_str(), mat->GetName(), index, Z, mat->GetZ(), A, density);
        if( name != mat->GetName() )  {
          TGeoMaterial* nmat = this->clone_material( mat, name, density );
          if( nmat != nullptr )  {
            this->materials[index] = nmat;
            return 1;
          }
        }
        else  {
          this->materials[index] = mat;
          return 1;
        }
      }
    }
    printout( ERROR, "geant3_geometry",
              "add_material: %-11s    %-24s FAILED material resolution index:%3d Z:%8.3f A:%5.1f dens:%7.5f g/cm3",
              type.c_str(), name.c_str(), index, Z, A, density);
  }
  return 0;
}

int geant3_geometry::make_material( int32_t            index,
                                    const std::string& name,
                                    double             Z,
                                    double             A,
                                    double             density,
                                    double             intlen,
                                    double             radlen)
{
  TGeoManager&      mgr = description.manager();
  TGeoMaterial*     mat = mgr.GetMaterial(name.c_str());

  if( nullptr != mat )  {
    return 1;
  }

  TGeoElementTable* tab = mgr.GetElementTable();
  TGeoElement*      elt = tab->FindElement(name.c_str());
  mat = new TGeoMaterial(name.c_str(), A, Z, density, radlen, intlen);
  if( nullptr == elt )  {
    printout(ERROR, "geant3_geometry",
             "add_material: Warning: Element with name %s and index %d Z=%7.3f A=%7.3f dens:%7.5f g/cm3 is UNKNOWN.",
             name.c_str(), index, Z, A, density);
  }
  printout(ERROR, "geant3_geometry",
           "add_material: Create material (pure) with name %-24s and index %3d: Z=%5.1f A=%5.1f dens:%7.5f g/cm3",
           name.c_str(), index, Z, A, density);
  this->add_material_z( index, mat );
  return 1;
}

int geant3_geometry::make_mixture(         int32_t            index,
                                           const std::string& name,
                                           double             Z,
                                           double             A,
                                           double             density,
                                           const std::vector<mixture_entry>& mixture)
{
  return this->approximate_mixture( index, name, Z, A, density, mixture );
}

int geant3_geometry::approximate_mixture(  int32_t            index,
                                           const std::string& name,
                                           double             Z,
                                           double             A,
                                           double             density,
                                           const std::vector<mixture_entry>& mixture)
{
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial(name.c_str());

  if( nullptr != mat )  {
    return 1;
  }
  bool resolved = true;
  for( std::size_t i=0; i < mixture.size(); ++i )  {
    const auto& mix = mixture[i];
    if( mix.fraction < 1e-5 ) continue;
    auto [approx, mix_name] = this->get_material_z( mix.Z );
    mat = mix_name.empty() ? nullptr : this->get_material( mix_name );
    if( mat )  {
      this->add_material_z( -1, mat );
      continue;
    }
    resolved = false;
  }
  if( !resolved )  {
    if( this->resolve_materials > 1 )  {
      printout( this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
                "add_material: FAILED material (mixture) with name %s and index %3d: Z:%5.1f A:%5.1f dens:%7.5f g/cm3 mix:%ld",
                name.c_str(), index, Z, A, density, mixture.size());
      for( std::size_t i=0; i < mixture.size(); ++i )  {
        const auto& mix = mixture[i];
        if( mix.fraction < 1e-5 ) continue;
        auto [approx,mix_name] = this->get_material_z( mix.Z );
        mat = mix_name.empty() ? nullptr : this->get_material( mix_name );
        printout( this->resolve_materials > 1 ? ERROR : DEBUG, "geant3_geometry",
                  "add_material:   COMPONENT %-24s. Z:%8.3f A:%5.1f fraction:%8.4f",
                  mat ? mat->GetName() : "FAILED Z-lookup", mix.Z, mix.A, mix.fraction);
      }
    }
    return 0;
  }
  if( resolved )  {    
    TGeoElementTable* tab = mgr.GetElementTable();
    std::map<std::string, double> weights;

    printout( this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
              "add_material: Create material (mixture) with name %s and index %3d: Z:%5.1f A:%5.1f dens:%7.5f g/cm3 mix:%ld",
              name.c_str(), index, Z, A, density, mixture.size());
    for( std::size_t i=0, n=mixture.size(); i < n; ++i )  {
      const auto& mix = mixture[i];
      if( mix.fraction < 1e-7 ) continue;
      auto [approx, mix_name] = this->get_material_z( mix.Z );
      if( mix_name.empty() )   {
        printout( ERROR, "geant3_geometry",
                  "add_material:     Mixture: %s FAILED component Z resolution. index:%3d Z:%8.3f A:%5.1f fraction:%8.4f",
                  name.c_str(), index, mix.Z, mix.A, mix.fraction);
      }
      
      mat = this->get_material( mix_name );
      printout( this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
                "add_material:   MIXTURE (mat)      %2d %-24s Z:%5.1f A:%5.1f fraction:%8.4f Nel:%2d ",
                i, mat->GetName(), mat->GetZ(), mat->GetA(), mat->GetNelements(), mix.fraction );
      if( mat && mat->IsMixture() )  {
        double val;
        auto* geo_mixture = (TGeoMixture*)mat;
        const double* weight = geo_mixture->GetWmixt();
        for( int j=0; j < geo_mixture->GetNelements(); ++j )  {
          auto* elt = geo_mixture->GetElement(j);
          if( weights.find(elt->GetName()) != weights.end() )
            val = weights[elt->GetName()] + (weight[j] * mix.fraction);
          else
            val = weight[j] * mix.fraction;
          weights[elt->GetName()] = val;
        }
      }
      else  {
        weights[mat->GetElement()->GetName()] = 1.0;
      }
    }
    auto* mix = new TGeoMixture( name.c_str(), weights.size(), density );
    int count = 0;
    for( const auto& w : weights )  {
      TGeoElement* elt = tab->FindElement(w.first.c_str());
      printout( this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
                "add_material:   MIXTURE (elt)      %2d %-24s Z:%3d   A:%5.1f fraction:%8.4f ",
                count++, elt->GetName(), elt->Z(), elt->A(), w.second );
      mix->AddElement(elt, w.second );
    }
    mix->ComputeDerivedQuantities();
    this->add_material_z( Z, mix );
    return 1;
  }
  return 0;
}

/// Add volume medium to geometry inventory
int geant3_geometry::add_medium(  int32_t            index,
                                  const std::string& name,
                                  const std::string& material)  {
  TGeoManager& mgr    = description.manager();
  TGeoMedium*  medium = mgr.GetMedium(name.c_str());

  if( nullptr == medium )  {
    TGeoMaterial* mat = mgr.GetMaterial( material.c_str() );
    if( !mat )  {
      auto med = this->get_medium( material );
      if( med.isValid() ) mat = med->GetMaterial();
    }
    --unique_mat_id;
    medium = new TGeoMedium( name.c_str(), unique_mat_id, mat );
    medium->SetTitle( "material" );
    medium->SetUniqueID( unique_mat_id );
  }
  printout( this->debug_materials ? ALWAYS : DEBUG, "geant3_geometry",
            "add_material: Created medium with name: %s index %d material: %s.",
            name.c_str(), index, material.c_str() );
  this->media[index] = medium;
  return 1;
}

/// Add volume to geometry inventory
void geant3_geometry::add_volume(    int32_t            index,
                                     const std::string& name,
                                     const std::string& medium,
                                     Solid              solid,
                                     const std::string& option)
{
  if( this->divisions.find(name) != this->divisions.end() )  {
    return;
  }
  if( this->volumes.find(name) != this->volumes.end() )  {
    except("geant3_geometry", "+++ Volume %s with index %d is already registered.",
           name.c_str(), index);
  }
  VisAttr  vis = this->description.visAttributes("Vis_"+name);
  Material mat = this->get_medium(medium);
  Volume   vol(name, solid, mat);

  if( !option.empty() )  {
  }
  
  if( have_debug(name[0]) || this->debug_volumes )  {
    std::string vis_name = vis.isValid() ? vis.name() : "????";
    std::string med_name = (medium != mat.name()) ? "[alias: "+std::string(mat.name())+"]" : std::string("");
    std::string mat_name = mat->GetMaterial() ? mat->GetMaterial()->GetName() : "??";
    std::string sol_isa  = solid->IsA()->GetName();
    printout(ALWAYS, "geant3_geometry",
             "+++ Volume: %-10s vis: %-12s medium: %-16s material: %-16s %s [%s]",
             name.c_str(), vis_name.c_str(), medium.c_str(), mat_name.c_str(),
             med_name.c_str(), sol_isa.c_str() );
  }
  if( vis.isValid() )  {
    vol.setVisAttributes(vis);
  }
  this->volumes[name] = vol;
}

/// Add volume division to geometry inventory
void geant3_geometry::add_division( const std::string& name,
                                    const std::string& volume,
                                    int32_t            axis,
                                    int32_t            ndiv,
                                    double             start,
                                    double             step,
                                    int32_t            idx_medium,
                                    const char*        opt)
{
  bool dbg = have_debug( name[0] ) || have_debug( volume[0] ) || this->debug_divisions;
  if( this->processing_step < PROCESSING_DIVISIONS )  {
    this->divisions[name] = nullptr;
    return;
  }
  auto iv = this->volumes.find(name);
  if( iv != this->volumes.end() )  {
    except( "geant3_geometry","add_division: daughter volume "+name+" ALREADY present." );
  }
  iv = this->volumes.find(volume);
  if( iv == this->volumes.end() )  {
    except( "geant3_geometry","add_division: volume "+volume+" NOT present." );
  }
  Volume vol(iv->second);
  if( idx_medium != 0 )  {
    auto ip = this->media.find( idx_medium );
    if( ip != this->media.end() )  {
      idx_medium = ip->second->GetId();
    }
  }
  VisAttr vis = this->description.visAttributes( "Vis_"+name );
  Volume  div = vol.divide( name, axis, ndiv,
                            start/units::degree, step/units::degree,
                            idx_medium, opt );
  if( vis.isValid() )  {
    div.setVisAttributes( vis );
  }
  printout( dbg ? ALWAYS : DEBUG, "geant3_geometry",
            "+++ add_division: %s --> %d  axis: %d ndiv: %d start:%f step:%f medium:%d opt:%s",
            volume.c_str(), name.c_str(), axis, ndiv, start, step, idx_medium, opt);
  this->volumes[name] = div;
}

/// Add volume medium to geometry inventory
void geant3_geometry::add_placement( int                index,
                                     const std::string& mother,
                                     const std::string& volume,
                                     int                copy_nr,
                                     const Position&    position,
                                     int                index_rotation,
                                     const params_t&    params,
                                     const std::string& option)
{
  Rotation3D  rot;
  std::string volnam = volume;
  auto ip = this->placements.find(index);
  auto im = this->volumes.find(mother);
  auto iv = this->volumes.find(volume);
  bool dbg = have_debug(volume[0]) || have_debug(mother[0]) || this->debug_placements;

  if( ip != this->placements.end() )  {
    except("geant3_geometry","place_volume: Placed volume %d already registered.", index);
  }
  if( im == this->volumes.end() )  {
    except("geant3_geometry","place_volume: No mother volume "+mother+" present.");
  }
  if( iv == this->volumes.end() )  {
    except("geant3_geometry","place_volume: No daughter volume "+volume+" present.");
  }
  if( index_rotation != 0 )  {
    auto ir = this->rotations.find(index_rotation);
    if( ir == this->rotations.end() )  {
      except("geant3_geometry","place_volume: No rotation for volume "+volume+" im mother "+mother);
    }
    rot = ir->second;
  }
  /// How does one handle boolean solids ?
  if( !option.empty() )  {
    /**
       • UNION: the union of two volumes B and C is obtained positioning two overlapping ‘MANY’ daughters
       B and C inside the same mother A. To identify the result of the union as a single volume is enough to
       create a SET associated to the volumes B and C.
       • SUBTRACTION: the subtraction of a volume B from a volume C is obtained positioning B as ‘ONLY’
       and C as a ‘MANY’ overlapping B. The result of the subtraction is automatically what the tracking
       sees as the C volume, so GSPOS is the only user interface needed. The volume B can have the same
       material as the mother A or not. The normal positioning technique used for ‘ONLYs’ is a particular
       case of boolean subtraction (a volume is contained inside the other).
       • INTERSECTION: the intersection of two volumes B and C is obtained positioning a protuding ‘MANY’
       object C in a ‘MANY’ object B which is normally placed in a ‘ONLY’ mother A. A and B must have the
       same material, while C has the material asked for the intersection. The intersection is given by the
       part of C which is not protuding from B, therefore by what the new tracking sees as the volume C:
       again the only needed user interface is GSPOS. If not interacting with other daughters of A, B could
       also be ‘ONLY’. Without any ambiguity, even further ‘MANY’ daughters of A can overlap the protuding
       part of C outside B, because it is really invisible to the new tracking.
    */
  }
  Volume   vol = iv->second;
  Material mat = vol.material();
  VisAttr  vis = this->description.visAttributes("Vis_"+volume);
  if( !params.empty() )  {
    Solid  new_solid = this->create_solid( vol.solid()->IsA(), params);
    if( !new_solid.isValid() )  {
      printout(WARNING, "geant3_geometry",
               "+++ add_placement: Unhandled shape placement with parameters: %s of type %s",
               volume.c_str(), new_solid->IsA()->GetName() );
    }
    vol = Volume(volnam, new_solid, mat);
    if( vis.isValid() )  {
      vol.setVisAttributes(vis);
    }
    volnam = volume+" ("+std::to_string(copy_nr)+")";
  }
#if defined(USE_TGEOHMATRIX)
  TGeoHMatrix *trafo = new TGeoHMatrix();
  double rot_data[9];
  rot.GetComponents(rot_data, rot_data+9);
  trafo->SetDx(position.x());
  trafo->SetDy(position.y());
  trafo->SetDz(position.z());
  trafo->SetRotation(rot_data);
#else
  Transform3D trafo( rot, position );
#endif
  PlacedVolume pv = im->second.placeVolume( vol, copy_nr, trafo );
  this->placements[index] = pv;
  printout(dbg ? ALWAYS : DEBUG, "geant3_geometry",
           "+++ add_placement: %-12s volume: %-10s mother: %-12s material:%-24s %s%s [%s]",
           pv.name(), volnam.c_str(), mother.c_str(), mat.name(),
           vis.isValid() ? "vis: " : "", vis.isValid() ? vis.name() : "",
           vol.solid()->IsA()->GetName() );
}

/// Create copy of an existing solid with new parameters
Solid geant3_geometry::create_solid( const TClass* typ, const params_t& p)  {
  Solid  solid;
  double cm = units::cm;
  double dg = units::degree;

  if( typ == TGeoTubeSeg::Class() && p.size() == 3 )  {
    solid = Tube( p[0]*cm, p[1]*cm, p[2]*cm );
  }
  else if( typ == TGeoTubeSeg::Class() && p.size() == 5 )  {
    solid = Tube( p[0]*cm, p[1]*cm, p[2]*cm, p[3]*dg, p[4]*dg );
  }
  else if( typ == TGeoCone::Class() )  {
    solid = Cone( p[0]*cm, p[1]*cm, p[2]*cm, p[3]*cm, p[4]*cm );
  }
  else if( typ == TGeoConeSeg::Class() )  {
    solid = ConeSegment( p[0]*cm, p[1]*cm, p[2]*cm, p[3]*cm, p[4]*cm, p[5]*dg, p[6]*dg );
  }
  else if( typ == TGeoHype::Class() )  {
    solid = Hyperboloid( p[0]*cm, p[3]*dg, p[1]*cm, p[3]*dg, p[2]*cm );
  }
  else if( typ == TGeoSphere::Class() )  {
    solid = Sphere( p[0]*cm, p[1]*cm, p[2]*dg, p[3]*dg, p[4]*dg, p[5]*dg );
  }                
  else if( typ == TGeoTrd1::Class() )  {
    solid = Trd1( p[0]*cm, p[1]*cm, p[2]*cm, p[3]*cm );
  }
  else if( typ == TGeoTrd2::Class() )  {
    solid = Trd2( p[0]*cm, p[1]*cm, p[2]*cm, p[3]*cm, p[4]*cm );
  }
  else if( typ == TGeoTrap::Class() )  {
    solid = Trap( p[0]*cm, p[1]*dg, p[2]*dg, p[3]*dg, p[4]*cm, p[5]*cm,
                  p[6]*dg, p[7]*dg, p[8]*cm, p[9]*cm, p[10]*dg );
  }
  else if( typ == TGeoPara::Class() )  {
    solid = Solid(new TGeoPara( "para", p[0]*cm, p[1]*cm, p[2]*cm, p[3], p[4], p[5] ));
  }
  else if( typ == TGeoPcon::Class() )  {
    std::vector<double> dz, rmin, rmax;
    int nz = int(p[2]);
    for( int i=0; i<nz; ++i )  {
      dz.push_back(   p[3+i*3]*cm );
      rmin.push_back( p[4+i*3]*cm );
      rmax.push_back( p[5+i*3]*cm );
    }
    solid = Polycone(p[0]*dg, p[1]*dg, rmin, rmax, dz);
  }
  else if( typ == TGeoPgon::Class() )  {
    std::vector<double> dz, rmin, rmax;
    int nz = int(p[2]);
    for( int i=0; i<nz; ++i )  {
      dz.push_back(   p[3+i*3]*cm );
      rmin.push_back( p[4+i*3]*cm );
      rmax.push_back( p[5+i*3]*cm );
    }
    solid = Polyhedra(nz, p[0]*dg, p[1]*dg, dz, rmin, rmax );
  }
  else if( typ == TGeoBBox::Class() )  {
    solid = Box( p[0]*cm, p[1]*cm, p[2]*cm );
  }
  return solid;
}

/// Get the DetElement parent from the path
DetElement geant3_geometry::get_detelement_parent(const std::string& path)  const  {
  if( path.empty() )  {
    return this->description.world();
  }
  return this->description.world();
}

/// Add DetElement to structure tree
DetElement geant3_geometry::add_detelement(const det_def& def, PlacedVolume pv)  const {
  DetElement parent = this->get_detelement_parent(def.parent);
  DetElement detector( parent, def.name, def.id );
  if( !def.volid.empty() )  {
    detector->volumeID = def.id;
  }
  detector.setPlacement( pv );
  if( !def.volid.empty() )  {
    pv.addPhysVolID( def.volid, def.id );
  }
  SensitiveDetector sd;
  Readout ro;
  if( !def.sensitive.empty() )  {
    sd = SensitiveDetector( def.name );
    this->description.addSensitiveDetector( sd );
    sd.setType( def.sensitive );
    if( !def.readout.empty() )  {
      ro = this->description.readout( def.readout );
      if( !ro.isValid() )  {
        printout( ERROR, "geant3_geometry",
                  "+++ Detector: %s Invalid readout object '%s'",
                  def.name.c_str(), def.readout.c_str() );
      }
      sd.setReadout( ro );
    }
  }
  printout( this->debug_structure ? ALWAYS : DEBUG, "geant3_geometry",
            "+++ Assign DetElement %-24s sensitive:%-12s readout:%-12s placement:%s",
            detector.path().c_str(),
            sd.isValid() ? sd.name() : "----",
            ro.isValid() ? ro.name() : "----",
            detector.placementPath().c_str() );
  return detector;
}

/// Set volume sensitive
void geant3_geometry::set_sensitive( const std::string& sens_det, PlacedVolume pv )  const  {
  Volume vol = pv.volume();
  if( !vol.sensitiveDetector().isValid() )  {
    SensitiveDetector sd = this->description.sensitiveDetector(sens_det);
    if( sd.isValid() )  {
      vol.setSensitiveDetector(sd);
      printout( this->debug_sensitive ? ALWAYS : DEBUG, "geant3_geometry",
                "+++ Assign sensitive detector '%s' to volume %s'",
                sd.name(), vol.name() );
    }
  }
}

/// Enhance PlaceVolume
void geant3_geometry::enhance(const std::string& path, PlacedVolume node)  const {
  Volume      vol  = node.volume();
  std::string vnam = vol.name();

  auto iv = this->detelement_volumes.find(vnam);
  if( iv != this->detelement_volumes.end() )  {
    this->add_detelement( iv->second, node );
  }
  auto ip = this->detelement_placements.find(path);      
  if( ip != this->detelement_placements.end() )  {
    this->add_detelement( ip->second, node );
  }
  auto is = this->sensitive_volumes.find(vnam);
  if( is != this->sensitive_volumes.end() )  {
    this->set_sensitive( is->second, node );
  }
}

/// Initializing constructor
geant3_geometry_imp::geant3_geometry_imp(Detector& desc)
  : geant3_geometry(desc)
{
}

/// Default destructor
geant3_geometry_imp::~geant3_geometry_imp()  {
}

//==========================================================================
/// ROOT include files
#include <TROOT.h>
#include <TRint.h>
namespace alpha  {

  void instantiate_dd4hep_geometry(const std::string& compact)  {
    std::pair<int, char**> arg(0, 0);
    dd4hep::Detector& desc = dd4hep::Detector::getInstance();

    desc.fromXML(compact);
    std::cout << "Create ROOT interpreter." << std::endl;
    gApplication = new TRint("DD4hepRint", &arg.first, arg.second);

    const char* dis_argv[2] = {"-level", "20" };
    desc.apply("DD4hep_GeometryDisplay", sizeof(dis_argv)/sizeof(dis_argv[0]), (char**)dis_argv);
    desc.apply("DD4hep_Rint", 0, nullptr);
  }
}
//==========================================================================
