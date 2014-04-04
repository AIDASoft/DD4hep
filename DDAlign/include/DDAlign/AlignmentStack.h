// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTSTACK_H
#define DD4HEP_ALIGNMENT_ALIGNMENTSTACK_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <memory>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Alignment namespace declaration
   */
  namespace Geometry {

    /** @class AlignmentStack AlignmentStack.h AlignmentStack.h
     *
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class AlignmentStack  {
    public:
      enum {
	OVERLAP_DEFINED     = 1<<0,
	MATRIX_DEFINED      = 1<<1,
	CHECKOVL_DEFINED    = 1<<2,
	CHECKOVL_VALUE      = 1<<3,
	RESET_VALUE         = 1<<4,
	RESET_CHILDREN      = 1<<5,
	____LLLAST          = 1<<31
      } Flags;

      struct StackEntry {
	DetElement    detector;
	Transform3D   transform;
	std::string   path;
	double        overlap;
	int           flag;

	/// Fully initializing constructor
        StackEntry(const DetElement& p, const std::string& placement, const Transform3D& t, double ov, int flg);
	/// Constructor with partial initialization
	StackEntry(DetElement element, bool rst=true, bool rst_children=true);
	/// Constructor with partial initialization
	StackEntry(DetElement element, const Transform3D& trafo, bool rst=true, bool rst_children=true);
	/// Constructor with partial initialization
	StackEntry(DetElement element, const Position& translation, bool rst=true, bool rst_children=true);
	/// Constructor with partial initialization
	StackEntry(DetElement element, const RotationZYX& rot, bool rst=true, bool rst_children=true);
	/// Constructor with partial initialization
	StackEntry(DetElement element, const Position& translation, const RotationZYX& rot, bool rst=true, bool rst_children=true);
	/// Copy constructor
	StackEntry(const StackEntry& e);
	/// Default destructor
	~StackEntry() {}
	bool checkFlag(int mask) const {  return (flag&mask) == mask; }
	bool checkOverflow() const     {  return checkFlag(CHECKOVL_DEFINED); }
	bool overflowValue() const     {  return checkFlag(CHECKOVL_VALUE); }
	bool checkOverlap() const      {  return checkFlag(OVERLAP_DEFINED); }
	bool hasMatrix() const         {  return checkFlag(MATRIX_DEFINED); }
	bool needsReset() const        {  return checkFlag(RESET_VALUE); }
	bool resetChildren() const     {  return checkFlag(RESET_CHILDREN); }

	/// Attach transformation object
	StackEntry& setTransformation(const Transform3D& trafo);
	/// Instruct entry to ignore the transformation
	StackEntry& clearTransformation();
	/// Set flag to reset the entry to it's ideal geometrical position
	StackEntry& setReset(bool new_value=true);
	/// Set flag to reset the entry's children to their ideal geometrical position
	StackEntry& setResetChildren(bool new_value=true);
	/// Set flag to check overlaps
	StackEntry& setOverlapCheck(bool new_value=true);
	/// Set the precision for the overlap check (otherwise the default is 0.001 cm)
	StackEntry& setOverlapPrecision(double precision=0.001);
      };
      typedef std::map<std::string, StackEntry*> Stack;

    protected:
      /// The subdetector specific map of alignments caches
      Stack m_stack;

      /// Default constructor
      AlignmentStack();

    public:

      /// Default destructor. Careful with this one: 
      virtual ~AlignmentStack();
      /// Static client accessor
      static AlignmentStack& get(); 
      /// Create an alignment stack instance. The creation of a second instance will be refused.
      static void create();
      /// Check existence of alignment stack
      static bool exists();
      /// Add a new entry to the cache. The key is the placement path
      static bool insert(const std::string& full_path, StackEntry* new_entry);
      /// Add a new entry to the cache. The key is the placement path. The placement path must be set in the entry
      static bool insert(StackEntry* new_entry);
      /// Clear data content and remove the slignment stack
      void release();
      /// Access size of the alignment stack
      size_t size() const  {  return m_stack.size(); }
      /// Add a new entry to the cache. The key is the placement path
      bool add(StackEntry* new_entry);
      /// Retrieve an alignment entry of the current stack
      std::auto_ptr<StackEntry> pop();
      /// Get all path entries to be aligned. Note: transient!
      std::vector<const StackEntry*> entries() const;
    };

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_ALIGNMENT_ALIGNMENTSTACK_H       */
