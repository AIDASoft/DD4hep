// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_LCDDGEOIMP_H
#define DD4hep_LCDDGEOIMP_H

// Framework include files
#include "DD4hep/LCDD.h"

// Forward declarations
class TGeoManager;

// C/C++ include files
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  /*
   *   XML namespace declaration
   */
  namespace Geometry  {

    class LCDDImp : public LCDD  {
    public:
      struct InvalidObjectError : public std::runtime_error {
        InvalidObjectError(const std::string& msg) : std::runtime_error(msg) {}
      };
      
      struct ObjectHandleMap : public HandleMap  {
        ObjectHandleMap() {}
        void append(const Ref_t& e, bool throw_on_doubles=true) { 
          if ( e.isValid() )  {
            std::string n = e.name();
	    std::pair<iterator,bool> r = this->insert(std::make_pair(n,e.ptr()));
	    if ( !throw_on_doubles || r.second )
	      return;
	    throw InvalidObjectError("Attempt to add an already existing object:"+std::string(e.name())+".");
          }
          throw InvalidObjectError("Attempt to add an invalid object.");
        }

        template <typename T> void append(const Ref_t& e, bool throw_on_doubles=true) {
          T* obj = dynamic_cast<T*>(e.ptr());
          if ( obj )  {
            this->append(e,throw_on_doubles);
	    return;
          }
          throw InvalidObjectError("Attempt to add an object, which is of the wrong type.");
        }
      };
		      
      ObjectHandleMap     m_readouts;
      ObjectHandleMap     m_idDict;
      ObjectHandleMap     m_limits;
      ObjectHandleMap     m_regions;
      ObjectHandleMap     m_detectors;
      ObjectHandleMap     m_alignments;

      ObjectHandleMap     m_sensitive;
      ObjectHandleMap     m_display;
      ObjectHandleMap     m_fields;
      
      // GDML fields
      ObjectHandleMap     m_define;
      ObjectHandleMap     m_materials;
      
      DetElement          m_world;
      DetElement          m_trackers;
      Volume              m_worldVol;
      Volume              m_trackingVol;
      
      Material            m_materialAir;
      Material            m_materialVacuum;
      VisAttr             m_invisibleVis;
      OverlayedField      m_field;
      Ref_t               m_header;
      Properties*         m_properties;
      LCDDBuildType       m_buildType;

      /// Default constructor
      LCDDImp();

      /// Standard destructor
      virtual ~LCDDImp();

      /// Access flag to steer the detail of building of the geometry/detector description
      virtual LCDDBuildType buildType() const { return m_buildType; }
		      
      /// Read compact geometry description or alignment file
      virtual void fromCompact(const std::string& fname, LCDDBuildType type=BUILD_DEFAULT) 
      { fromXML(fname,type);              }

      /// Read any XML file
      virtual void fromXML(const std::string& fname,LCDDBuildType type=BUILD_DEFAULT);
      
      virtual void dump() const;

      /// Manipulate geometry using facroy converter
      virtual void apply(const char* factory, int argc, char** argv);

      /// Apply & lock realigments
      //virtual void applyAlignment();
      
      //virtual void create();
      virtual void init();
      virtual void endDocument();
      
      virtual Handle<TObject> getRefChild(const HandleMap& e, const std::string& name, bool throw_if_not=true)  const;
      virtual Volume          pickMotherVolume(const DetElement& sd) const;

      /// Access to properties
      Properties&             properties()  const      { return *m_properties;          }
      /// Return handle to material describing air
      virtual Material        air() const              { return m_materialVacuum;       }
      /// Return handle to material describing vacuum
      virtual Material        vacuum() const           { return m_materialAir;          }
      /// Return handle to "invisible" visualization attributes
      virtual VisAttr  invisible() const               { return m_invisibleVis;         }
      /// Return reference to the top-most (world) detector element
      virtual DetElement      world() const            { return m_world;                }
      /// Return reference to detector element with all tracker devices.
      virtual DetElement      trackers() const         { return m_trackers;             }
      /// Return handle to the world volume containing everything
      virtual Volume          worldVolume() const      { return m_worldVol;             }
      /// Return handle to the world volume containing the volume with the tracking devices
      virtual Volume          trackingVolume() const   { return m_trackingVol;          }
      /// Return handle to the combined electromagentic field description.
      virtual OverlayedField  field() const            { return m_field;                }
      /// Accessor to the header entry
      virtual Header          header()  const          { return m_header;               }
      /// Accessor to the header entry
      virtual void setHeader(Header h)                 { m_header = h;                  }

      /// Typed access to constants: access string values
      virtual std::string       constantAsString(const std::string& name) const;
      /// Typed access to constants: long values
      virtual long              constantAsLong(const std::string& name) const;
      /// Typed access to constants: double values
      virtual double            constantAsDouble(const std::string& name) const;

      /// Retrieve a constant by it's name from the detector description
      virtual Constant     constant(const std::string& name) const 
      {  return getRefChild(m_define,name);                                             }
      /// Retrieve a limitset by it's name from the detector description
      virtual LimitSet          limitSet(const std::string& name)  const
      {  return getRefChild(m_limits,name);                                             }  
      /// Retrieve a visualization attribute by it's name from the detector description
      virtual VisAttr           visAttributes(const std::string& name) const
      {  return getRefChild(m_display,name,false);                                      }  
      /// Retrieve a matrial by it's name from the detector description
      virtual Material          material(const std::string& name)  const 
      {  return getRefChild(m_materials,name);                                          }
      /// Retrieve a region object by it's name from the detector description
      virtual Region            region(const std::string& name)  const
      {  return getRefChild(m_regions,name);                                            }
      /// Retrieve a id descriptor by it's name from the detector description
      virtual IDDescriptor      idSpecification(const std::string& name)  const
      {  return getRefChild(m_idDict,name);                                             }
      /// Retrieve a readout object by it's name from the detector description
      virtual Readout           readout(const std::string& name)  const
      {  return getRefChild(m_readouts,name);                                           }
      /// Retrieve an alignment entry by it's name from the detector description
      virtual AlignmentEntry    alignment(const std::string& path) const 
      {  return getRefChild(alignments(),path);                                         }
      /// Retrieve a subdetector element by it's name from the detector description
      virtual DetElement        detector(const std::string& name)  const
      {  return getRefChild(m_detectors,name);                                          }
      /// Retrieve a sensitive detector by it's name from the detector description
      virtual SensitiveDetector sensitiveDetector(const std::string& name)  const
      {  return getRefChild(m_sensitive,name,false);                                    }
      /// Retrieve a subdetector element by it's name from the detector description
      virtual CartesianField    field(const std::string& name)  const 
      {  return getRefChild(m_fields,name,false);                                       }
      
      /// Accessor to the map of constants
      virtual const HandleMap& constants() const        { return m_define;              }
      /// Accessor to the map of visualisation attributes
      virtual const HandleMap& visAttributes() const    { return m_display;             }
      /// Accessor to the map of limit settings
      virtual const HandleMap& limitsets()  const       { return m_limits;              }
      /// Accessor to the map of region settings
      virtual const HandleMap& regions() const          { return m_regions;             }
      /// Accessor to the map of materials
      virtual const HandleMap& materials()  const       { return m_materials;           }
      /// Accessor to the map of readout structures
      virtual const HandleMap& readouts() const         { return m_readouts;            }
      /// Accessor to the map of sub-detectors
      virtual const HandleMap& detectors()  const       { return m_detectors;           }
      /// Accessor to the map of aligment entries
      virtual const HandleMap& alignments()  const      { return m_alignments;          }
      /// Accessor to the map of field entries, which together form the global field
      virtual const HandleMap& fields()  const          { return m_fields;              }

#define __R  return *this
      /// Add a new constant to the detector description
      virtual LCDD& add(Constant x)                     { return addConstant(x);        }
      /// Add a new material to the detector description
      virtual LCDD& add(Material x)                     { return addMaterial(x);        }
      /// Add a new limit set to the detector description
      virtual LCDD& add(LimitSet x)                     { return addLimitSet(x);        }
      /// Add a new detector region to the detector description
      virtual LCDD& add(Region x)                       { return addRegion(x);          }
      /// Add a new visualisation attribute to the detector description
      virtual LCDD& add(VisAttr x)                      { return addVisAttribute(x);    }
      /// Add a new id descriptor to the detector description
      virtual LCDD& add(IDDescriptor x)                 { return addIDSpecification(x); }
      /// Add alignment entry to the detector description
      virtual LCDD& add(AlignmentEntry x)               { return addAlignment(x);       }
      /// Add a new detector readout to the detector description
      virtual LCDD& add(Readout x)                      { return addReadout(x);         }
      /// Add a new subdetector to the detector description
      virtual LCDD& add(DetElement x)                   { return addDetector(x);        }
      /// Add a field component to the detector description
      virtual LCDD& add(CartesianField x)               { return addField(x);           }
      
      /// Add a new constant by named reference to the detector description
      virtual LCDD& addConstant(const Ref_t& x)         { m_define.append(x,false); __R;}
      /// Add a new material by named reference to the detector description
      virtual LCDD& addMaterial(const Ref_t& x)         { m_materials.append(x);    __R;}
      /// Add a new limit set by named reference to the detector description
      virtual LCDD& addLimitSet(const Ref_t& x)         { m_limits.append(x);       __R;}
      /// Add a new detector region by named reference to the detector description
      virtual LCDD& addRegion(const Ref_t& x)           { m_regions.append(x);      __R;}
      /// Add a new id descriptor by named reference to the detector description
      virtual LCDD& addIDSpecification(const Ref_t& x)  { m_idDict.append(x);       __R;}
      /// Add a new detector readout by named reference to the detector description
      virtual LCDD& addReadout(const Ref_t& x)          { m_readouts.append(x);     __R;}
      /// Add a new visualisation attribute by named reference to the detector description
      virtual LCDD& addVisAttribute(const Ref_t& x)     { m_display.append(x);      __R;}
      /// Add a new sensitive detector by named reference to the detector description
      virtual LCDD& addSensitiveDetector(const Ref_t& x){ m_sensitive.append(x);    __R;}
      /// Add a new subdetector by named reference to the detector description
      virtual LCDD& addDetector(const Ref_t& x);
      /// Add a new alignment entry by named reference to the detector description
      virtual LCDD& addAlignment(const Ref_t& x)        { m_alignments.append(x);   __R;}
      /// Add a field component by named reference to the detector description
      virtual LCDD& addField(const Ref_t& x);
#undef __R
      
    };
  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_LCDDGEOIMP_H    */
