// Author: Tomonori Takahashi

#include <iomanip>
#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

#include "DAQNode.hh"
#include "UnpackerManager.hh"
#include "Unpacker.hh"

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "PrintHelper.hh"
#include "user_analyzer.hh"

#define SeparateComma 1

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  typedef unsigned long long Scaler;

  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  int  run_number;
  bool flag_spill_by_spill = false;
  bool flag_semi_online    = false;

  static const int MaxRow = 32;
  enum eDisp { kLeft, kRight, MaxColumn };

  struct ScalerInfo
  {
    std::string name;
    int         module_id;
    int         channel;
    bool        flag_disp;
    Scaler      data; // integral in a spill
    Scaler      curr; // current value
    Scaler      prev; // previous value

    ScalerInfo( void ) {}
    ScalerInfo( std::string n, int id, int Ch, bool flag)
      : name(n), module_id(id), channel(Ch), flag_disp(flag),
	data(0), curr(0), prev(0)
    {}
  };

  std::vector< std::vector<ScalerInfo> > info;

  ///// for EMC
  static double emc_pos_x_start = -9999.;
  static double emc_pos_y_start = -9999.;
  static double emc_pos_x_stop  = -9999.;
  static double emc_pos_y_stop  = -9999.;
  static const double emc_x_offset = 500000 - 303300;
  static const double emc_y_offset = 500000 + 164000;

//____________________________________________________________________________
inline std::string
separate_comma( Scaler number )
{
  std::vector<Scaler> sep_num;

  while( number/1000 ){
    sep_num.push_back(number%1000);
    number /= 1000;
  }

  std::stringstream ss; ss<<number;
  std::vector<Scaler>::reverse_iterator itr, end=sep_num.rend();
  for( itr=sep_num.rbegin(); itr!=end; ++itr ){
    ss << "," << std::setfill('0') << std::setw(3) << *itr;
  }
  return ss.str();
}

//____________________________________________________________________________
inline Scaler
Get( const std::string& key )
{
  static const std::string func_name("[::Get()]");

  std::vector<ScalerInfo>::const_iterator itr, itr_end;
  for( std::size_t i=0; i<MaxColumn; ++i ){
    itr_end = info[i].end();
    for( itr=info[i].begin(); itr!=itr_end; ++itr ){
      if( itr->name == key ) return itr->data;
    }
  }

  std::cerr << "#W " << func_name << " "
	    << "no such ScalerInfo : " << key << std::endl;

  return 0;
}

