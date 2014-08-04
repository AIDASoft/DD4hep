// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDCORE_LCDDLOAD_H
#define DD4HEP_DDCORE_LCDDLOAD_H

// Framework includes

// C/C++ include files
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  namespace XML  { class Handle_t; }
  namespace Geometry { class LCDD; }

  /// Data implementation class of the LCDD interface
  /** @class LCDDLoad   LCDDLoad.h  DD4hep/LCDDLoad.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class LCDDLoad  {
  public:
    friend class Geometry::LCDD;
  protected:
    /// Reference to the LCDD instance
    Geometry::LCDD* m_lcdd;
    /// Default constructor
    LCDDLoad(Geometry::LCDD* lcdd);
    /// Default destructor
    virtual ~LCDDLoad();
  public:
    /// Adopt all data from source structure.
    virtual void processXML(const std::string& fname);
    /// Adopt all data from source structure.
    virtual void processXML(const XML::Handle_t& base, const std::string& fname);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const std::string& msg_source, const XML::Handle_t& root);
  };

} /* End namespace DD4hep   */
#endif    /* DD4HEP_DDCORE_LCDDLOAD_H */
