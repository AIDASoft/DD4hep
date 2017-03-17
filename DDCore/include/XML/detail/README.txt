
XML/detail
----------

This directory is used to implement helpers to input parsers.
These common implementation of the helpers should ensure that all
dependent parsers offer a rather identical interface to users.

The base interface is implemented in the Element header file,
but all derived classes such as Dimension, Component or Detector
have the identical interface and are supposed to ease the access
to inditvidual attributes or child elements independent if the
underlying implementation is based on:

-- XercesC  --> uses the XML branch
-- TinyXML  --> uses the XML branch
-- JSON     --> Uses the JSON branch
                with the implementation of boost::property_tree
-- RapitXML --> Not implmented.
                Would be a jiffy to implement using the JSON branch and
                the implementation of boost::property_tree.
                Only the document handling is different.

