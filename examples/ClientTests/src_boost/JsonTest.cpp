//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifdef DD4HEP_USE_BOOST

// Framework include files
#include "JSON/Elements.h"
#include "JSON/DocumentHandler.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::JSON;
using namespace DD4hep::Geometry;

static long json_dump(LCDD& lcdd, int argc, char** argv)   {
  if ( argc < 1 )  {
    return 0;
  }
  DocumentHolder doc(JSON::DocumentHandler().load(argv[0]));
  Element        elt = doc.root();

  for( boost::property_tree::ptree::value_type const& rowPair : elt.ptr()->second.get_child( "" ) ) 
	{
		cout << "1--" << rowPair.first << ": " << endl;
    cout << rowPair.second.get_value<string>() << " " << endl;

		for( boost::property_tree::ptree::value_type const& itemPair : rowPair.second ) 
		{
			cout << "\t2--" << itemPair.first << " " << endl;

		  for( boost::property_tree::ptree::value_type const& node : itemPair.second ) 
			{
				cout << "\t\t3--" << node.first << "=" << node.second.get_value<string>() << endl;
			}

			cout << endl;

		}

		cout << endl;
  }

  return 1;
}

DECLARE_APPLY(DD4hep_JsonDumper,json_dump)

#endif
