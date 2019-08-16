//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : A. Sailer
//
//==========================================================================

//Header to check if the current compiler and stdlibrary implementation supports filesystem

#include <filesystem>

#include <iostream>
namespace fs = std::filesystem;

int main ()
{
    fs::space_info devi = fs::space("/dev/null");
    fs::space_info tmpi = fs::space("/tmp");
    std::cout << ".        Capacity       Free      Available\n"
              << "/dev:   " << devi.capacity << "   "
              << devi.free << "   " << devi.available  << '\n'
              << "/tmp: " << tmpi.capacity << " "
              << tmpi.free << " " << tmpi.available  << '\n';
  return 0;
}
