// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOHANDLER_H
#define DD4HEP_GEOHANDLER_H

#include "DD4hep/LCDD.h"
#include <set>
#include <map>
#include <vector>

// Forward declarations
class TGeoMatrix;
class TGeoVolume;
class TGeoMedium;
class TGeoShape;
class TGeoNode;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class GeoHandler  GeoHandler.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class GeoHandler {
    public:
      typedef std::map<std::string, TGeoVolume*>                 VolumeSet;
      typedef std::vector<std::pair<std::string, TGeoMatrix*> >  TransformSet;
      typedef std::map<std::string, TGeoShape*>                  SolidSet;
      typedef std::map<std::string, TGeoMedium*>                 MaterialSet;
      typedef std::map<int, std::set<const TGeoNode*> >          Data;
      typedef LCDD::HandleMap                                    VisRefs;

      struct GeometryInfo   {
	SolidSet           solids;
	VolumeSet          volumes;
	TransformSet       trafos;
	VisRefs            vis;
	MaterialSet        materials;
	std::set<TGeoMedium*>   media;
	std::set<TGeoElement*>  elements;
      };
    protected:
      Data* m_data;

      /// Internal helper to collect geometry information from traversal
      GeoHandler& i_collect(const TGeoNode* node, int level);

    public:
      /// Default constructor
      GeoHandler();
      /// Initializing constructor
      GeoHandler(Data* ptr);
      /// Default destructor
      virtual ~GeoHandler();
      /// Collect geometry information from traversal
      GeoHandler& collect(DetElement top);
      /// Collect geometry information from traversal with aggregated information
      GeoHandler& collect(DetElement top, GeometryInfo& info);
      /// Access to collected node list
      Data* release();
#if  0
      template <typename C, typename F> static void _handle(const O* ptr, const C& c, F pmf)  {
	for(typename C::const_iterator i=c.begin(); i != c.end(); ++i)   {
	  (ptr->*pmf)((*i).first, (*i).second);
	}
      }
#endif
      template <typename O, typename C, typename F> static void handle(const O* o, const C& c, F pmf)    {
	for(typename C::const_iterator i=c.begin(); i != c.end(); ++i)
	  (o->*pmf)((*i).first, (*i).second);
      }
      template <typename O, typename C, typename F> static void handle(O* o, const C& c, F pmf)     {
	for(typename C::const_iterator i=c.begin(); i != c.end(); ++i)
	  (o->*pmf)((*i).first, (*i).second);
      }
    };

    struct GeoScan {
      protected:
      /// Data holder
      GeoHandler::Data* m_data;
      public:
      /// Initializing constructor
      GeoScan(DetElement e);
      /// Default destructor
      virtual ~GeoScan();
      /// Work callback
      virtual GeoScan& operator()();
    };
  }    // End namespace Geometry
}      // End namespace DD4hep

#endif // DD4HEP_GEOHANDLER_H

