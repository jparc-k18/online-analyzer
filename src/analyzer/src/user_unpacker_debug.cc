// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <vector>

#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "ConfMan.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;
  
//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);

  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  return 0;
}

//____________________________________________________________________________
int
process_event()
{
  sleep(1);
  //  GUnpacker::get_instance().dump_data_fe(131);
  return 0;
}

}
