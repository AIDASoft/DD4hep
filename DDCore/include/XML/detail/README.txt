
XML/detail
----------

This directory is used to implement helpers to input parsers.
These common implementation of the helpers should ensure that all
dependent parsers offer a rather identical interface to users.
The header files in this directory should NEVER be included
directly. There are corresponding specialized wrappers present
in the XML or JSON branch, which allow to use the required
functionalities.

JSON should become however a valid alternative to parsing XML files
with a small maintenance overhead, since the enhancement implementation
present in XMLDimesion.h, XMLDetector.h and XMLChildValue.h is shared
and instantiated for both.

The base interface is implemented in the Element header file,
but all derived classes such as Dimension, Component or Detector
have the identical interface and are supposed to ease the access
to inditvidual attributes or child elements independent if the
underlying implementation is based on:

-- XercesC  --> uses the XML branch  (requires compiler definition)
-- TinyXML  --> uses the XML branch  (default)
-- JSON     --> Uses the JSON branch (requires boost presence)
                with the implementation of boost::property_tree
-- RapitXML --> Not implmented.
                Would be a jiffy to implement using the JSON branch and
                the implementation of boost::property_tree.
                Only the document handling is different.
