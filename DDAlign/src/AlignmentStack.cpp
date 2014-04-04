// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DDAlign/AlignmentStack.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static AlignmentStack* s_alignStack = 0;


/// Fully initializing constructor
AlignmentStack::StackEntry::StackEntry(const DetElement& p, const std::string& placement, const Transform3D& t, double ov, int f)
  : detector(p), transform(t), path(placement), overlap(ov), flag(f)
{
}

/// Constructor with partial initialization
AlignmentStack::StackEntry::StackEntry(DetElement element, bool rst, bool rst_children)
  : detector(element), transform(), overlap(0.001), flag(0)
{
  if ( rst ) flag |= RESET_VALUE;
  if ( rst_children ) flag |= RESET_CHILDREN;
  if ( detector.isValid() ) path = detector.placementPath();
}

/// Constructor with partial initialization
AlignmentStack::StackEntry::StackEntry(DetElement element, const Transform3D& trafo, bool rst, bool rst_children)
  : detector(element), transform(trafo), overlap(0.001), flag(0)
{
  flag |= MATRIX_DEFINED;
  if ( rst ) flag |= RESET_VALUE;
  if ( rst_children ) flag |= RESET_CHILDREN;
  if ( detector.isValid() ) path = detector.placementPath();
}

/// Constructor with partial initialization
AlignmentStack::StackEntry::StackEntry(DetElement element, const Position& translation, bool rst, bool rst_children)
: detector(element), transform(translation), overlap(0.001), flag(0)
{
  flag |= MATRIX_DEFINED;
  if ( rst ) flag |= RESET_VALUE;
  if ( rst_children ) flag |= RESET_CHILDREN;
  if ( detector.isValid() ) path = detector.placementPath();
}

/// Constructor with partial initialization
AlignmentStack::StackEntry::StackEntry(DetElement element, const RotationZYX& rot, bool rst, bool rst_children)
: detector(element), transform(rot), overlap(0.001), flag(0)
{
  flag |= MATRIX_DEFINED;
  if ( rst ) flag |= RESET_VALUE;
  if ( rst_children ) flag |= RESET_CHILDREN;
  if ( detector.isValid() ) path = detector.placementPath();
}

/// Constructor with partial initialization
AlignmentStack::StackEntry::StackEntry(DetElement element, const Position& translation, const RotationZYX& rot, bool rst, bool rst_children) 
  : detector(element), transform(rot,translation), overlap(0.001), flag(0)
{
  flag |= MATRIX_DEFINED;
  if ( rst ) flag |= RESET_VALUE;
  if ( rst_children ) flag |= RESET_CHILDREN;
  if ( detector.isValid() ) path = detector.placementPath();
}

/// Copy constructor
AlignmentStack::StackEntry::StackEntry(const StackEntry& e)
: detector(e.detector), transform(e.transform), path(e.path), overlap(e.overlap), flag(e.flag)
{
}

/// Attach transformation object
AlignmentStack::StackEntry& AlignmentStack::StackEntry::setTransformation(const Transform3D& trafo)   {
  flag |= MATRIX_DEFINED;
  transform = trafo;
  return *this;
}

/// Instruct entry to ignore the transformation
AlignmentStack::StackEntry& AlignmentStack::StackEntry::clearTransformation()   {
  flag &= ~MATRIX_DEFINED;
  transform = Transform3D();
  return *this;
}

/// Set flag to reset the entry to it's ideal geometrical position
AlignmentStack::StackEntry& AlignmentStack::StackEntry::setReset(bool new_value)   {
  new_value ? (flag |= RESET_VALUE) : (flag &= ~RESET_VALUE);
  return *this;
}


/// Set flag to reset the entry's children to their ideal geometrical position
AlignmentStack::StackEntry& AlignmentStack::StackEntry::setResetChildren(bool new_value)   {
  new_value ? (flag |= RESET_CHILDREN) : (flag &= ~RESET_CHILDREN);
  return *this;
}


/// Set flag to check overlaps
AlignmentStack::StackEntry& AlignmentStack::StackEntry::setOverlapCheck(bool new_value)   {
  new_value ? (flag |= CHECKOVL_DEFINED) : (flag &= ~CHECKOVL_DEFINED);
  return *this;
}


/// Set the precision for the overlap check (otherwise the default is 0.001 cm)
AlignmentStack::StackEntry& AlignmentStack::StackEntry::setOverlapPrecision(double precision)   {
  flag |= CHECKOVL_DEFINED;
  flag |= CHECKOVL_VALUE;
  overlap = precision;
  return *this;
}

/// Default constructor
AlignmentStack::AlignmentStack()
{
}

/// Default destructor
AlignmentStack::~AlignmentStack()   {
  destroyObjects(m_stack)();
}

/// Static client accessor
AlignmentStack& AlignmentStack::get()  {
  if ( s_alignStack ) return *s_alignStack;
  throw runtime_error("AlignmentStack> Stack not allocated -- may not be retrieved!");
}

/// Create an alignment stack instance. The creation of a second instance will be refused.
void AlignmentStack::create()   {
  if ( s_alignStack )   {
    throw runtime_error("AlignmentStack> Stack already allocated. Multiple copies are not allowed!");
  }
  s_alignStack = new AlignmentStack();
}

/// Check existence of alignment stack
bool AlignmentStack::exists()   {
  return s_alignStack != 0;
}

/// Clear data content and remove the slignment stack
void AlignmentStack::release()    {
  if ( s_alignStack )  {
    delete s_alignStack;
    s_alignStack = 0;
    return;
  }
  throw runtime_error("AlignmentStack> Attempt to delete non existing stack.");
}

/// Add a new entry to the cache. The key is the placement path
bool AlignmentStack::insert(const std::string& full_path, StackEntry* entry)  {
  if ( entry && !full_path.empty() )  {
    entry->path = full_path;
    return get().add(entry);
  }
  throw runtime_error("AlignmentStack> Attempt to apply an invalid alignment entry.");
}

/// Add a new entry to the cache. The key is the placement path
bool AlignmentStack::insert(StackEntry* entry)  {
  return get().add(entry);
}

/// Add a new entry to the cache. The key is the placement path
bool AlignmentStack::add(StackEntry* entry)  {
  if ( entry && !entry->path.empty() )  {
    Stack::const_iterator i = m_stack.find(entry->path);
    if ( i == m_stack.end() )   {
      m_stack.insert(make_pair(entry->path,entry));
      return true;
    }
    throw runtime_error("AlignmentStack> The entry with path "+entry->path+
			" cannot be re-aligned twice in one transaction.");
  }
  throw runtime_error("AlignmentStack> Attempt to apply an invalid alignment entry.");
}

/// Retrieve an alignment entry of the current stack
auto_ptr<AlignmentStack::StackEntry> AlignmentStack::pop()   {
  Stack::iterator i = m_stack.begin();
  if ( i != m_stack.end() )   {
    StackEntry* e = (*i).second;
    m_stack.erase(i);
    return auto_ptr<StackEntry>(e);
  }
  throw runtime_error("AlignmentStack> Alignment stack is empty. "
		      "Cannot pop entries - check size first!");
}

/// Get all pathes to be aligned
vector<const AlignmentStack::StackEntry*> AlignmentStack::entries() const    {
  vector<const StackEntry*> result;
  result.reserve(m_stack.size());
  for(Stack::const_iterator i=m_stack.begin(); i != m_stack.end(); ++i)
    result.push_back((*i).second);
  return result;
}

