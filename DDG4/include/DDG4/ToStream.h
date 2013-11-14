// $Id: ToStream.h,v 1.8 2008/10/30 23:38:46 marcocle Exp $
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.8 $
// ============================================================================
#ifndef DD4HEPPROPERTYPARSERS_PARSERVALUETOSTREAM_H
#define DD4HEPPROPERTYPARSERS_PARSERVALUETOSTREAM_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <sstream>

// ============================================================================
/** @file DD4hepKernel/ToStream.h
 *  implemenattiono fvarioud functions for streaming.
 *  this functionality is essenital for usag eof varuodu types as property for
 *  the various DD4hep components
 *  @attention the implemenation of the specific specializations must be done
 *                    before the inclusion of this file
 *  @todo ToStream.h : reimplement in terms of functors, to allow
 *                     easier especializations
 */
// ============================================================================
namespace DD4hep {
  // ==========================================================================
  namespace Utils {
    // ========================================================================
    /** the generic implementation of the printout to the std::ostream
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    template <class TYPE>
    std::ostream& toStream(const TYPE& obj, std::ostream& s);
    // ========================================================================
    /** the helper function to print the sequence
     *  @param first (INPUT)  begin-iterator for the sequence
     *  @param last  (INPUT)  end-iterator for the sequence
     *  @param s     (UPDATE) the stream itself
     *  @param open  (INPUT)  "open"-symbol
     *  @param close (INPUT)  "close"-symbol
     *  @param delim (INPUT)  "delimiter"-symbol
     *  @return the stream
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-09-15
     */
    template <class ITERATOR>
    inline std::ostream& toStream(ITERATOR first,                       // begin of the sequence
        ITERATOR last,                       //   end of the sequence
        std::ostream& s,                       //            the stream
        const std::string& open,                       //               opening
        const std::string& close,                       //               closing
        const std::string& delim);                     //             delimiter
    // ========================================================================
    /** the printtout of the strings.
     *  the string is printed a'la Python using the quotes
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    inline std::ostream& toStream(const std::string& obj, std::ostream& s) {
      if (std::string::npos == obj.find('\'')) {
        s << "\'" << obj << "\'";
      }
      else {
        s << "\"" << obj << "\"";
      }
      return s;
    }
    /** the printout of boolean values "a'la Python"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-09-09
     */
    inline std::ostream& toStream(const bool obj, std::ostream& s) {
      return s << (obj ? "True" : "False");
    }
    /** the printout of float values with the reasonable precision
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-09-09
     */
    inline std::ostream& toStream(const float obj, std::ostream& s, const int prec = 6) {
      const int p = s.precision();
      return s << std::setprecision(prec) << obj << std::setprecision(p);
    }
    /** the printout of double values with the reasonable precision
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-09-09
     */
    inline std::ostream& toStream(const double obj, std::ostream& s, const int prec = 8) {
      const int p = s.precision();
      return s << std::setprecision(prec) << obj << std::setprecision(p);
    }
    /** the printout of long double values with the reasonable precision
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-09-09
     */
    inline std::ostream& toStream(const long double obj, std::ostream& s, const int prec = 10) {
      const int p = s.precision();
      return s << std::setprecision(prec) << obj << std::setprecision(p);
    }
    // ========================================================================
    /** the partial template specialization of
     *  <c>std::pair<KTYPE,VTYPE></c> printout
     *  the pair is printed a'la Python tuple: " ( a , b )"
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    template <class KTYPE, class VTYPE>
    inline std::ostream& toStream(const std::pair<KTYPE, VTYPE>& obj, std::ostream& s) {
      s << "( ";
      toStream(obj.first, s);
      s << " , ";
      toStream(obj.second, s);
      return s << " )";
    }
    // ========================================================================
    /** the partial template specialization of <c>std::vector<TYPE,ALLOCATOR></c>
     *  printout. The vector is printed a'la Python list: "[ a, b, c ]"
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    template <class TYPE, class ALLOCATOR>
    inline std::ostream& toStream(const std::vector<TYPE, ALLOCATOR>& obj, std::ostream& s) {
      return toStream(obj.begin(), obj.end(), s, "[ ", " ]", " , ");
    }
    // ========================================================================
    /** the partial template specialization of <c>std::list<TYPE,ALLOCATOR></c>
     *  printout. The vector is printed a'la Python list: "[ a, b, c ]"
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-04-08
     */
    template <class TYPE, class ALLOCATOR>
    inline std::ostream& toStream(const std::list<TYPE, ALLOCATOR>& obj, std::ostream& s) {
      return toStream(obj.begin(), obj.end(), s, "[ ", " ]", " , ");
    }
    // ========================================================================
    /** the partial template specialization of <c>std::set<TYPE,CMP,ALLOCATOR></c>
     *  printout. The vector is printed a'la Python list: "[ a, b, c ]"
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    template <class TYPE, class CMP, class ALLOCATOR>
    inline std::ostream& toStream(const std::set<TYPE, CMP, ALLOCATOR>& obj, std::ostream& s) {
      return toStream(obj.begin(), obj.end(), s, "[ ", " ]", " , ");
    }
    // ========================================================================
    /** the partial template specialization of
     *  <c>std::map<KTYPE,VTYPE,CMP,ALLOCATOR></c> printout
     *  the map is printed a'la Python dict: " ( a : b , c: d , e : f )"
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    template <class KTYPE, class VTYPE, class CMP, class ALLOCATOR>
    inline std::ostream& toStream(const std::map<KTYPE, VTYPE, CMP, ALLOCATOR>& obj, std::ostream& s) {
      s << "{ ";
      for (typename std::map<KTYPE, VTYPE, CMP, ALLOCATOR>::const_iterator cur = obj.begin(); obj.end() != cur; ++cur) {
        if (obj.begin() != cur) {
          s << " , ";
        }
        toStream(cur->first, s);
        s << " : ";
        toStream(cur->second, s);
      }
      return s << " }";
    }

    // ========================================================================
    /** the specialization for C-arrays, a'la python tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhenf.nl
     *  @date 2009-10-05
     */
    template <class TYPE, unsigned int N>
    std::ostream& toStream(TYPE (&obj)[N], std::ostream& s) {
      return toStream(obj, obj + N, s, "( ", " )", " , ");
    }
    // ========================================================================
    /** the specialization for C-arrays, a'la python tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhenf.nl
     *  @date 2009-10-05
     */
    template <class TYPE, unsigned int N>
    std::ostream& toStream(const TYPE (&obj)[N], std::ostream& s) {
      return toStream(obj, obj + N, s, "( ", " )", " , ");
    }
    // ========================================================================
    /** the specialization for C-string, a'la python tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhenf.nl
     *  @date 2009-10-05
     */
    template <unsigned int N>
    std::ostream& toStream(char (&obj)[N], std::ostream& s) {
      return toStream(std::string(obj, obj + N), s);
    }
    // ========================================================================
    /** the specialization for C-string, a'la python tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhenf.nl
     *  @date 2009-10-05
     */
    template <unsigned int N>
    std::ostream& toStream(const char (&obj)[N], std::ostream& s) {
      return toStream(std::string(obj, obj + N), s);
    }
    // ========================================================================
    /** the specialization for C-string, a'la python tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhenf.nl
     *  @date 2009-10-05
     */
    inline std::ostream& toStream(const char* obj, std::ostream& s) {
      return toStream(std::string(obj), s);
    }
    // ========================================================================
    /** the generic implementation of the printout to the std::ostream
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     */
    template <class TYPE>
    inline std::ostream& toStream(const TYPE& obj, std::ostream& s) {
      return s << obj;
    }
    // ========================================================================
    /** the helper function to print the sequence
     *  @param first (INPUT)  begin-iterator for the sequence
     *  @param last  (INPUT)  end-iterator for the sequence
     *  @param s     (UPDATE) the stream itself
     *  @param open  (INPUT)  "open"-symbol
     *  @param close (INPUT)  "close"-symbol
     *  @param delim (INPUT)  "delimiter"-symbol
     *  @return the stream
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-09-15
     */
    template <class ITERATOR>
    inline std::ostream& toStream(ITERATOR first,                       // begin of the sequence
        ITERATOR last,                       //   end of the sequence
        std::ostream& s,                       //            the stream
        const std::string& open,                       //               opening
        const std::string& close,                       //               closing
        const std::string& delim)                       //             delimiter
        {
      s << open;
      for (ITERATOR curr = first; curr != last; ++curr) {
        if (first != curr) {
          s << delim;
        }
        toStream(*curr, s);
      }
      s << close;
      //
      return s;
    }
    // ========================================================================
    /** the generic implementation of the type conversion to the string
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-05-12
     *  @see DD4hep::Utils::toStream
     *  @todo need to be compared with boost::lexical_cast
     */
    template <class TYPE>
    inline std::string toString(const TYPE& obj) {
      std::ostringstream s;
      std::ios::fmtflags orig_flags = s.flags();
      s.setf(std::ios::showpoint);   // to display correctly floats
      toStream(obj, s);
      s.flags(orig_flags);
      return s.str();
    }
  // ========================================================================
  }//                                            end of namespace DD4hep::Utils
// ==========================================================================
}//                                                     end of namespace DD4hep
// ============================================================================
// The END
// ============================================================================
#endif
// ============================================================================

