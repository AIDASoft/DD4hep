//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DDDIGI_DIGIMONITOROPTIONS_H
#define DDDIGI_DIGIMONITOROPTIONS_H

/// Framework include files
#include <DDDigi/DigiAction.h>
#include <DDDigi/DigiData.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Property to parse 1D histogram options
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class H1DParams   {
    public:
      std::string name;
      std::string title;
      int nbin_x;
      double min_x, max_x;
    };

    /// Property to parse 2D histogram options
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class H2DParams   {
    public:
      std::string name;
      std::string title;
      int    nbin_x;
      double min_x, max_x;
      int    nbin_y;
      double min_y, max_y;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIMONITOROPTIONS_H
