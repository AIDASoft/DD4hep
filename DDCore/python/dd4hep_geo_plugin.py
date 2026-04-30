# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
#
# dd4hep_geo_plugin.py — helpers for Python-based DD4hep geometry plugins.
#
# Usage in a geometry plugin module:
#   from dd4hep_geo_plugin import *
#
# Provides:
#   - DD4hep header and cppyy initialisation (idempotent)
#   - C++ helper namespace `dd4hep_geo_plugin` with safe wrappers for XML
#     attribute access and xml_coll_t iteration
#   - Python wrapper functions: _handle, _collValid, _hasAttr, _getAttrBool
#   - Convenience type aliases covering common DD4hep geometry types
#
# Background — three cppyy limitations that require workarounds:
#
# 1. dd4hep::xml::Element::hasAttr / attr<T> accept const XmlChar* (= const
#    char16_t* with the Xerces-C backend).  cppyy cannot auto-convert a Python
#    str to char16_t*, so direct calls raise TypeError.  The C++ helpers below
#    accept const char* and convert via dd4hep::xml::Strng_t.
#
# 2. xml_coll_t inherits from Collection_t but cppyy does NOT map its
#    operator bool() to Python __bool__, so `while coll:` loops forever and
#    eventually segfaults on a NULL pointer.  Use _collValid(coll) instead.
#
# 3. xml_coll_t's constructor expects a Handle_t but cppyy will not
#    auto-upcast xml_det_t / xml_comp_t even though C++ would.  Use
#    _handle(el) to perform the cast explicitly.
#
# ==========================================================================

import cppyy
from ROOT import gInterpreter, TMath  # noqa: F401  (re-exported via __all__)

# ---------------------------------------------------------------------------
# Load DD4hep headers into cppyy (idempotent — safe to call multiple times)
# ---------------------------------------------------------------------------
gInterpreter.ProcessLine('#include "DD4hep/DetFactoryHelper.h"')

# ---------------------------------------------------------------------------
# Thin C++ helpers in namespace dd4hep_geo_plugin
# ---------------------------------------------------------------------------
cppyy.cppdef(r"""
#include "DD4hep/DetFactoryHelper.h"
#ifndef DD4HEP_GEO_PLUGIN_HELPERS_DEFINED
#define DD4HEP_GEO_PLUGIN_HELPERS_DEFINED
namespace dd4hep_geo_plugin {
  /// Test for a named attribute; accepts plain C string (avoids char16_t* conversion).
  inline bool hasAttr(dd4hep::xml::Element el, const char* name) {
    return el.hasAttr(dd4hep::xml::Strng_t(name));
  }
  /// Read a named attribute as std::string.
  inline std::string getAttrStr(dd4hep::xml::Element el, const char* name) {
    return el.attr<std::string>(dd4hep::xml::Strng_t(name));
  }
  /// Read a named attribute as double (DD4hep unit-aware).
  inline double getAttrDbl(dd4hep::xml::Element el, const char* name) {
    return el.attr<double>(dd4hep::xml::Strng_t(name));
  }
  /// Read a named attribute as bool.
  inline bool getAttrBool(dd4hep::xml::Element el, const char* name) {
    return el.attr<bool>(dd4hep::xml::Strng_t(name));
  }
  /// Return true if the xml_coll_t iterator is still valid.
  /// Use instead of operator bool(), which cppyy does not map to __bool__.
  inline bool collValid(const dd4hep::xml::Collection_t& c) {
    return static_cast<bool>(c);
  }
}
#endif // DD4HEP_GEO_PLUGIN_HELPERS_DEFINED
""")


# ---------------------------------------------------------------------------
# Python wrapper functions
# ---------------------------------------------------------------------------

def _handle(el):
    """Cast any xml Element type to Handle_t.

    Required because cppyy does not auto-upcast xml_det_t / xml_comp_t to
    Handle_t even though C++ would do so implicitly.  xml_coll_t's constructor
    expects a Handle_t, so pass _handle(el) instead of el directly.
    """
    return cppyy.gbl.dd4hep.xml.Handle_t(el.ptr())


