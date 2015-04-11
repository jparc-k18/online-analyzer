// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <vector>

#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "DAQNode.hh"
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
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  int node_id = g_unpacker.get_fe_id("vme01");
  //  int node_id = g_unpacker.get_fe_id("skseb"); // Event builder
  std::cout << std::hex;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_magic) << std::dec << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_data_size) << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_event_number) << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_run_number) << std::hex <<std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_node_id) << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_node_type) << std::dec <<std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_number_of_blocks) << std::dec <<std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_unix_time) << std::dec <<std::endl;

  for(int i = 0; i<24; ++i){
    std::cout << g_unpacker.get_fe_info(node_id, 0x10000000, i, 0) << std::endl;
  }

  g_unpacker.dump_data_fe(node_id, 0x10000000, 3);

  return 0;
}

}
