// -*- C++ -*-

#include <iomanip>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <THttpServer.h>
#include <TKey.h>
#include <TMath.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TTimeStamp.h>

#include <user_analyzer.hh>
#include <Unpacker.hh>
#include <UnpackerManager.hh>
#include <DAQNode.hh>
#include <filesystem_util.hh>

#include "BH2Filter.hh"
#include "BH2Hit.hh"
#include "ConfMan.hh"
#include "DetectorID.hh"
#include "DCAnalyzer.hh"
#include "DCAnalyzerOld.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCHit.hh"
#include "DCLocalTrack.hh"
#include "DCTdcCalibMan.hh"
#include "EMCParamMan.hh"
#include "EventAnalyzer.hh"
#include "FiberCluster.hh"
#include "FiberHit.hh"
#include "HistMaker.hh"
#include "HodoAnalyzer.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "HodoRawHit.hh"
#include "HttpServer.hh"
#include "MacroBuilder.hh"
#include "MatrixParamMan.hh"
#include "MsTParamMan.hh"
#include "PsMaker.hh"
#include "RawData.hh"
#include "UserParamMan.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    std::vector<TH1*> hptr_array;
    const UnpackerManager& gUnpacker  = GUnpacker::get_instance();
    //    BH2Filter&       gBH2Filter = BH2Filter::GetInstance();
          HistMaker&       gHist      = HistMaker::getInstance();
          HttpServer&      gHttp      = HttpServer::GetInstance();
    const UserParamMan&    gUser      = UserParamMan::GetInstance();
    const DCGeomMan&       gGeom      = DCGeomMan::GetInstance();
    TCanvas* c1 = nullptr;
    TGraph* g = nullptr;
  }

//____________________________________________________________________________
int
process_begin( const std::vector<std::string>& argv )
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  // gConfMan.InitializeParameter<BH2Filter>("BH2FLT");
  gConfMan.InitializeParameter<DCGeomMan>("DCGEOM");
  gConfMan.InitializeParameter<DCTdcCalibMan>("TDCCALIB");
  gConfMan.InitializeParameter<DCDriftParamMan>("DRFTPM");
  gConfMan.InitializeParameter<HodoParamMan>("HDPRM");
  gConfMan.InitializeParameter<HodoPHCMan>("HDPHC");
  gConfMan.InitializeParameter<UserParamMan>("USER");
  if( !gConfMan.IsGood() ) return -1;
  // unpacker and all the parameter managers are initialized at this stage

  // gBH2Filter.Print();

  gHttp.SetPort(9090);
  gHttp.Open();
  //BcOut
  gHttp.Register(gHist.createBH2());
  gHttp.Register(gHist.createBH2_E42());
  gHttp.Register(gHist.createBeamProfile());

  if(0 != gHist.setHistPtr(hptr_array)){ return -1; }

  // Macro for HttpServer
  for( ParticleType ip : { kKaon, kPion, kAll } ){
    gHttp.Register(http::BeamProfileX(ip));
    gHttp.Register(http::BeamProfileY(ip));
    gHttp.Register(http::BeamProfileXY(ip));
  }
  gHttp.Register(http::BeamProfileFF());
  for( Int_t i=0, n=hptr_array.size(); i<n; ++i ){
    TString name = hptr_array[i]->GetName();
    if( name.Contains("Kaon") )
      hptr_array[i]->SetLineColor( kGreen + 2 );
    else if( name.Contains("Pion") )
      hptr_array[i]->SetLineColor( kBlue + 2 );
    else
      hptr_array[i]->SetLineColor( kBlack );
    hptr_array[i]->SetLineWidth(2);
    hptr_array[i]->SetDirectory(0);
  }

  // Graph
  c1 = new TCanvas("BeamEnvelope", "BeamEnvelope");
  c1->Divide( 2, 2 );
  g = new TGraphErrors;
  g->SetNameTitle( "MeanX", "MeanX" );
  c1->cd(1); g->Draw( "AP" );
  gHttp.Register( c1 );

  gStyle->SetOptStat(1110);
  gStyle->SetTitleW(.4);
  gStyle->SetTitleH(.1);
  gStyle->SetStatW(.42);
  gStyle->SetStatH(.3);

  return 0;
}

