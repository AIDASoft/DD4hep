//==========================================================================
//  Surface installer plugin for generic sliced detector drivers
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : N. Nikiforou, adapted from DD4hep/SiTrackerBarrel_surfaces.cpp
//              by M. Frank. Originally part of the lcgeo package
//==========================================================================

/** \addtogroup SurfacePlugin
 * @{
 * \package DD4hep_GenericSurfaceInstallerPlugin
 * \brief Plugin to install measurement surfaces on a generic sliced detector
 * 
 * Takes up to 13 arguments:
 * - \c dimension: A property of the surface that determines whether the resulting
 *   measurement is 1-D, e.g. strips (\c dimension=1) or 2-D, e.g. pixels (\c dimension=2).
 * - \c u_x, \c u_y, and \c u_z: The x, y, and z components of the u vector, usually associated
 *   with the "good" (i.e. best resolution) measurement direction.
 * - \c v_x, \c v_y, and \c v_z: The x, y, and z components of the v vector, usually associated
 *   with the "bad" (i.e. worst resolution) measurement direction.
 * - \c n_x, \c n_y, and \c n_z: The x, y, and z components of the vector perpendicular to the surface
 *   plane and usually pointing outwards of the detector. 
 * - \c o_x, \c o_y, and \c o_z: The x, y, and z components of the origin vector, used to shift the origin
 *   of the surface from where it is placed within the volume (usually in the center
 *   of a sensitive volume.
 * 
 * All the arguments are conveniently initialized to zero by default, 
 * therefore only the non-zero components need to be provided. For example, to use this
 * plugin for the VertexEndcap detector, put this snippet in the relevant compact file:
 * \verbatim 
<plugins>
      <plugin name="DD4hep_GenericSurfaceInstallerPlugin">
          <argument value="VertexEndcap"/>
          <argument value="dimension=2"/>
          <argument value="u_x=-1."/>
          <argument value="v_z=1."/>
          <argument value="n_y=1."/>
      </plugin>
</plugins>
\endverbatim
 * The plugin assumes boxes are stacked along one of the primary axes, x,y or z 
 * The normal vector (n) must be along one of the axes (i.e. only one non-zero 
 * component). The inner/outer thicknesses are calculated according to n.
 * 
 * Note: Assumes module and component volumes are boxes. For Trapezoids,
 * a fitting box is built around the trapezoid which means dx1=dx2=dx1 and
 * dy1=dy2=dy. This is in principle fine, since we only access the thicknesses
 * (DY in the TrackerEncapSurfacePlugin example) which is supposed to be the same.
 *
 * For more information, please consult with GenericSurfaceInstaller.cpp
 * @}
 */

namespace { 
    struct UserData { 
        int dimension ;
        double uvector[3];
        double vvector[3];
        double nvector[3];
        double ovector[3];
        
    }; 
    
}

// Framework include files
#define SURFACEINSTALLER_DATA UserData
#define DD4HEP_USE_SURFACEINSTALL_HELPER DD4hep_GenericSurfaceInstallerPlugin
#include "DD4hep/SurfaceInstaller.h"

namespace{
    template <> void Installer<UserData>::handle_arguments(int argc, char** argv)   {

        //Initialize defaults to zero
        data.dimension=0;
        data.uvector[0]=0.;
        data.uvector[1]=0.;
        data.uvector[2]=0.;
        data.vvector[0]=0.;
        data.vvector[1]=0.;
        data.vvector[2]=0.;
        data.nvector[0]=0.;
        data.nvector[1]=0.;
        data.nvector[2]=0.;
        data.ovector[0]=0.;
        data.ovector[1]=0.;
        data.ovector[2]=0.;
        
        for(int i=0; i<argc; ++i)  {
            double value = -1;
            char* ptr = ::strchr(argv[i],'=');
            if ( ptr )  {
                std::string name( argv[i] , ptr ) ;
                value = DD4hep::Geometry::_toDouble(++ptr);
                std::cout << "DD4hep_GenericSurfaceInstallerPlugin: argument[" << i << "] = " << name 
                << " = " << value << std::endl;
                if( name=="dimension" ) data.dimension = value ; 
                if( name=="u_x" ) data.uvector[0]=value ; 
                if( name=="u_y" ) data.uvector[1]=value ; 
                if( name=="u_z" ) data.uvector[2]=value ; 
                if( name=="v_x" ) data.vvector[0]=value ; 
                if( name=="v_y" ) data.vvector[1]=value ; 
                if( name=="v_z" ) data.vvector[2]=value ; 
                if( name=="n_x" ) data.nvector[0]=value ; 
                if( name=="n_y" ) data.nvector[1]=value ; 
                if( name=="n_z" ) data.nvector[2]=value ; 
                if( name=="o_x" ) data.ovector[0]=value ; 
                if( name=="o_y" ) data.ovector[1]=value ; 
                if( name=="o_z" ) data.ovector[2]=value ; 
            }
        }
    
        std::cout <<"DD4hep_GenericSurfaceInstallerPlugin: vectors: ";
        std::cout <<"u( "<<data.uvector[0]<<" , "<<data.uvector[1]<<" , "<<data.uvector[2]<<") ";
        std::cout <<"v( "<<data.vvector[0]<<" , "<<data.vvector[1]<<" , "<<data.vvector[2]<<") ";
        std::cout <<"n( "<<data.nvector[0]<<" , "<<data.nvector[1]<<" , "<<data.nvector[2]<<") ";
        std::cout <<"o( "<<data.ovector[0]<<" , "<<data.ovector[1]<<" , "<<data.ovector[2]<<") "<<std::endl;
    
    }  
    
