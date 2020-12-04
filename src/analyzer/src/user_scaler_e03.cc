// -*- C++ -*-

#include <iomanip>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "DAQNode.hh"
#include "UnpackerManager.hh"
#include "Unpacker.hh"

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "PrintHelper.hh"
#include "ScalerAnalyzer.hh"
#include "user_analyzer.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;

  namespace
  {
    UnpackerManager& gUnpacker = GUnpacker::get_instance();
    ScalerAnalyzer&  gScaler   = ScalerAnalyzer::GetInstance();
    ///// Number of spill for Scaler Sheet
    Scaler nspill_scaler_sheet  = 1;
  }

//____________________________________________________________________________
Int_t
process_begin( const std::vector<std::string>& argv )
{
  ConfMan::GetInstance().Initialize(argv);

  // flags
  gScaler.SetFlag( ScalerAnalyzer::kSeparateComma );
  gScaler.SetFlag( ScalerAnalyzer::kSemiOnline );
  for( Int_t i=0, n=argv.size(); i<n; ++i ){
    TString v = argv[i];
    if( v.Contains("--print") ){
      gScaler.SetFlag( ScalerAnalyzer::kScalerSheet );
    }
    if( v.Contains("--spill=") ){
      nspill_scaler_sheet = v.ReplaceAll("--spill=","").Atoi();
    }
    if( v.Contains(":") ){
      gScaler.SetFlag( ScalerAnalyzer::kSemiOnline, false );
    }
    if( v.Contains("--spill-by-spill") ){
      gScaler.SetFlag( ScalerAnalyzer::kSpillBySpill );
    }
    if( v.Contains("--spillon") ){
      gScaler.SetFlag( ScalerAnalyzer::kSpillOn );
    }
    if( v.Contains("--spilloff") ){
      gScaler.SetFlag( ScalerAnalyzer::kSpillOff );
    }
  }

  //////////////////// Set Channels
  // ScalerAnalylzer::Set( Int_t column,
  //                       Int_t raw,
  //                       ScalerInfo( name, module, channel ) );
  // scaler information is defined from here.
  // please do not use a white space character.
  {
    Int_t c = ScalerAnalyzer::kLeft;
    Int_t r = 0;
    gScaler.Set( c, r++, ScalerInfo( "10M-Clock",  0,  0 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1",        0, 16 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-SUM",   -1, -1 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-01",     1,  0 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-02",     1,  1 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-03",     1,  2 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-04",     1,  3 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-05",     1,  4 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-06",     1,  5 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-07",     1,  6 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-08",     1,  7 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-09",     1,  8 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-10",     1,  9 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-11",     1, 10 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2",        0, 17 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-SUM",   -1, -1 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-01",     0, 64 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-02",     0, 65 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-03",     0, 66 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-04",     0, 67 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-05",     0, 68 ) );
    gScaler.Set( c, r++, ScalerInfo( "BAC",        0, 18 ) );
    gScaler.Set( c, r++, ScalerInfo( "PVAC",       0, 20 ) );
    gScaler.Set( c, r++, ScalerInfo( "FAC",        0, 21 ) );
    gScaler.Set( c, r++, ScalerInfo( "SCH",        0, 22 ) );
    gScaler.Set( c, r++, ScalerInfo( "TOF",        0, 22 ) );
    gScaler.Set( c, r++, ScalerInfo( "TOF-24",     0, 29 ) );
    gScaler.Set( c, r++, ScalerInfo( "LAC",        0, 23 ) );
    gScaler.Set( c, r++, ScalerInfo( "WC",         0, 24 ) );
    gScaler.Set( c, r++, ScalerInfo( "Mtx2D-1",    0, 32 ) );
    gScaler.Set( c, r++, ScalerInfo( "Mtx2D-2",    0, 33 ) );
    gScaler.Set( c, r++, ScalerInfo( "Mtx3D",      0, 34 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other1",     0, 25 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other2",     0, 26 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other3",     0, 27 ) );
    gScaler.Set( c, r++, ScalerInfo( "Other4",     0, 28 ) );
  }

  // {
  //   Int_t c = ScalerAnalyzer::kCenter;
  //   Int_t r = 0;
  //   gScaler.Set( c, r++, ScalerInfo( "BGO",            0, 54 ) );
  // }

  {
    Int_t c = ScalerAnalyzer::kRight;
    Int_t r = 0;
    gScaler.Set( c, r++, ScalerInfo( "Spill",        -1, -1 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-1/100-PS",  1, 11 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH1-1/1e5-PS",  1, 12 ) );
    gScaler.Set( c, r++, ScalerInfo( "TM",            0,  9 ) );
    gScaler.Set( c, r++, ScalerInfo( "SY",            0, 10 ) );
    gScaler.Set( c, r++, ScalerInfo( "Real-Time",     0,  1 ) );
    gScaler.Set( c, r++, ScalerInfo( "Live-Time",     0,  2 ) );
    gScaler.Set( c, r++, ScalerInfo( "L1-Req",        0,  3 ) );
    gScaler.Set( c, r++, ScalerInfo( "L1-Acc",        0,  4 ) );
    gScaler.Set( c, r++, ScalerInfo( "MstClr",        0,  5 ) );
    gScaler.Set( c, r++, ScalerInfo( "Clear",         0,  6 ) );
    gScaler.Set( c, r++, ScalerInfo( "L2-Req",        0,  7 ) );
    gScaler.Set( c, r++, ScalerInfo( "L2-Acc",        0,  8 ) );
    gScaler.Set( c, r++, ScalerInfo( "BEAM-A",        0, 35 ) );
    gScaler.Set( c, r++, ScalerInfo( "BEAM-B",        0, 36 ) );
    gScaler.Set( c, r++, ScalerInfo( "BEAM-C",        0, 37 ) );
    gScaler.Set( c, r++, ScalerInfo( "BEAM-D",        0, 38 ) );
    gScaler.Set( c, r++, ScalerInfo( "BEAM-E",        0, 39 ) );
    gScaler.Set( c, r++, ScalerInfo( "BEAM-F",        0, 40 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-A",        0, 41 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-B",        0, 42 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-C",        0, 43 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-D",        0, 44 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-E",        0, 45 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-F",        0, 46 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-A-PS",     0, 48 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-B-PS",     0, 49 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-C-PS",     0, 50 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-D-PS",     0, 51 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-E-PS",     0, 52 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-F-PS",     0, 53 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-PSOR-A",   0, 54 ) );
    gScaler.Set( c, r++, ScalerInfo( "TRIG-PSOR-B",   0, 55 ) );
    gScaler.Set( c, r++, ScalerInfo( "Clock-PS",      0, 56 ) );
    gScaler.Set( c, r++, ScalerInfo( "Reserve2-PS",   0, 57 ) );
    gScaler.Set( c, r++, ScalerInfo( "Level1-PS",     0, 58 ) );
  }

  gScaler.PrintFlags();

  return 0;
}

//____________________________________________________________________________
Int_t
process_end( void )
{
  if( gScaler.GetFlag( ScalerAnalyzer::kScalerSheet ) )
    return 0;

  gScaler.Print();

  return 0;

}

//____________________________________________________________________________
Int_t
process_event( void )
{
  static Int_t  event_count = 0;
  static Bool_t en_disp     = false;

  if( gScaler.GetFlag( ScalerAnalyzer::kScalerSheet ) && event_count==0 )
    hddaq::cout << "waiting spill end " << std::flush;

  ++event_count;

  if( !gScaler.Decode() )
    return 0;

  if( gScaler.GetFlag( ScalerAnalyzer::kSemiOnline ) ){
    if( event_count%300 == 0 ) en_disp = true;
  } else {
    if( event_count%10 == 0 ) en_disp = true;
  }

  if( gScaler.IsSpillEnd() )
    en_disp = true;

  if( gScaler.GetFlag( ScalerAnalyzer::kScalerSheet ) &&
      !gScaler.IsSpillEnd() ){
    if( event_count%100==0 )
      hddaq::cout << "." << std::flush;
    return 0;
  }

  if( en_disp ){
    gScaler.Print();
    en_disp = false;
  }

  if( gScaler.IsSpillEnd() &&
      gScaler.GetFlag( ScalerAnalyzer::kScalerSheet ) ){
    hddaq::cout << "found spill end "
		<< gScaler.Get("Spill") << "/" << nspill_scaler_sheet
		<< std::endl;

    if( gScaler.Get("Spill") == nspill_scaler_sheet ){
      gScaler.PrintScalerSheet();
      return -1;
    }

    if( gScaler.Get("Spill") > nspill_scaler_sheet ){
      hddaq::cout << "something is wrong!" << std::endl;
      return -1;
    }

    hddaq::cout << "waiting spill end " << std::flush;
  }

  return 0;
}

}
