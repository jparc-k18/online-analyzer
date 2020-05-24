// -*- C++ -*-

// Author: Tomonori Takahashi

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
  using namespace hddaq;

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
  gScaler.SetFlag( ScalerAnalyzer::kScalerE42 );
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
  }

  //////////////////// Set Channels
  // ScalerAnalylzer::Set( Int_t column,
  //                       Int_t raw,
  //                       ScalerInfo( name, module, channel ) );
  // scaler information is defined from here.
  // please do not use a white space character.

  {
    Int_t c = 0;
    Int_t r = 0;
    gScaler.Set( c, r++, ScalerInfo( "10M-Clock",     0, 50 ) );
    gScaler.Set( c, r++, ScalerInfo( "Spill",         0, 49 ) );
    gScaler.Set( c, r++, ScalerInfo( "WC-U",          1, 32 ) );
    gScaler.Set( c, r++, ScalerInfo( "WC-D",          1, 33 ) );
    gScaler.Set( c, r++, ScalerInfo( "WC-SUM",        1, 34 ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-01",        2, 32  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-02",        2, 33  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-03",        2, 34  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-04",        2, 35  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-05",        2, 36  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-06",        2, 37  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-07",        2, 38  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-08",        2, 39  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-09",        2, 40  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-10",        2, 41  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-11",        2, 42  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-12",        2, 43  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-13",        2, 44  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-14",        2, 45  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-15",        2, 46  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2",           2, 47  ) );
    gScaler.Set( c, r++, ScalerInfo( "BH2-SUM",      -1, -1 ) );
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
    std::cout << "waiting spill end " << std::flush;

  ++event_count;

  gScaler.Decode();

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
      std::cout << "." << std::flush;
    return 0;
  }

  if( en_disp ){
    gScaler.Print();
    en_disp = false;
  }

  if( gScaler.IsSpillEnd() &&
      gScaler.GetFlag( ScalerAnalyzer::kScalerSheet ) ){
    std::cout << "found spill end "
    	      << gScaler.Get("Spill") << "/" << nspill_scaler_sheet
	      << std::endl;

    if( gScaler.Get("Spill") == nspill_scaler_sheet ){
      gScaler.PrintScalerSheet();
      return -1;
    }

    if( gScaler.Get("Spill") > nspill_scaler_sheet ){
      std::cout << "something is wrong!" << std::endl;
      return -1;
    }

    std::cout << "waiting spill end " << std::flush;
  }

  return 0;
}

}