//____________________________________________________________________________
int
process_end( void )
{
  hptr_array.clear();
  return 0;
}

//____________________________________________________________________________
int
process_event( void )
{
  static const Double_t MinTotBC3 = gUser.GetParameter("BC3_TOT");
  static const Double_t MinTotBC4 = gUser.GetParameter("BC4_TOT");

  EventAnalyzer event;
  event.DecodeRawData();
  event.DecodeDCAnalyzer();
  event.DecodeHodoAnalyzer();

  auto HodoAna = event.GetHodoAnalyzer();
  auto DCAna = event.GetDCAnalyzer();

  static Int_t run_number = -1;
  if( run_number != gUnpacker.get_root()->get_run_number() ){
    for( Int_t i=0, n=hptr_array.size(); i<n; ++i ){
      hptr_array[i]->Reset();
    }
    run_number = gUnpacker.get_root()->get_run_number();
  }

  // Trigger Flag
  Bool_t trig_flag[NParticleType];
  trig_flag[kKaon] = false;
  trig_flag[kPion] = false;
  trig_flag[kAll] = true;
  {
    static const Int_t k_device = gUnpacker.get_device_id("TFlag");
    static const Int_t k_tdc    = gUnpacker.get_data_id("TFlag", "tdc");
    Int_t nhit_k = gUnpacker.get_entries( k_device, 0, trigger::kBeamTOF, 0, k_tdc );
    Int_t nhit_pi = gUnpacker.get_entries( k_device, 0, trigger::kBeamPi, 0, k_tdc );
    for( Int_t m = 0; m<nhit_k; ++m ){
      UInt_t tdc = gUnpacker.get( k_device, 0, trigger::kBeamTOF, 0, k_tdc, m );
      //if(tflag_tdc_min < tdc && tdc < tflag_tdc_max) pipi_flag = true;
      if(tdc!=0) trig_flag[kKaon] = true;//K- beam trigger
    }
    for( Int_t m = 0; m<nhit_pi; ++m ){
      UInt_t tdc = gUnpacker.get( k_device, 0, trigger::kBeamPi, 0, k_tdc, m );
      //if(tflag_tdc_min < tdc && tdc < tflag_tdc_max) pipi_flag = true;
      if(tdc!=0) trig_flag[kPion] = true;//pi- beam trigger
    }
  }

  Int_t nhBh2 = HodoAna->GetNHitsBH2();
  Int_t segBh2 = -1.;
  Double_t bh2mt = -9999.;
  for( Int_t i=0; i<nhBh2; ++i ){
    const BH2Hit* const hit = HodoAna->GetHitBH2(i);
    Int_t seg   = hit->SegmentId();
    Double_t mt = hit->MeanTime();
    if( TMath::Abs( mt ) < TMath::Abs( bh2mt ) ){
      bh2mt = mt;
      segBh2 = seg;
    }
    // if( nhBh2 == 1 ) segBh2 = seg;
    static const Int_t hit_id = gHist.getSequentialID(kBH2, 0, kHitPat);
    hptr_array[hit_id]->Fill( seg );
  }

  // DCAna->TotCutBC3( MinTotBC3 );
  // DCAna->TotCutBC4( MinTotBC4 );
  // DCAna->TotCutBC3( 10 );
  // DCAna->TotCutBC4( 10 );

  Double_t bcout_nhits = 0;
  {
    // const std::vector<Double_t>& xmin = gBH2Filter.GetXmin( segBh2 );
    // const std::vector<Double_t>& xmax = gBH2Filter.GetXmax( segBh2 );
    for( Int_t l=0; l<NumOfLayersBcOut; ++l ){
      const DCHitContainer& cont = DCAna->GetBcOutHC(l+1);
      Int_t nhOut = cont.size();
      bcout_nhits += (Double_t)nhOut;
#if 0
      for( Int_t i=0; i<nhOut; ++i ){
	const DCHit* const hit = cont.at(i);
	Double_t pos   = hit->GetWirePosition();
	const Int_t bh2raw_id = gHist.getSequentialID(kMisc, ip, kHitPat2D, 1);
	// static const Int_t bh2cut_id = gHist.getSequentialID(kMisc, 1, kHitPat2D);
	// hptr_array[bh2raw_id+l]->Fill( pos, segBh2 );
	// if( xmin.at(l) < pos && pos < xmax.at(l) ){
	//   hptr_array[bh2cut_id+l]->Fill( pos, segBh2 );
	// }
      }
#endif
    }
  }
  if( (bcout_nhits/NumOfLayersBcOut) > 10 ){
    cout << "#D too much nhits: " << bcout_nhits << std::endl;
    return 0;
  }

  // event.ApplyBH2Filter();

  DCAna->TrackSearchBcOut();

  for( ParticleType ip : { kKaon, kPion, kAll } ){
    if( !trig_flag[ip] )
      continue;

    // BcOutTracking
    const Int_t nt_id = gHist.getSequentialID( kMisc, ip, kMulti, 1 );
    const Int_t chisqr_id = gHist.getSequentialID( kMisc, ip, kChisqr, 1 );
    const Int_t bh2hit_id = gHist.getSequentialID( kMisc, ip, kHitPat2D, 1 );
    const Int_t xpos_id = gHist.getSequentialID(kMisc, ip, kHitPat);
    const Int_t ypos_id = gHist.getSequentialID(kMisc, ip, kHitPat,
						NProfile*2+1);
    const Int_t xypos_id = gHist.getSequentialID(kMisc, ip, kHitPat,
						 NProfile*4+1);
    const Int_t uxpos_id = gHist.getSequentialID(kMisc, ip, kHitPat,
						 NProfile*5+1);
    const Int_t vypos_id = gHist.getSequentialID(kMisc, ip, kHitPat,
						 NProfile*5+2);
    const Double_t zBH2 = gGeom.GetLocalZ("BH2");
    Int_t ntBcOut = DCAna->GetNtracksBcOut();
    Int_t cntBcOut = 0;
    for( Int_t i=0; i<ntBcOut; ++i ){
      const DCLocalTrack* const track = DCAna->GetTrackBcOut(i);
      const Double_t chisqr = track->GetChiSquare();
      hptr_array[chisqr_id]->Fill( chisqr );
      if( !track || chisqr>10. ) continue;
      cntBcOut++;
      hptr_array[bh2hit_id]->Fill( track->GetX(zBH2), segBh2 );
      for( Int_t j=0; j<NProfile; ++j ){
	const Double_t z = FFfromVO + Profiles[j];
	const Double_t x = track->GetX(z);
	const Double_t y = track->GetY(z);
	hptr_array[xpos_id+j]->Fill(x);
	hptr_array[ypos_id+j]->Fill(y);
	hptr_array[xpos_id+j+NProfile]->Fill(x);
	hptr_array[ypos_id+j+NProfile]->Fill(y);
	hptr_array[xypos_id+j]->Fill(x, y);
      }
      hptr_array[uxpos_id]->Fill( track->GetX0(), track->GetU0() );
      hptr_array[vypos_id]->Fill( track->GetY0(), track->GetV0() );
    }
    // hptr_array[nt_id]->Fill( ntBcOut );
    hptr_array[nt_id]->Fill( cntBcOut );

    // if( gUnpacker.get_counter()%100 == 0 ){
    http::UpdateBeamProfile(ip);
  }
  // }

  g->Set(0);
  for( Int_t i=0; i<NProfile; ++i ){
    g->SetPoint( i, Profiles[i], i );
  }
  c1->Update();

  return 0;
}

}