def _collValid(c) -> bool:
    """Return True if an xml_coll_t iterator is still valid.

    Never write ``while c:`` — cppyy maps Python __bool__ to the default
    object truthiness (always True), not to C++ operator bool(). Use
    ``while _collValid(c):`` instead to avoid an infinite loop and segfault.
    """
    return cppyy.gbl.dd4hep_geo_plugin.collValid(c)


def _hasAttr(el, name: str) -> bool:
    """Return True if the XML element has the named attribute."""
    return cppyy.gbl.dd4hep_geo_plugin.hasAttr(el, name)


def _getAttrStr(el, name: str) -> str:
    """Return the named XML attribute as a string."""
    return cppyy.gbl.dd4hep_geo_plugin.getAttrStr(el, name)


def _getAttrDbl(el, name: str) -> float:
    """Return the named XML attribute as a double (DD4hep unit-aware)."""
    return cppyy.gbl.dd4hep_geo_plugin.getAttrDbl(el, name)


def _getAttrBool(el, name: str) -> bool:
    """Return the named XML attribute as a bool."""
    return cppyy.gbl.dd4hep_geo_plugin.getAttrBool(el, name)


# ---------------------------------------------------------------------------
# DD4hep type aliases
# ---------------------------------------------------------------------------
_dd4hep = cppyy.gbl.dd4hep

# -- Core detector / volume types
Assembly = _dd4hep.Assembly
DetElement = _dd4hep.DetElement
PlacedVolume = _dd4hep.PlacedVolume
Volume = _dd4hep.Volume

# -- Transforms and positions
Position = _dd4hep.Position
Transform3D = _dd4hep.Transform3D
Translation3D = _dd4hep.Translation3D
Rotation3D = _dd4hep.Rotation3D
RotationX = _dd4hep.RotationX
RotationY = _dd4hep.RotationY
RotationZ = _dd4hep.RotationZ
RotationZYX = _dd4hep.RotationZYX

# -- Shapes (common subset sufficient for most detector geometry)
Box = _dd4hep.Box
Tube = _dd4hep.Tube
CutTube = _dd4hep.CutTube
Cone = _dd4hep.Cone
ConeSegment = _dd4hep.ConeSegment
Sphere = _dd4hep.Sphere
Torus = _dd4hep.Torus
Polycone = _dd4hep.Polycone
Trapezoid = _dd4hep.Trapezoid
SubtractionSolid = _dd4hep.SubtractionSolid
UnionSolid = _dd4hep.UnionSolid
IntersectionSolid = _dd4hep.IntersectionSolid

# -- XML handle types used in detector factory functions
_U = _dd4hep.xml.Strng_t       # equivalent of the C++ _U("tag") macro
_toString = _dd4hep.xml._toString     # equivalent of the C++ _toString(n, "fmt") macro

xml_det_t = cppyy.gbl.xml_det_t
xml_coll_t = cppyy.gbl.xml_coll_t
xml_comp_t = cppyy.gbl.xml_comp_t

# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------
__all__ = [
    # ROOT helper
    "TMath",
    # Python wrapper functions
    "_handle", "_collValid",
    "_hasAttr", "_getAttrStr", "_getAttrDbl", "_getAttrBool",
    # Core types
    "Assembly", "DetElement", "PlacedVolume", "Volume",
    # Transforms
    "Position", "Transform3D", "Translation3D",
    "Rotation3D", "RotationX", "RotationY", "RotationZ", "RotationZYX",
    # Shapes
    "Box", "Tube", "CutTube", "Cone", "ConeSegment",
    "Sphere", "Torus", "Polycone", "Trapezoid",
    "SubtractionSolid", "UnionSolid", "IntersectionSolid",
    # XML helpers
    "_U", "_toString", "xml_det_t", "xml_coll_t", "xml_comp_t",
    ]
