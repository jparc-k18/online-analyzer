// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <vector>

#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "Unpacker.hh"
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
  //  int node_id = g_unpacker.get_fe_id("vme01");
  int node_id = g_unpacker.get_fe_id("skseb"); // Event builder
  std::cout << std::hex;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_magic) << std::dec << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_data_size) << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_event_number) << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_run_number) << std::hex <<std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_node_id) << std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_node_type) << std::dec <<std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_number_of_blocks) << std::dec <<std::endl;
  std::cout << g_unpacker.get_node_header(node_id, DAQNode::k_unix_time) << std::dec <<std::endl;

  std::cout << "EEE" << std::endl;
  std::cout << g_unpacker.get_root()->get_run_number() << std::endl;
  return 0;
}

}
