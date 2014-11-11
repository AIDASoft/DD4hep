// $Id: Geant4Converter.cpp 588 2013-06-03 11:41:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDD.h"
#include "DD4hep/Objects.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/Printout.h"
#include <cstdarg>
#include <stdexcept>

static size_t _the_printer(void*, DD4hep::PrintLevel, const char* src, const char* text) {
  size_t len = ::fputs(src, stdout);
  len += fputs(": ", stdout);
  len += fputs(text, stdout);
  ::fflush(stdout);
  return len;
}

static DD4hep::PrintLevel print_lvl = DD4hep::INFO;
static void* print_arg = 0;
static DD4hep::output_function_t print_func = _the_printer;

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const char* fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, fmt);
    printout(severity, src, fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const std::string& src, const char* fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, fmt);
    printout(severity, src.c_str(), fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const std::string& fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, &fmt);
    printout(severity, src, fmt.c_str(), args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const std::string& src, const std::string& fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, &fmt);
    printout(severity, src.c_str(), fmt.c_str(), args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const char* fmt, va_list& args) {
  if (severity >= print_lvl) {
    char str[4096];
    size_t len = vsnprintf(str, sizeof(str) - 2, fmt, args);
    str[len] = '\n';
    str[len + 1] = '\0';
    print_func(print_arg, severity, src, str);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const std::string& src, const char* fmt, va_list& args) {
  return printout(severity, src.c_str(), fmt, args);
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const std::string& fmt, va_list& args) {
  return printout(severity, src, fmt.c_str(), args);
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const std::string& src, const std::string& fmt, va_list& args) {
  return printout(severity, src.c_str(), fmt.c_str(), args);
}

/** Build exception string and throw std::runtime_error
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
std::string DD4hep::format(const std::string& src, const std::string& fmt, ...) {
  va_list args;
  va_start(args, &fmt);
  std::string str = format(src, fmt, args);
  va_end(args);
  return str;
}

/** Build exception string and throw std::runtime_error
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  @return Status code indicating success or failure
 */
std::string DD4hep::format(const std::string& src, const std::string& fmt, va_list& args) {
  char str[4096];
  size_t len = ::snprintf(str, sizeof(str), "%s: ", src.c_str());
  ::vsnprintf(str + len, sizeof(str) - len, fmt.c_str(), args);
  return std::string(str);
}

/// Set new print level. Returns the old print level
DD4hep::PrintLevel DD4hep::setPrintLevel(PrintLevel new_level) {
  PrintLevel old = print_lvl;
  print_lvl = new_level;
  return old;
}

/// Access the current printer level
DD4hep::PrintLevel DD4hep::printLevel()  {
  return print_lvl;
}

/// Customize printer function
void DD4hep::setPrinter(void* arg, output_function_t fcn) {
  print_arg = arg;
  print_func = fcn;
}

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
using namespace std;
namespace DD4hep {
  using namespace Geometry;

  template <typename T> void PrintMap<T>::operator()() const {
    Printer < T > p(lcdd, os);
    os << "++" << endl << "++          " << text << endl << "++" << endl;
    for (LCDD::HandleMap::const_iterator i = cont.begin(); i != cont.end(); ++i)
      p((*i).second);
  }

  template <> void Printer<Handle<NamedObject> >::operator()(const Handle<NamedObject>& val) const {
    printout(INFO, "Printer", "%s ++ Handle:%s %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }
  template <> void Printer<Handle<TNamed> >::operator()(const Handle<TNamed>& val) const {
    printout(INFO, "Printer", "%s ++ Handle:%s %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }

  template <> void Printer<Constant>::operator()(const Constant& val) const {
    printout(INFO, "Printer", "%s ++ Constant:%s %s", prefix.c_str(), val->GetName(), val.toString().c_str());
  }

  template <> void Printer<Material>::operator()(const Material& val) const {
    printout(INFO, "Printer", "%s ++ Material:%s %s", prefix.c_str(), val->GetName(), val.toString().c_str());
  }

  template <> void Printer<VisAttr>::operator()(const VisAttr& val) const {
    printout(INFO, "Printer", "%s ++ VisAttr: %s", prefix.c_str(), val.toString().c_str());
  }

  template <> void Printer<Readout>::operator()(const Readout& val) const {
    printout(INFO, "Printer", "%s ++ Readout: %s of type %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }

  template <> void Printer<Region>::operator()(const Region& val) const {
    printout(INFO, "Printer", "%s ++ Region:  %s of type %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }

  template <> void Printer<RotationZYX>::operator()(const RotationZYX& val) const {
    printout(INFO, "Printer", "%s ++ ZYXRotation: phi: %7.3 rad theta: %7.3 rad psi: %7.3 rad", prefix.c_str(), val.Phi(),
        val.Theta(), val.Psi());
  }

  template <> void Printer<Position>::operator()(const Position& val) const {
    printout(INFO, "Printer", "%s ++ Position:    x: %9.3 mm y: %9.3 mm z: %9.3 mm", prefix.c_str(), val.X(), val.Y(), val.Z());
  }
#if 0
  template <> void Printer<LimitSet>::operator()(const LimitSet& val) const {
    const set<Limit>& o = val.limits();
    printout(INFO, "Printer", "%s ++ LimitSet: %s", prefix.c_str(), val.name());
    val->TNamed::Print();
    for (set<Limit>::const_iterator i = o.begin(); i != o.end(); ++i) {
      os << "++    Limit:" << (*i).name << " " << (*i).particles << " [" << (*i).unit << "] " << (*i).content << " "
          << (*i).value << endl;
    }
  }

  template <> void Printer<DetElement>::operator()(const DetElement& val) const {
    DetElement::Object* obj = val.data<DetElement::Object>();
    if (obj) {
      char text[256];
      const DetElement& sd = val;
      PlacedVolume plc = sd.placement();
      bool vis = plc.isValid();
      bool env = plc.isValid();
      bool mat = plc.isValid();
      ::snprintf(text, sizeof(text), "ID:%-3d Combine Hits:%3s Material:%s Envelope:%s VisAttr:%s", sd.id(),
          yes_no(sd.combineHits()), mat ? plc.material().name() : yes_no(mat),
          env ? plc.motherVol()->GetName() : yes_no(env), yes_no(vis));
      os << prefix << "+= DetElement: " << val.name() << " " << val.type() << endl;
      os << prefix << "|               " << text << endl;

      if (vis) {
        VisAttr attr = plc.volume().visAttributes();
        VisAttr::Object* v = attr.data<VisAttr::Object>();
        TColor* col = gROOT->GetColor(v->color);
        char text[256];
        ::snprintf(text, sizeof(text), " RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s", col->AsHexString(),
		   v->color, col->GetAlpha(), int(v->drawingStyle), 
		   int(v->lineStyle), yes_no(v->showDaughters),
		   yes_no(v->visible));
        os << prefix << "|               VisAttr:  " << setw(32) << left << attr.name() << text << endl;
      }
      if (plc.isValid()) {
        Volume vol = plc.volume();
        Solid s = vol.solid();
        Material m = vol.material();
        ::snprintf(text, sizeof(text), "Volume:%s Shape:%s Material:%s", vol->GetName(), s.isValid() ? s.name() : "Unknonw",
            m.isValid() ? m->GetName() : "Unknown");
        os << prefix << "+-------------  " << text << endl;
      }
      const DetElement::Children& ch = sd.children();
      for (DetElement::Children::const_iterator i = ch.begin(); i != ch.end(); ++i)
        Printer < DetElement > (lcdd, os, prefix + "| ")((*i).second);
      return;
    }
  }
#endif
  template <> void Printer<const LCDD*>::operator()(const LCDD* const &) const {
    //Header(lcdd.header()).fromCompact(doc,compact.child(Tag_info),Strng_t("In memory"));
    PrintMap < Constant > (lcdd, os, lcdd->constants(), "List of Constants")();
    //PrintMap < VisAttr > (lcdd, os, lcdd->visAttributes(), "List of Visualization attributes")();
    //PrintMap < LimitSet > (lcdd, os, lcdd->readouts(), "List of Readouts")();
    //PrintMap < Region > (lcdd, os, lcdd->regions(), "List of Regions")();
    //PrintMap < DetElement > (lcdd, os, lcdd->detectors(), "List of DetElements")();
  }
}