//____________________________________________________________________________
inline void
Print( void )
{
  PrintHelper phelper( 6, std::ios::dec | std::ios::fixed, std::cout );

  std::cout << "\033[2J" << std::endl;

  int event_number = g_unpacker.get_event_number();
  std::cout << std::left  << std::setw(12) << "RUN"
	    << std::right << std::setw(16) << run_number << " : "
	    << std::left  << std::setw(12) << "Event Number"
	    << std::right << std::setw(16) << event_number
	    << std::endl  << std::endl;

  double real_time = (double)Get("Real_Time");
  double live_time = (double)Get("Live_Time");
  double l1_req    = (double)Get("L1_Req");
  double l1_acc    = (double)Get("L1_Acc");
  double l2_acc    = (double)Get("L2_Acc");
  double real_live = live_time/real_time;
  double daq_eff   = l1_acc/l1_req;
  double l2_eff    = l2_acc/l1_acc;
  double duty_factor = daq_eff/(1-daq_eff)*(1/real_live - 1);
  if( duty_factor > 1. ) duty_factor = 1.;

  for( std::size_t i=0; i<MaxRow; ++i ){
    std::cout << std::left  << std::setw(12) << info[kLeft][i].name
	      << std::right << std::setw(16)
#if SeparateComma
	      << separate_comma( info[kLeft][i].data )
#else
	      << info[kLeft][i].data
#endif
	      << " : "
	      << std::left  << std::setw(12) << info[kRight][i].name
	      << std::right << std::setw(16)
#if SeparateComma
	      << separate_comma( info[kRight][i].data )
#else
	      << info[kRight][i].data
#endif
	      << std::endl;
  }

  std::cout << std::endl  << std::setprecision(6) << std::fixed
	    << std::left  << std::setw(12) << "Live/Real"
	    << std::right << std::setw(14) << real_live*100 << " % : "
	    << std::left  << std::setw(12) << "DAQ_Eff"
	    << std::right << std::setw(14) << daq_eff*100 << " %" << std::endl
	    << std::left  << std::setw(12) << "L2_Eff"
	    << std::right << std::setw(14) << l2_eff*100 << " % : "
	    << std::left  << std::setw(12) << "Duty factor"
	    << std::right << std::setw(14) << duty_factor*100 << " %"
	    << std::endl  << std::endl;
}

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);

  // spill by spill flag
  if( argv.size() == 4 ){
    flag_spill_by_spill = true;
  }

  // semi online flag
  if( argv.size() == 3 ){
    if( argv[2].find(":",0) == std::string::npos ){
      flag_semi_online = true;
    }
  }

  info.resize(MaxColumn);

  for( int i=0; i<MaxRow; ++i ){
    info[kLeft].push_back( ScalerInfo("n/a", kLeft, i, false) );
    info[kRight].push_back( ScalerInfo("n/a", kRight, i, false) );
  }

  { // kLeft column (counter info)
    int index = 0;
    info[kLeft][index++] = ScalerInfo("K_beam",  0,  0, true);
    info[kLeft][index++] = ScalerInfo("pi_beam", 0,  1, true);
    info[kLeft][index++] = ScalerInfo("/p_beam", 0,  2, true);
    info[kLeft][index++] = ScalerInfo("BH1-OR",  0,  3, true);
    info[kLeft][index++] = ScalerInfo("BH1-1",   0,  4, true);
    info[kLeft][index++] = ScalerInfo("BH1-2",   0,  5, true);
    info[kLeft][index++] = ScalerInfo("BH1-3",   0,  6, true);
    info[kLeft][index++] = ScalerInfo("BH1-4",   0,  7, true);
    info[kLeft][index++] = ScalerInfo("BH1-5",   0,  8, true);
    info[kLeft][index++] = ScalerInfo("BH1-6",   0,  9, true);
    info[kLeft][index++] = ScalerInfo("BH1-7",   0, 10, true);
    info[kLeft][index++] = ScalerInfo("BH1-8",   0, 11, true);
    info[kLeft][index++] = ScalerInfo("BH1-9",   0, 12, true);
    info[kLeft][index++] = ScalerInfo("BH1-10",  0, 13, true);
    info[kLeft][index++] = ScalerInfo("BH1-11",  0, 14, true);
    info[kLeft][index++] = ScalerInfo("BH2-OR",  0, 15, true);
    info[kLeft][index++] = ScalerInfo("BAC1",    0, 16, true);
    info[kLeft][index++] = ScalerInfo("BAC2",    0, 17, true);
    info[kLeft][index++] = ScalerInfo("FBH",     0, 18, true);
    info[kLeft][index++] = ScalerInfo("PVAC",    0, 19, true);
    info[kLeft][index++] = ScalerInfo("FAC",     0, 20, true);
    info[kLeft][index++] = ScalerInfo("SCH",     0, 21, true);
    info[kLeft][index++] = ScalerInfo("TOF",     0, 22, true);
    info[kLeft][index++] = ScalerInfo("23",      0, 23, true);
    info[kLeft][index++] = ScalerInfo("24",      0, 24, true);
    info[kLeft][index++] = ScalerInfo("25",      0, 25, true);
    info[kLeft][index++] = ScalerInfo("26",      0, 26, true);
    info[kLeft][index++] = ScalerInfo("27",      0, 27, true);
    info[kLeft][index++] = ScalerInfo("28",      0, 28, true);
    info[kLeft][index++] = ScalerInfo("29",      0, 29, true);
    info[kLeft][index++] = ScalerInfo("30",      0, 30, true);
    info[kLeft][index++] = ScalerInfo("31",      0, 31, true);
  }
  { // kRight column (DAQ info)
    int index = 0;
    info[kRight][index++] = ScalerInfo("Spill",        0, 32, true);
    info[kRight][index++] = ScalerInfo("10M Clock",    0, 33, true);
    info[kRight][index++] = ScalerInfo("IM",           0, 34, true);
    info[kRight][index++] = ScalerInfo("TM",           0, 35, true);
    info[kRight][index++] = ScalerInfo("Real_Time",    0, 36, true);
    info[kRight][index++] = ScalerInfo("Live_Time",    0, 37, true);
    info[kRight][index++] = ScalerInfo("L1_Req",       0, 38, true);
    info[kRight][index++] = ScalerInfo("L1_Acc",       0, 39, true);
    info[kRight][index++] = ScalerInfo("MTX_Acc",      0, 40, true);
    info[kRight][index++] = ScalerInfo("MTX-1",        0, 41, true);
    info[kRight][index++] = ScalerInfo("MST_Acc",      0, 42, true);
    info[kRight][index++] = ScalerInfo("MST_Clear",    0, 43, true);
    info[kRight][index++] = ScalerInfo("MST_Clear_PS", 0, 44, true);
    info[kRight][index++] = ScalerInfo("L2_Clear",     0, 45, true);
    info[kRight][index++] = ScalerInfo("L2_Req",       0, 46, true);
    info[kRight][index++] = ScalerInfo("L2_Acc",       0, 47, true);
    info[kRight][index++] = ScalerInfo("(ub)",         0, 48, true);
    info[kRight][index++] = ScalerInfo("(ub,ub)",      0, 49, true);
    info[kRight][index++] = ScalerInfo("(pi,TOF)",     0, 50, true);
    info[kRight][index++] = ScalerInfo("(K,K)",        0, 51, true);
    info[kRight][index++] = ScalerInfo("(ub)PS",       0, 52, true);
    info[kRight][index++] = ScalerInfo("(ub,ub)PS",    0, 53, true);
    info[kRight][index++] = ScalerInfo("(pi,TOF)PS",   0, 54, true);
    info[kRight][index++] = ScalerInfo("(K,K)PS",      0, 55, true);
    info[kRight][index++] = ScalerInfo("K_in",         0, 56, true);
    info[kRight][index++] = ScalerInfo("pi_in",        0, 57, true);
    info[kRight][index++] = ScalerInfo("K_out",        0, 58, true);
    info[kRight][index++] = ScalerInfo("pi_out",       0, 59, true);
    info[kRight][index++] = ScalerInfo("BH1xBH2",      0, 60, true);
    info[kRight][index++] = ScalerInfo("BH2xTOF",      0, 61, true);
    info[kRight][index++] = ScalerInfo("PVACx/FAC",    0, 62, true);
    info[kRight][index++] = ScalerInfo("31",           0, 63, true);
  }
  { // kRight column (DAQ info)
    int index = 0;
    info[kRight][index++] = ScalerInfo("Spill",        0, 64, true);
    info[kRight][index++] = ScalerInfo("10M Clock",    0, 65, true);
    info[kRight][index++] = ScalerInfo("IM",           0, 66, true);
    info[kRight][index++] = ScalerInfo("TM",           0, 67, true);
    info[kRight][index++] = ScalerInfo("Real_Time",    0, 68, true);
    info[kRight][index++] = ScalerInfo("Live_Time",    0, 69, true);
    info[kRight][index++] = ScalerInfo("L1_Req",       0, 70, true);
    info[kRight][index++] = ScalerInfo("L1_Acc",       0, 71, true);
    info[kRight][index++] = ScalerInfo("MTX_Acc",      0, 72, true);
    info[kRight][index++] = ScalerInfo("MTX-1",        0, 73, true);
    info[kRight][index++] = ScalerInfo("MST_Acc",      0, 74, true);
    info[kRight][index++] = ScalerInfo("MST_Clear",    0, 75, true);
    info[kRight][index++] = ScalerInfo("MST_Clear_PS", 0, 76, true);
    info[kRight][index++] = ScalerInfo("L2_Clear",     0, 77, true);
    info[kRight][index++] = ScalerInfo("L2_Req",       0, 78, true);
    info[kRight][index++] = ScalerInfo("L2_Acc",       0, 79, true);
    info[kRight][index++] = ScalerInfo("(ub)",         0, 80, true);
    info[kRight][index++] = ScalerInfo("(ub,ub)",      0, 81, true);
    info[kRight][index++] = ScalerInfo("(pi,TOF)",     0, 82, true);
    info[kRight][index++] = ScalerInfo("(K,K)",        0, 83, true);
    info[kRight][index++] = ScalerInfo("(ub)PS",       0, 84, true);
    info[kRight][index++] = ScalerInfo("(ub,ub)PS",    0, 85, true);
    info[kRight][index++] = ScalerInfo("(pi,TOF)PS",   0, 86, true);
    info[kRight][index++] = ScalerInfo("(K,K)PS",      0, 87, true);
    info[kRight][index++] = ScalerInfo("K_in",         0, 88, true);
    info[kRight][index++] = ScalerInfo("pi_in",        0, 89, true);
    info[kRight][index++] = ScalerInfo("K_out",        0, 90, true);
    info[kRight][index++] = ScalerInfo("pi_out",       0, 91, true);
    info[kRight][index++] = ScalerInfo("BH1xBH2",      0, 92, true);
    info[kRight][index++] = ScalerInfo("BH2xTOF",      0, 93, true);
    info[kRight][index++] = ScalerInfo("PVACx/FAC",    0, 94, true);
    info[kRight][index++] = ScalerInfo("31",           0, 95, true);
  }

  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  PrintHelper phelper( 6, std::ios::dec | std::ios::fixed, std::cout );

  std::cout << "\n#D : End of scaler, summarize this run" << std::endl;

  Print();

  Scaler spill      = Get("Spill");
  Scaler kbeam      = Get("K_beam");
  Scaler pibeam     = Get("pi_beam");
  Scaler kk         = Get("(K,K)");
  Scaler bh1xbh2    = Get("BH1xBH2");
  Scaler real_time  = Get("Real_Time");
  Scaler live_time  = Get("Live_Time");
  Scaler l1_req     = Get("L1_Req");
  Scaler l1_acc     = Get("L1_Acc");
  Scaler clear      = Get("L2_Clear");
  Scaler l2_acc     = Get("L2_Acc");
  Scaler bh2        = Get("BH2-OR");
  Scaler fbh        = Get("FBH");
  Scaler sch        = Get("SCH");
  Scaler TM         = Get("TM");
  double daq_eff    = (double)l1_acc/l1_req;
  double real_live  = (double)live_time/real_time;
  double duty_factor = 0;
  if( 1-daq_eff == 0 )
    duty_factor = 1.;
  else
    duty_factor = daq_eff/(1-daq_eff)*(1/real_live - 1);

  // summary
  std::cout << std::endl
	    << "#D : For the check sheet" << std::endl << std::endl
	    << std::left  << std::setw(20) << ""
	    << std::right << std::setw(16) << "" << " : "
	    << std::left  << std::setw(20) << "BH2"
	    << std::right << std::setw(16) << separate_comma(bh2) << std::endl
	    << std::left  << std::setw(20) << "Spill"
	    << std::right << std::setw(16) << separate_comma(spill) << " : "
	    << std::left  << std::setw(20) << "FBH"
	    << std::right << std::setw(16) << separate_comma(fbh) << std::endl
	    << std::left  << std::setw(20) << "K beam"
	    << std::right << std::setw(16) << separate_comma(kbeam) << " : "
	    << std::left  << std::setw(20) << "K beam/TM"
	    << std::right << std::setw(16) << (double)kbeam/TM << std::endl
	    << std::left  << std::setw(20) << "pi beam"
	    << std::right << std::setw(16) << separate_comma(pibeam) << " : "
	    << std::left  << std::setw(20) << "K beam/BH1xBH2"
	    << std::right << std::setw(16) << (double)kbeam/bh1xbh2 << std::endl
	    << std::left  << std::setw(20) << "BH1xBH2"
	    << std::right << std::setw(16) << separate_comma(bh1xbh2) << " : "
	    << std::left  << std::setw(20) << "K beam/SCH"
	    << std::right << std::setw(16) << (double)kbeam/sch << std::endl
	    << std::left  << std::setw(20) << "(K,K)"
	    << std::right << std::setw(16) << separate_comma(kk) << " : "
	    << std::left  << std::setw(20) << "(K,K)/K beam"
	    << std::right << std::setw(16) << (double)kk/kbeam << std::endl
	    << std::left  << std::setw(20) << "L1 Req"
	    << std::right << std::setw(16) << separate_comma(l1_req) << " : "
	    << std::left  << std::setw(20) << "Live/Real"
	    << std::right << std::setw(16) << real_live << std::endl
	    << std::left  << std::setw(20) << "L1 Acc"
	    << std::right << std::setw(16) << separate_comma(l1_acc) << " : "
	    << std::left  << std::setw(20) << "L1 Acc/L1 Req"
	    << std::right << std::setw(16) << daq_eff << std::endl
	    << std::left  << std::setw(20) << "L2 Clear"
	    << std::right << std::setw(16) << separate_comma(clear) << " : "
	    << std::left  << std::setw(20) << "L2 Acc/L1 Acc"
	    << std::right << std::setw(16) << (double)l2_acc/l1_acc << std::endl
	    << std::left  << std::setw(20) << "L2 Acc"
	    << std::right << std::setw(16) << separate_comma(l2_acc) << " : "
	    << std::left  << std::setw(20) << "Duty Factor"
	    << std::right << std::setw(16) << duty_factor << std::endl
	    << std::endl;

  // EMC
  std::cout << std::setprecision(3)
	    << std::left  << std::setw(20) << "EMC Start Position"
	    << "( " << emc_pos_x_start << ", " << emc_pos_y_start << " )"
	    << std::endl
	    << std::left  << std::setw(20) << "EMC Stop Position"
	    << "( " << emc_pos_x_stop << ", " << emc_pos_y_stop << " )"
	    << std::endl << std::endl;

  return 0;

}

