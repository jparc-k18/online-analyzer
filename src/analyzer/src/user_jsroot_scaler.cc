// -*- C++ -*-

// Author: Shuhei Hayakawa

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TString.h>

#include <THttpServer.h>
#include <TKey.h>
#include <TSystem.h>
#include <TTimeStamp.h>

#include <DAQNode.hh>
#include <filesystem_util.hh>
#include <Unpacker.hh>
#include <UnpackerConfig.hh>
#include <UnpackerManager.hh>
#include <std_ostream.hh>

#include "user_analyzer.hh"

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "HttpServer.hh"
#include "ScalerAnalyzer.hh"

#define CFT 0

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    HttpServer&   gHttp = HttpServer::GetInstance();
    ScalerAnalyzer& gScaler = ScalerAnalyzer::GetInstance();
  }

//____________________________________________________________________________
Int_t
process_begin( const std::vector<std::string>& argv )
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  if( !gConfMan.IsGood() ) return -1;

  gHttp.SetPort(9092);
  gHttp.Open();
  gHttp.SetItemField("/","_monitoring","100");
  gHttp.SetItemField("/","_layout","vert2");
  gHttp.SetItemField("/","_drawitem","[Scaler,Tag]");
  gHttp.CreateItem("/Scaler", "DAQ Scaler");
  gHttp.SetItemField("/Scaler", "_kind", "Text");
  gHttp.CreateItem("/Tag", "Tag Check");
  gHttp.SetItemField("/Tag", "_kind", "Text");
  std::stringstream ss;
  ss << "<div style='color: white; background-color: black;"
     << "width: 100%; height: 100%;'>"
     << "Tag cheker is running" << "</div>";
  gHttp.SetItemField("/Tag", "value", ss.str().c_str());
  gHttp.Hide("/Reset");

  gScaler.SetFlag( ScalerAnalyzer::kSeparateComma );
  gScaler.SetFlag( ScalerAnalyzer::kSpillBySpill );

  //////////////////// Set Channels
  // ScalerAnalylzer::Set( Int_t column,
  //                       Int_t raw,
  //                       ScalerInfo( name, module, channel ) );
  // scaler information is defined from here.
  // please do not use a white space character.
  {
    Int_t c = ScalerAnalyzer::kLeft;
    Int_t r = 0;
    gScaler.Set( c, r++, ScalerInfo( "10M-Clock",  0, 50 ) );
    gScaler.Set( c, r++, ScalerInfo( "Beam",       0,  0 ) );
    gScaler.Set( c, r++, ScalerInfo( "pi-Beam",    0,  1 ) );
    gScaler.Set( c, r++, ScalerInfo( "p-Beam",     0,  2 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1",        0,  3 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-01",     0,  4 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-02",     0,  5 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-03",     0,  6 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-04",     0,  7 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-05",     0,  8 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-06",     0,  9 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-07",     0, 10 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-08",     0, 11 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-09",     0, 12 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-10",     0, 13 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-11",     0, 14 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2",        0, 15 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-01",     0, 16 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-02",     0, 17 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-03",     0, 18 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-04",     0, 19 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-05",     0, 20 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-06",     0, 21 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-07",     0, 22 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-08",     0, 23 ) );
    gScaler.Set( c, r++, ScalerInfo( "SAC",        0, 24 ) );
    gScaler.Set( c, r++, ScalerInfo( "SCH",        0, 25 ) );
    gScaler.Set( c, r++, ScalerInfo( "FHT1",       1, 62 ) );
    gScaler.Set( c, r++, ScalerInfo( "FHT2",       1, 63 ) );
    gScaler.Set( c, r++, ScalerInfo( "TOF",        0, 26 ) );
    gScaler.Set( c, r++, ScalerInfo( "TOF-HT",     0, 27 ) );
    gScaler.Set( c, r++, ScalerInfo( "TOF-24",     0, 63 ) );
    gScaler.Set( c, r++, ScalerInfo( "LC",         0, 28 ) );
  }

  {
    Int_t c = ScalerAnalyzer::kCenter;
    Int_t r = 0;
    gScaler.Set( c, r++, ScalerInfo( "BGO",            1, 54 ) );
    // gScaler.Set( c, r++, ScalerInfo( "BGO-01",         1,  0 ) );
    // gScaler.Set( c, r++, ScalerInfo( "BGO-02",         1,  1 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-03",         1,  2 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-04",         1, 13 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-05",         1,  4 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-06",         1,  5 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-07",         1,  6 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-08",         1,  7 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-09",         1,  8 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-10",         1, 15 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-11",         1, 10 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-12",         1, 11 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-13",         1, 16 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-14",         1, 17 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-15",         1, 18 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-16",         1, 19 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-17",         1, 20 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-18",         1, 21 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-19",         1, 22 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-20",         1, 23 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-21",         1, 24 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-22",         1, 25 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-23",         1, 26 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-24",         1, 27 ) );
    gScaler.Set( c, r++, ScalerInfo( "CFT-Phi1",       1, 55 ) );
    gScaler.Set( c, r++, ScalerInfo( "CFT-Phi2",       1, 56 ) );
    gScaler.Set( c, r++, ScalerInfo( "CFT-Phi3",       1, 57 ) );
    gScaler.Set( c, r++, ScalerInfo( "CFT-Phi4",       1, 58 ) );
    gScaler.Set( c, r++, ScalerInfo( "CFT-3Coin",      1, 53 ) );
    gScaler.Set( c, r++, ScalerInfo( "PiID",           1, 59 ) );
    gScaler.Set( c, r++, ScalerInfo( "pi-Scat",        1, 60 ) );
    gScaler.Set( c, r++, ScalerInfo( "p-Scat",         1, 61 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-SUM",       -1, -1 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-SUM",       -1, -1 ) );
    // gScaler.Set( c, r++, ScalerInfo( "SCH-SUM",       -1, -1 ) );
  }

  {
    Int_t c = ScalerAnalyzer::kRight;
    Int_t r = 0;
    gScaler.Set( c, r++, ScalerInfo( "Spill",         0, 49 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-1/100-PS",  0, 51 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-1/1e5-PS",  0, 39 ) );
    gScaler.Set( c, r++, ScalerInfo( "TM",            0, 52 ) );
    gScaler.Set( c, r++, ScalerInfo( "Real-Time",     0, 53 ) );
    gScaler.Set( c, r++, ScalerInfo( "Live-Time",     0, 54 ) );
    gScaler.Set( c, r++, ScalerInfo( "L1-Req",        0, 55 ) );
    gScaler.Set( c, r++, ScalerInfo( "L1-Acc",        0, 56 ) );
    gScaler.Set( c, r++, ScalerInfo( "Matrix",        0, 57 ) );
    gScaler.Set( c, r++, ScalerInfo( "Mst-Acc",       0, 58 ) );
    gScaler.Set( c, r++, ScalerInfo( "Mst-Clr",       0, 59 ) );
    gScaler.Set( c, r++, ScalerInfo( "Clear",         0, 60 ) );
    gScaler.Set( c, r++, ScalerInfo( "L2-Req",        0, 61 ) );
    gScaler.Set( c, r++, ScalerInfo( "L2-Acc",        0, 62 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,K)",       0, 40 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,TOF)",     0, 34 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,pi)",      0, 35 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,p)",       0, 36 ) );
    gScaler.Set( c, r++, ScalerInfo( "(p,BGO)",       0, 37 ) );
    gScaler.Set( c, r++, ScalerInfo( "Extra",         0, 38 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,K)-PS",    0, 47 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,TOF)-PS",  0, 42 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,pi)-PS",   0, 43 ) );
    gScaler.Set( c, r++, ScalerInfo( "(BH2,p)-PS",    0, 44 ) );
    gScaler.Set( c, r++, ScalerInfo( "(p,BGO)-PS",    0, 45 ) );
    gScaler.Set( c, r++, ScalerInfo( "Extra-PS",      0, 46 ) );
    gScaler.Set( c, r++, ScalerInfo( "Beam-PS",       0, 41 ) );
    gScaler.Set( c, r++, ScalerInfo( "K-Scat",        0, 48 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other1",        0, 29 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other2",        0, 30 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other3",        0, 31 ) );
    gScaler.Set( c, r++, ScalerInfo( "BGO-OR",        0, 32 ) );
    gScaler.Set( c, r++, ScalerInfo( "LAC",           0, 33 ) );
  }

  gScaler.PrintFlags();

  return 0;
}

//____________________________________________________________________________
Int_t
process_end()
{
  return 0;
}

//____________________________________________________________________________
Int_t
process_event( void )
{
  static auto& gUnpacker = GUnpacker::get_instance();
  static auto* root = gUnpacker.get_root();
  static auto& gConfig = GConfig::get_instance();
  static const TString tout = gConfig.get_control_param("tout");

  static Int_t count = 0;
  count++;

  Int_t run_number = root->get_run_number();
  Int_t event_number = gUnpacker.get_event_number();

  std::stringstream ss;

  // Tag
  ss.str("");
  ss << "<div style='color: white; background-color: black;"
     << "width: 100%; height: 100%;'>";
  ss << "RUN " << run_number << "   Event " << event_number
     << "<br>";
  if( !gUnpacker.is_good() ){
    std::ifstream ifs(tout);
    if( ifs.good() ){
      TString buf;
      while( !ifs.eof() ){
	buf.ReadLine(ifs, false);
	if( buf.Contains("!") && !buf.Contains("............!") )
	  buf = "<font color='yellow'>" + buf + "</font>";
	// if( buf.Contains("bUuSELselthdg") )
	//   ss << TString(' ', 24);
	ss << buf << "<br>";
      }
      ifs.close();
      tag_summary.seekp(0, std::ios_base::beg);
    } else {
      ss << Form("Failed to read %s", tout.Data());
    }
    ss << "</div>";
    gHttp.SetItemField("/Tag", "value", ss.str().c_str());
  }

  // Scaler
  gScaler.Decode();
  if( count%50 != 0 && !gScaler.IsSpillEnd() )
  // if( !gScaler.IsSpillEnd() )
    return 0;

  if ( gUnpacker.is_good() ){
    ss << "Tag cheker is running" << "</div>";
    gHttp.SetItemField("/Tag", "value", ss.str().c_str());
  }

  ss.str("");
  ss << "<div style='color: white; background-color: black;"
     << "width: 100%; height: 100%;'>";
  ss << "<table border=\"0\" width=\"700\" cellpadding=\"0\">";
  TString end_mark = gScaler.IsSpillEnd() ? "Spill End" : "";
  ss << "<tr><td width=\"100\">RUN</td><td align=\"right\" width=\"100\">"
     << gScaler.SeparateComma( run_number ) << "</td><td width=\"100\">"
     << " : Event Number" << "</td><td align=\"right\">"
     << gScaler.SeparateComma( event_number ) << "</td>"
     << "<td width=\"100\">" << " : " << "</td>"
     << "<td align=\"right\" width=\"100\">" << end_mark << "</td>"
     << "<tr><td></td><td></td><td></td></tr>";
  for(Int_t j=0; j<ScalerAnalyzer::MaxRow; ++j){
    ss << "<tr>";
    for(Int_t i=0; i<ScalerAnalyzer::MaxColumn; ++i){
      ss << "<td>";
      if(i != 0)
	ss << " : ";
      ss << gScaler.GetScalerName(i, j) << "</td>"
	 << "<td align=\"right\">" << gScaler.SeparateComma(gScaler.Get(i, j)) << "</td>";
    }
    ss << "</tr>";
  }
  ss << "<tr><td></td><td></td><td></td></tr>"
     << "<tr><td>Beam/TM</td>"
     << "<td align=\"right\">" << gScaler.Fraction("Beam", "TM") << "</td>"
     << "<td> : Live/Real</td>"
     << "<td align=\"right\">" << gScaler.Fraction("Live-Time","Real-Time") << "</td>"
     << "<td> : DAQ-Eff</td>"
     << "<td align=\"right\">" << gScaler.Fraction("L1-Acc","L1-Req") << "</td>"
     << "</tr></tr>"
     << "<td>(BH2,K)/Beam</td>"
     << "<td align=\"right\">" << gScaler.Fraction("(BH2,K)", "Beam") << "</td>"
     << "<td> : L2-Eff</td>"
     << "<td align=\"right\">" << gScaler.Fraction("L2-Acc","L1-Acc") << "</td>"
     << "<td> : Duty-Factor</td>"
     << "<td align=\"right\">" << gScaler.Duty() << "</td>"
     << "</tr>";
  ss << "</table>";
  ss << "</div>";
  gHttp.SetItemField("/Scaler", "value", ss.str().c_str());

  if( gScaler.IsSpillEnd() ){
    gSystem->Sleep(150);
    gSystem->ProcessEvents();
  }

  return 0;
}

}