    /// Install measurement surfaces
    template <typename UserData> 
    void Installer<UserData>::install(DetElement component, PlacedVolume pv)   {
        Volume comp_vol = pv.volume();
        if ( comp_vol.isSensitive() )  {  
            Volume mod_vol  = parentVolume(component);
            //FIXME: WHAT IF TRAPEZOID? Should work if trapezoid since it will fit minimal box and dy1=dy2=dy
            DD4hep::Geometry::Box mod_shape(mod_vol.solid()), comp_shape(comp_vol.solid());
            
            if ( !comp_shape.isValid() || !mod_shape.isValid() )   {
                invalidInstaller("DD4hep_GenericSurfaceInstallerPlugin: Components and/or modules are not boxes -- invalid shapes");

            }else if ( !handleUsingCache(component,comp_vol) )  {
                const double* trans = placementTranslation(component);
                double half_module_thickness = 0.;
                double sensitive_z_position  = 0.;

                if (data.nvector[0] !=0 && data.nvector[1] ==0 && data.nvector[2] ==0){
                    half_module_thickness = mod_shape->GetDX();
                    sensitive_z_position = data.nvector[0]>0 ? trans[0] : -trans[0];
                }else if (data.nvector[1] !=0 && data.nvector[0] ==0 && data.nvector[2] ==0){
                    half_module_thickness = mod_shape->GetDY();
                    sensitive_z_position = data.nvector[1]>0 ? trans[1] : -trans[1];

                }else if (data.nvector[2] !=0 && data.nvector[0] ==0 && data.nvector[1] ==0){
                    half_module_thickness = mod_shape->GetDZ();
                    sensitive_z_position = data.nvector[2]>0 ? trans[2] : -trans[2];

                } else {
                    throw std::runtime_error("**** DD4hep_GenericSurfaceInstallerPlugin: normal vector unsupported! It has to be "
                    "perpenidcular to one of the box sides, i.e. only one non-zero component.") ;
                }

                double inner_thickness = half_module_thickness + sensitive_z_position;
                double outer_thickness = half_module_thickness - sensitive_z_position;

                //Surface is placed at the center of the volume, no need to shift origin
                //Make sure u,v,n form a right-handed coordinate system, v along the final z
                Vector3D u(data.uvector[0],data.uvector[1],data.uvector[2]);
                Vector3D v(data.vvector[0],data.vvector[1],data.vvector[2]);
                Vector3D n(data.nvector[0],data.nvector[1],data.nvector[2]);
                Vector3D o(data.ovector[0],data.ovector[1],data.ovector[2]);
                Type type( Type::Sensitive ) ;
                
                if( data.dimension == 1 ) {
                    type.setProperty( Type::Measurement1D , true ) ;
                } else if( data.dimension != 2 ) {
                    throw std::runtime_error("**** DD4hep_GenericSurfaceInstallerPlugin: no or wrong "
                    "'dimension' argument given - has to be 1 or 2") ;
                }
                VolPlane surf(comp_vol, type, inner_thickness, outer_thickness, u, v, n, o);
                addSurface(component,surf);
            }
        }
    }
}// namespace
