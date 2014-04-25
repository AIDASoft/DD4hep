// $Id: XMLStack.h 889 2013-11-14 15:55:39Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_CONDITIONS_CONDITIONSSTACK_H
#define DD4HEP_CONDITIONS_CONDITIONSSTACK_H

// Framework include files
#include "DD4hep/Detector.h"

// C/C++ include files
#include <memory>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    namespace ConditionsInterna {
      class Entry;
    }

    /** @class ConditionsStack  ConditionsStack.h DDCond/ConditionsStack.h
     * 
     *  The data class behind a conditions handle.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class ConditionsStack {
    public:
      friend class std::auto_ptr<ConditionsStack>;

      typedef ConditionsInterna::Entry Entry;
      typedef std::map<std::string,Entry*> Stack;

    protected:
      /// The subdetector specific map of conditions caches
      Stack m_stack;

      /// Standard constructor
      ConditionsStack();
      /// Standard destructor
      virtual ~ConditionsStack();

    public:
      /// Static client accessor
      static ConditionsStack& get(); 
      /// Create an conditions stack instance. The creation of a second instance will be refused.
      static void create();
      /// Check existence of conditions stack
      static bool exists();
      /// Push new entry into the stack
      void insert(std::auto_ptr<Entry>& data);
      /// Clear data content and remove the slignment stack
      void release();
      /// Access size of the conditions stack
      size_t size() const  {  return m_stack.size(); }
       /// Retrieve an conditions entry of the current stack
      std::auto_ptr<Entry> pop();
      /// Get all path entries to be aligned. Note: transient!
      std::vector<const Entry*> entries() const;
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_CONDITIONSSTACK_H  */
