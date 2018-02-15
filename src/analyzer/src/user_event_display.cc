// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TString.h>

#include <Unpacker.hh>
#include <UnpackerManager.hh>

#include "user_analyzer.hh"

#include "BH2Filter.hh"
#include "BH2Hit.hh"
#include "ConfMan.hh"
#include "DCAnalyzer.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCHit.hh"
#include "DCLocalTrack.hh"
#include "DCTdcCalibMan.hh"
#include "DetectorID.hh"
#include "EventAnalyzer.hh"
#include "EventDisplay.hh"
#include "HistMaker.hh"
#include "HodoAnalyzer.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "MacroBuilder.hh"
#include "RawData.hh"
#include "UserParamMan.hh"

#define BH2FILTER 1

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    const UnpackerManager& gUnpacker  = GUnpacker::get_instance();
    const HistMaker&       gHist      = HistMaker::getInstance();
    const DCGeomMan&       gGeom      = DCGeomMan::GetInstance();
          BH2Filter&       gBH2Filter = BH2Filter::GetInstance();
          EventDisplay&    gEvDisp    = EventDisplay::GetInstance();

    std::vector<TH1*> hptr_array;

  }

//____________________________________________________________________________
Int_t
process_begin( const std::vector<std::string>& argv )
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  gConfMan.InitializeParameter<BH2Filter>("BH2FLT");
  gConfMan.InitializeParameter<DCGeomMan>("DCGEOM");
  gConfMan.InitializeParameter<DCTdcCalibMan>("TDCCALIB");
  gConfMan.InitializeParameter<DCDriftParamMan>("DRFTPM");
  gConfMan.InitializeParameter<HodoParamMan>("HDPRM");
  gConfMan.InitializeParameter<HodoPHCMan>("HDPHC");
  gConfMan.InitializeParameter<UserParamMan>("USER");
  gConfMan.InitializeParameter<EventDisplay>();
  if( !gConfMan.IsGood() ) return -1;

  gBH2Filter.Print();
  // gBH2Filter.SetVerbose();

  gEvDisp.SetStyle();

  return 0;
}

//____________________________________________________________________________
Int_t
process_end( void )
{
  return 0;
}

//____________________________________________________________________________
Int_t
process_event( void )
{
  EventAnalyzer event;
  event.DecodeRawData();
  event.DecodeDCAnalyzer();
  event.DecodeHodoAnalyzer();

  const HodoAnalyzer* const hodoAna = event.GetHodoAnalyzer();
  const DCAnalyzer* const dcAna = event.GetDCAnalyzer();

  gEvDisp.DrawText();

  // gEvDisp.DrawTrigger( std::vector<Int_t>() );

  // Int_t nhBh2 = hodoAna->GetNHitsBH2();
  // Int_t segBh2 = -1.;
  // for( Int_t i=0; i<nhBh2; ++i ){
  //   const BH2Hit* const hit = hodoAna->GetHitBH2(i);
  //   Int_t seg = hit->SegmentId();
  //   if( nhBh2 == 1 ) segBh2 = seg;
  //   static const Int_t hit_id = gHist.getSequentialID(kBH2, 0, kHitPat);
  //   hptr_array[hit_id]->Fill( seg );
  // }

  // BH2 % BcOut Hit
  // if( segBh2 >= 0 ){
  //   const std::vector<Double_t>& xmin = gBH2Filter.GetXmin( segBh2 );
  //   const std::vector<Double_t>& xmax = gBH2Filter.GetXmax( segBh2 );
  //   for( Int_t l=0; l<NumOfLayersBcOut; ++l ){
  //     const DCHitContainer& cont = dcAna->GetBcOutHC(l+1);
  //     Int_t nhOut = cont.size();
  //     for( Int_t i=0; i<nhOut; ++i ){
  // 	const DCHit* const hit = cont.at(i);
  // 	Double_t pos   = hit->GetWirePosition();
  // 	static const Int_t bh2raw_id = gHist.getSequentialID(kMisc, 0, kHitPat2D);
  // 	static const Int_t bh2cut_id = gHist.getSequentialID(kMisc, 1, kHitPat2D);
  // 	hptr_array[bh2raw_id+l]->Fill( pos, segBh2 );
  // 	if( xmin.at(l) < pos && pos < xmax.at(l) ){
  // 	  hptr_array[bh2cut_id+l]->Fill( pos, segBh2 );
  // 	}
  //     }
  //   }
  // }

  // BcOutTracking
// #if !BH2FILTER
//   event.TrackSearchBcOut();
//   {
//     Int_t ntBcOut = dcAna->GetNtracksBcOut();
//     hptr_array[gHist.getSequentialID(kMisc, 0, kMulti, 1)]->Fill( ntBcOut );
//     for( Int_t i=0; i<ntBcOut; ++i ){
//       const DCLocalTrack* const track = dcAna->GetTrackBcOut(i);
//       static const Int_t chisqr_id = gHist.getSequentialID(kMisc, 0, kChisqr, 1);
//       Double_t chisqr = track->GetChiSquare();
//       hptr_array[chisqr_id]->Fill( chisqr );
//       if( !track || chisqr>10. ) continue;
//       static const Int_t bh2raw_id = gHist.getSequentialID(kMisc, 2, kHitPat2D, 1);
//       static const Double_t zBH2 = gGeom.GetLocalZ("BH2");
//       hptr_array[bh2raw_id]->Fill( track->GetX(zBH2), segBh2 );
//     }
//   }
// #endif

//   event.ApplyBH2Filter();
//   event.TrackSearchBcOut();
//   {
//     Int_t ntBcOut = dcAna->GetNtracksBcOut();
//     hptr_array[gHist.getSequentialID(kMisc, 0, kMulti, 2)]->Fill( ntBcOut );
//     for( Int_t i=0; i<ntBcOut; ++i ){
//       const DCLocalTrack* const track = dcAna->GetTrackBcOut(i);
//       static const Int_t chisqr_id = gHist.getSequentialID(kMisc, 0, kChisqr, 2);
//       Double_t chisqr = track->GetChiSquare();
//       hptr_array[chisqr_id]->Fill( chisqr );
//       if( !track || chisqr>10. ) continue;
//       // Double_t x0 = track->GetX0(); Double_t y0 = track->GetY0();
//       // Double_t u0 = track->GetU0(); Double_t v0 = track->GetV0();
//       for( Int_t j=0; j<NHist; ++j ){
// 	Double_t z = dist_FF+FF_plus+TgtOrg_plus[j];
// 	Double_t x = track->GetX(z); Double_t y = track->GetY(z);
// 	static const Int_t xpos_id   = gHist.getSequentialID(kMisc, 0, kHitPat);
// 	static const Int_t ypos_id   = gHist.getSequentialID(kMisc, 0, kHitPat, NHist+1);
// 	static const Int_t xypos_id  = gHist.getSequentialID(kMisc, 0, kHitPat, NHist*2+1);
// 	hptr_array[xpos_id+j]->Fill(x);
// 	hptr_array[ypos_id+j]->Fill(y);
// 	hptr_array[xypos_id+j]->Fill(x, y);
//       }
//       static const Int_t bh2cut_id = gHist.getSequentialID(kMisc, 2, kHitPat2D, 2);
//       static const Double_t zBH2 = gGeom.GetLocalZ("BH2");
//       hptr_array[bh2cut_id]->Fill( track->GetX(zBH2), segBh2 );
//     }
//   }

// #if DEBUG
//   std::cout << __FILE__ << " " << __LINE__ << std::endl;
// #endif

  gEvDisp.EndOfEvent();

  return 0;
}

}