//____________________________________________________________________________
int
process_event()
{
  static int  event_count = 0;
  static bool en_disp     = false;
  static int  scaler_id   = g_unpacker.get_device_id("Scaler");

  if( flag_semi_online ){
    if( event_count%300 == 0 ) en_disp = true;
  } else {
    if( event_count%1 == 0 ) en_disp = true;
  }

  // EMC
  {
    static const int k_device = g_unpacker.get_device_id("EMC");
    static const int k_xpos   = g_unpacker.get_data_id("EMC", "xpos");
    static const int k_ypos   = g_unpacker.get_data_id("EMC", "ypos");
    double xpos = -9999.;
    double ypos = -9999.;
    int xpos_nhit = g_unpacker.get_entries( k_device, 0, 0, 0, k_xpos );
    if( xpos_nhit!=0 ) xpos = g_unpacker.get( k_device, 0, 0, 0, k_xpos );
    int ypos_nhit = g_unpacker.get_entries( k_device, 0, 0, 0, k_ypos );
    if( ypos_nhit!=0 ) ypos = g_unpacker.get( k_device, 0, 0, 0, k_ypos );
    if( xpos_nhit>0 && ypos_nhit>0 ){
      if( event_count==0 ){
	emc_pos_x_start = (xpos-emc_x_offset)/1000.;
	emc_pos_y_start = (ypos-emc_y_offset)/1000.;
      }
      emc_pos_x_stop   = (xpos-emc_x_offset)/1000.;
      emc_pos_y_stop   = (ypos-emc_y_offset)/1000.;
    }
  }

  ++event_count;

  if( !g_unpacker.get_entries( scaler_id, 0, 0, 0, 0 ) ){
    en_disp = false;
    return 0;
  }

  if(run_number != g_unpacker.get_root()->get_run_number()){
    run_number = g_unpacker.get_root()->get_run_number();
    for( std::size_t i=0; i<MaxColumn; ++i ){
      for( std::size_t j=0; j<MaxRow; ++j ){
	if( i==kRight && j==0 ) continue;
	info[i][j].data = 0;
      }
    }
  }

  {
    bool inclement_spill = false;
    // scaler
    static int scaler_id    = g_unpacker.get_device_id("Scaler");

    for( std::size_t i=0; i<MaxColumn; ++i ){
      for( std::size_t j=0; j<MaxRow; ++j ){
	if( !info[i][j].flag_disp ) continue;
	if( i==kRight && j==0 ) continue;
	int module_id = info[i][j].module_id;
	int channel   = info[i][j].channel;
	int nhit = g_unpacker.get_entries( scaler_id, module_id, 0, channel, 0 );
	if( nhit<=0 ) continue;
	Scaler val = g_unpacker.get( scaler_id, module_id, 0, channel, 0 );

	if( info[i][j].prev > val ){
	  inclement_spill = true;
	  info[i][j].prev = 0;
	}

	if( flag_spill_by_spill && inclement_spill )
	  info[i][j].data = 0;

	info[i][j].curr  = val;
	info[i][j].data += val - info[i][j].prev;
	info[i][j].prev  = info[i][j].curr;
      }
    }

    // inclement spill
    if( inclement_spill )
      info[kRight][0].data++;

    if( en_disp ){
      Print();
    }
  }

  en_disp = false;

  return 0;
}

}
