// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_INTERNAL_NAMEDOBJECT_H
#define DD4HEP_GEOMETRY_INTERNAL_NAMEDOBJECT_H

// C/C++ include files
#include <string>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

    /** @class Container  NamedObject.h DD4hep/NamedObject.h
     * 
     *  The data class behind a conditions container handle.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class NamedObject {
    public:
      /// The object name
      std::string name;
      /// The object type
      std::string type;

      /// Standard constructor
      NamedObject();
      /// Initializing constructor
      NamedObject(const char* nam, const char* typ="");
      /// Initializing constructor
      NamedObject(const std::string& nam);
      /// Initializing constructor
      NamedObject(const std::string& nam, const std::string& typ);
      /// Copy constructor
      NamedObject(const NamedObject& c);
      /// Default destructor
      virtual ~NamedObject();
      /// Assignment operator
      NamedObject& operator=(const NamedObject& c);

      /// Access name
      const char* GetName()  const  {
	return name.c_str();
      }
      /// Set name (used by Handle)
      void SetName(const char* nam)  {
	name = nam;
      }
      /// Set Title (used by Handle)
      void SetTitle(const char* tit)  {
	type = tit;
      }
      /// Get name (used by Handle)
      const char* GetTitle() const  {
	return type.c_str();
      }
    };

} /* End namespace DD4hep                   */
#endif    /*  DD4HEP_GEOMETRY_INTERNAL_NAMEDOBJECT_H   */
