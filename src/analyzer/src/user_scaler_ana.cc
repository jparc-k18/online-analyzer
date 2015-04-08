// Author: Tomonori Takahashi

#include <iostream>
#include <cstdio>
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

  static const int NofCh = 32;

  enum dispColumn{
    left, right,
    size_dispColumn
  };

  enum idModule{
    id_v830, id_v820,
    size_idModule
  };

  struct scaler_info{
    std::string name;
    int  module_id;
    int  ch;
    bool flag_disp;
    
    scaler_info(){;}
    scaler_info(const char* Name, int Id, int Ch, bool flag):
      name(Name), module_id(Id), ch(Ch), flag_disp(flag)
    {;}
  };

  std::vector<scaler_info> cont_info[size_dispColumn];
  
//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);

  for(int i = 0; i<32; ++i){
    cont_info[left].push_back(  scaler_info("NULL", id_v820, i, false) );
    cont_info[right].push_back( scaler_info("NULL", id_v830, i, false) );
  }

  // left column (counter info)
  int index = 0;
  {scaler_info tmp("BH1OR",     id_v820, 0,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-1",     id_v820, 1,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-2",     id_v820, 2,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-3",     id_v820, 3,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-4",     id_v820, 4,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-5",     id_v820, 5,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-6",     id_v820, 6,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-7",     id_v820, 7,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-8",     id_v820, 8,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-9",     id_v820, 9,  true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-10",    id_v820, 10, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH1-11",    id_v820, 11, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BMW",       id_v820, 12, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH2OR",     id_v820, 13, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH2-3",     id_v820, 16, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH2-4",     id_v820, 17, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BH2-5",     id_v820, 18, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BAC1",      id_v820, 21, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("BAC2",      id_v820, 22, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("SAC1",      id_v820, 23, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("TOF",       id_v820, 24, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("SAC3",      id_v820, 25, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("SFV",       id_v820, 26, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("SMF",       id_v820, 27, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("K beam",    id_v820, 28, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("pi beam",   id_v820, 29, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("/p beam",   id_v820, 30, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("Real time", id_v820, 14, true); cont_info[left][index++] = tmp;}
  {scaler_info tmp("Live time", id_v820, 15, true); cont_info[left][index++] = tmp;}

  // right column (DAQ info)
  index = 0;
  {scaler_info tmp("spill",        id_v830, 0,  true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("IM",           id_v830, 1,  true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("TM",           id_v830, 28, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("L1 req",       id_v830, 3,  true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("L1 acc",       id_v820, 19, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("Coin Go",      id_v820, 20, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("Coin clear",   id_v830, 6,  true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("Overflow Go",  id_v830, 7,  true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("Overflow clear",id_v830, 8, true); cont_info[right][index++] = tmp;} 
  {scaler_info tmp("Total clear",  id_v830, 9,  true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("L2 req",       id_v830, 11, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("L2 acc",       id_v830, 12, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(K, pi)",      id_v830, 13, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(ub, ub)",     id_v830, 14, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(pi, pi)",     id_v830, 15, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(K, pi)PS",    id_v830, 16, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(ub, ub)PS",   id_v830, 17, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(ub)PS",       id_v830, 18, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(pi, pi)PS",   id_v830, 19, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("K in",         id_v830, 20, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("pi in",        id_v830, 21, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("pi Tout",      id_v830, 22, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("K Tout",       id_v830, 23, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("SFVx/SAC3",    id_v830, 24, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("pi Out",       id_v830, 25, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("(K,pi) w/oSMF",id_v830, 26, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("TOFx/SFV",     id_v830, 27, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("TOFxSMF",      id_v830, 29, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("BH1xBH2",      id_v830, 10, true); cont_info[right][index++] = tmp;}
  {scaler_info tmp("BH2xTOF",      id_v830, 30, true); cont_info[right][index++] = tmp;}

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
  static UnpackerManager& g_unpacker = GUnpacker::get_instance();
  
  static int run_number = g_unpacker.get_root()->get_run_number();
  static unsigned int prev[size_dispColumn][NofCh] = {{0}, {0}};
  static unsigned int curr[size_dispColumn][NofCh] = {{0}, {0}};
  static unsigned int val[size_dispColumn][NofCh]  = {{0}, {0}};
  
  static int event_count = 0;
  static bool en_disp = false;
  if(event_count%100 == 0){
    en_disp = true;
  }

  // clear console
  if(en_disp) printf("\033[2J");

  if(run_number != g_unpacker.get_root()->get_run_number()){
    for (int col = 0; col<size_dispColumn; ++col){
      for(int i = 0; i<NofCh; ++i){
	val[col][i] = 0;
      }
    }

    run_number = g_unpacker.get_root()->get_run_number();
  }

  {
    // scaler
    static int device_id = g_unpacker.get_device_id("scaler");

    for(int i = 0; i<NofCh; ++i){
      scaler_info info[size_dispColumn];

      info[left]  = cont_info[left][i];
      info[right] = cont_info[right][i];

      if(info[left].flag_disp){
	prev[left][i] = curr[left][i];
	curr[left][i] = g_unpacker.get(device_id, info[left].module_id, 0, info[left].ch, 0);

	if(curr[left][i] < prev[left][i]){
	  prev[left][i] = 0;
	}

	val[left][i] += curr[left][i] - prev[left][i];
      }

      if(info[right].flag_disp){
	prev[right][i] = curr[right][i];
	curr[right][i] = g_unpacker.get(device_id, info[right].module_id, 0, info[right].ch, 0);

	if(curr[right][i] < prev[right][i]){
	  prev[right][i] = 0;
	}

	val[right][i] += curr[right][i] - prev[right][i];
      }

      if(en_disp){
	printf("%-10s %10u : %-15s %10u\n",
	       info[left].name.c_str(),  val[left][i],
	       info[right].name.c_str(), val[right][i]
	       );
      }
    }
  }

  ++event_count;
  en_disp = false;

  return 0;
}

}
