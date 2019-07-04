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
#include "tbb/tbb.h"
#include <iostream>
using namespace tbb;
using namespace std;

class say_hello {
  int cnt = 0;
  const char* message;
public:
  
  say_hello(const char* str, int i) : message(str), cnt(i) {  }
  void operator( ) ( ) const { 
    cout << message << " " << cnt << endl;
  }
};

int main( )
{
  task_scheduler_init init(2);
  task_group tg1, tg2, tg3;
  for(int i=0; i<200; ++i)  {
    tg1.run(std::move(say_hello("child(1)",i)));
    tg2.run(std::move(say_hello("child(2)",i)));
    tg3.run(std::move(say_hello("child(3)",i)));
  }
  tg1.wait( ); // wait for tasks to complete
  tg2.wait( ); // wait for tasks to complete
  tg3.wait( ); // wait for tasks to complete
}
