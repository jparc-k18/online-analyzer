// -*- C++ -*-

// Author: Tomonori Takahashi

#include <iomanip>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TGFileBrowser.h>
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

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "DCAnalyzer.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "EMCParamMan.hh"
#include "EventAnalyzer.hh"
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

#define DEBUG    0
#define FLAG_DAQ 1

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    std::vector<TH1*> hptr_array;
    const UnpackerManager& gUnpacker = GUnpacker::get_instance();
    HistMaker&          gHist   = HistMaker::getInstance();
    HttpServer&         gHttp   = HttpServer::GetInstance();
    MatrixParamMan&     gMatrix = MatrixParamMan::GetInstance();
    MsTParamMan&        gMsT    = MsTParamMan::GetInstance();
    const UserParamMan& gUser   = UserParamMan::GetInstance();
  }

//____________________________________________________________________________
int
process_begin( const std::vector<std::string>& argv )
{
  // gROOT->SetBatch(kTRUE);
  gStyle->SetOptStat(1110);
  gStyle->SetTitleW(.4);
  gStyle->SetTitleH(.1);
  // gStyle->SetStatW(.42);
  // gStyle->SetStatH(.35);
  gStyle->SetStatW(.32);
  gStyle->SetStatH(.25);
  gStyle->SetPalette(55);

  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  gConfMan.InitializeParameter<HodoParamMan>("HDPRM");
  gConfMan.InitializeParameter<DCGeomMan>("DCGEOM");
  gConfMan.InitializeParameter<DCTdcCalibMan>("TDCCALIB");
  gConfMan.InitializeParameter<DCDriftParamMan>("DRFTPM");
  gConfMan.InitializeParameter<HodoParamMan>("HDPRM");
  gConfMan.InitializeParameter<HodoPHCMan>("HDPHC");
  gConfMan.InitializeParameter<UserParamMan>("USER");
  if( !gConfMan.IsGood() ) return -1;
  // unpacker and all the parameter managers are initialized at this stage

  gHttp.SetPort(9090);
  gHttp.Open();
  gHttp.Register(gHist.createBH1());
  gHttp.Register(gHist.createBFT());
  gHttp.Register(gHist.createBC3());
  gHttp.Register(gHist.createBC4());
  gHttp.Register(gHist.createBH2());
  gHttp.Register(gHist.createSFT());
  gHttp.Register(gHist.createSAC());
  gHttp.Register(gHist.createSDC1());
  gHttp.Register(gHist.createSCH());
  gHttp.Register(gHist.createSDC2());
  gHttp.Register(gHist.createSDC3());
  gHttp.Register(gHist.createTOF());
  gHttp.Register(gHist.createTOF_HT());
  gHttp.Register(gHist.createLC());
  // gHttp.Register(gHist.createMsT());
  gHttp.Register(gHist.createTriggerFlag());
  gHttp.Register(gHist.createCorrelation());
  gHttp.Register(gHist.createDAQ(false));
  gHttp.Register(gHist.createDCEff());
  {
    int btof_id = gHist.getUniqueID(kMisc, 0, kTDC);
    gHttp.Register( gHist.createTH1(btof_id, "BTOF",
				    300, -10, 5,
				    "BTOF [ns]", "") );
  }

  if(0 != gHist.setHistPtr(hptr_array)){ return -1; }

  // Macro for HttpServer
  gHttp.Register(http::BH1ADC());
  gHttp.Register(http::BH1TDC());
  gHttp.Register(http::BFT());
  gHttp.Register(http::BH2ADC());
  gHttp.Register(http::BH2TDC());
  gHttp.Register(http::SFT());
  gHttp.Register(http::SCHTDC());
  gHttp.Register(http::SCHTOT());
  gHttp.Register(http::SCHHitMulti());
  gHttp.Register(http::TOFADC());
  gHttp.Register(http::TOFTDC());
  // gHttp.Register(http::MsTTDC());
  gHttp.Register(http::BC3TDC());
  gHttp.Register(http::BC3HitMulti());
  gHttp.Register(http::BC4TDC());
  gHttp.Register(http::BC4HitMulti());
  gHttp.Register(http::SDC1TDC());
  gHttp.Register(http::SDC1HitMulti());
  gHttp.Register(http::SDC2TDC());
  gHttp.Register(http::SDC2HitMulti());
  gHttp.Register(http::SDC3TDC());
  gHttp.Register(http::SDC3HitMulti());
  gHttp.Register(http::BcOutEfficiency());
  gHttp.Register(http::SdcInOutEfficiency());
  gHttp.Register(http::HitPattern());

  for( Int_t i=0, n=hptr_array.size(); i<n; ++i ){
    hptr_array[i]->SetDirectory(0);
  }

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
  // static const Double_t MinTdcBH1 = gUser.GetParameter("BH1_TDC", 0);
  // static const Double_t MaxTdcBH1 = gUser.GetParameter("BH1_TDC", 1);

  static int run_number = -1;
  if( run_number != gUnpacker.get_root()->get_run_number() ){
    for( Int_t i=0, n=hptr_array.size(); i<n; ++i ){
      hptr_array[i]->Reset();
    }
    run_number = gUnpacker.get_root()->get_run_number();
  }

  EventAnalyzer event;
  event.DecodeRawData();
  event.DecodeHodoAnalyzer();
  event.DecodeDCAnalyzer();

  const RawData*      const rawData = event.GetRawData();
  const HodoAnalyzer* const hodoAna = event.GetHodoAnalyzer();
  // const DCAnalyzer*   const DCAna   = event.GetDCAnalyzer();

  // TriggerFlag
  std::vector<Int_t> trigflag(NumOfSegTFlag);
  {
    static const Int_t k_tdc_id = gHist.getSequentialID(kTriggerFlag, 0, kTDC);
    static const Int_t k_hit_id = gHist.getSequentialID(kTriggerFlag, 0, kHitPat);
    const HodoRHitContainer& cont = rawData->GetTrigRawHC();
    Int_t nh = cont.size();
    for( Int_t i=0; i<nh; ++i ){
      const HodoRawHit* const hit = cont[i];
      const Int_t seg = hit->SegmentId()+1;
      const Int_t tdc = hit->GetTdc1();
      hptr_array[k_tdc_id+seg]->Fill( tdc );
      hptr_array[k_hit_id]->Fill( seg );
      trigflag[seg-1] = tdc;
    }
    if( trigflag[SpillEndFlag]>0 ) return 0;
  }

  // BH1
  {
    static const Int_t bh1a_id   = gHist.getSequentialID(kBH1, 0, kADC);
    static const Int_t bh1t_id   = gHist.getSequentialID(kBH1, 0, kTDC);
    static const Int_t bh1awt_id = gHist.getSequentialID(kBH1, 0, kADCwTDC);
    static const Int_t bh1hit_id = gHist.getSequentialID(kBH1, 0, kHitPat);
    static const Int_t bh1mul_id = gHist.getSequentialID(kBH1, 0, kMulti);

    const HodoRHitContainer& cont = rawData->GetBH1RawHC();
    Int_t multiplicity = 0;
    for( Int_t i=0, n=cont.size(); i<n; ++i ){
      HodoRawHit *hit = cont[i];
      if( !hit ) continue;
      Int_t seg = hit->SegmentId();
      Int_t Au=hit->GetAdcUp(), Ad=hit->GetAdcDown();
      Int_t Tu=hit->GetTdcUp(), Td=hit->GetTdcDown();
      hptr_array[bh1a_id +seg]->Fill(Au);
      hptr_array[bh1a_id +seg +NumOfSegBH1]->Fill(Ad);
      if( Tu > 0 ){
	hptr_array[bh1t_id +seg]->Fill(Tu);
	hptr_array[bh1awt_id + seg]->Fill(Au);
      }
      if( Td > 0 ){
	hptr_array[bh1t_id +seg +NumOfSegBH1]->Fill(Td);
	hptr_array[bh1awt_id +seg +NumOfSegBH1]->Fill(Ad);
      }
      if( Tu > 0 && Td > 0 ){
	hptr_array[bh1hit_id]->Fill(seg);
	++multiplicity;
      }
    }
    hptr_array[bh1mul_id]->Fill(multiplicity);
  }

  // BFT
  {
    // TDC gate range
    static const int tdc_min = gUser.GetParameter("BFT_TDC", 0);
    static const int tdc_max = gUser.GetParameter("BFT_TDC", 1);

    // sequential id
    static const int bft_t_id    = gHist.getSequentialID(kBFT, 0, kTDC,      1);
    static const int bft_ct_id   = gHist.getSequentialID(kBFT, 0, kTDC,     11);
    static const int bft_tot_id  = gHist.getSequentialID(kBFT, 0, kADC,      1);
    static const int bft_ctot_id = gHist.getSequentialID(kBFT, 0, kADC,     11);
    static const int bft_hit_id  = gHist.getSequentialID(kBFT, 0, kHitPat,   1);
    static const int bft_chit_id = gHist.getSequentialID(kBFT, 0, kHitPat,  11);
    static const int bft_mul_id   = gHist.getSequentialID(kBFT, 0, kMulti,    1);
    static const int bft_cmul_id  = gHist.getSequentialID(kBFT, 0, kMulti,   11);

    static const int bft_ct_2d_id = gHist.getSequentialID(kBFT, 0, kTDC2D,   1);
    static const int bft_ctot_2d_id = gHist.getSequentialID(kBFT, 0, kADC2D, 1);

    int multiplicity  = 0; // includes both u and d planes.
    int cmultiplicity = 0; // includes both u and d planes.

    Int_t nh = hodoAna->GetNHitsBFT();
    for( Int_t i=0; i<nh; ++i ){
      const FiberHit* const hit = hodoAna->GetHitBFT(i);
      if( !hit ) continue;
      Int_t plane = hit->PlaneId();
      Int_t mhit  = hit->GetNumOfHit();
      Int_t seg   = hit->SegmentId();
      Int_t prev = 0;
      for( Int_t m=0; m<mhit; ++m ){
	Int_t leading  = hit->GetLeading(m);
	Int_t tot      = hit->GetWidth(m);
	hptr_array[bft_t_id +plane]->Fill( leading );
	hptr_array[bft_tot_id +plane]->Fill(tot);
	if( tdc_min < leading && leading < tdc_max ){
	  ++multiplicity;
	  hptr_array[bft_hit_id +plane]->Fill(seg);
	}
	if( prev == leading ) continue;
	prev = leading;
	if( tot==0 ) continue;
	hptr_array[bft_ct_id +plane]->Fill(leading);
	hptr_array[bft_ctot_id +plane]->Fill(tot);
	hptr_array[bft_ct_2d_id +plane]->Fill(seg, leading);
	hptr_array[bft_ctot_2d_id +plane]->Fill(seg, tot);
	if( tdc_min < leading && leading < tdc_max ){
	  ++cmultiplicity;
	  hptr_array[bft_chit_id +plane]->Fill(seg);
	}
      }
    }
    hptr_array[bft_mul_id]->Fill(multiplicity);
    hptr_array[bft_cmul_id]->Fill(cmultiplicity);
  }

  // BC3 -------------------------------------------------------------
  {
    // data type
    static const int k_device  = gUnpacker.get_device_id("BC3");
    static const int k_tdc     = 0;

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("BC3_TDC", 0);
    static const int tdc_max = gUser.GetParameter("BC3_TDC", 1);

    // sequential id
    static const int bc3t_id    = gHist.getSequentialID(kBC3, 0, kTDC);
    static const int bc3t1st_id = gHist.getSequentialID(kBC3, 0, kTDC2D);
    static const int bc3hit_id  = gHist.getSequentialID(kBC3, 0, kHitPat);
    static const int bc3mul_id  = gHist.getSequentialID(kBC3, 0, kMulti);
    static const int bc3mulwt_id
      = gHist.getSequentialID(kBC3, 0, kMulti, 1+NumOfLayersBC3);

    // TDC & HitPat & Multi
    for(int l = 0; l<NumOfLayersBC3; ++l){

      int multiplicity    = 0;
      int multiplicity_wt = 0;
      for(int w = 0; w<NumOfWireBC3; ++w){
	int nhit = gUnpacker.get_entries(k_device, l, 0, w, k_tdc);
	if(nhit == 0){continue;}

	// This wire fired at least one times.
	++multiplicity;
	bool flag_hit_wt = false;
	int  tdc1st = 0;
	for(int m = 0; m<nhit; ++m){
	  int tdc = gUnpacker.get(k_device, l, 0, w, k_tdc, m);
	  hptr_array[bc3t_id + l]->Fill(tdc);
	  if( tdc1st<tdc ) tdc1st = tdc;
	  // Drift time check
	  if(tdc_min < tdc && tdc < tdc_max){
	    hptr_array[bc3hit_id + l]->Fill(w);
	    flag_hit_wt = true;
	  }
	}

	if( tdc1st!=0 ) hptr_array[bc3t1st_id + l]->Fill(tdc1st);
	if(flag_hit_wt){ ++multiplicity_wt; }
      }

      hptr_array[bc3mul_id + l]->Fill(multiplicity);
      hptr_array[bc3mulwt_id + l]->Fill(multiplicity_wt);
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // BC4 -------------------------------------------------------------
  {
    // data type
    static const int k_device  = gUnpacker.get_device_id("BC4");
    static const int k_tdc = 0;

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("BC4_TDC", 0);
    static const int tdc_max = gUser.GetParameter("BC4_TDC", 1);

    // sequential id
    static const int bc4t_id    = gHist.getSequentialID(kBC4, 0, kTDC);
    static const int bc4t1st_id = gHist.getSequentialID(kBC4, 0, kTDC2D);
    static const int bc4hit_id  = gHist.getSequentialID(kBC4, 0, kHitPat);
    static const int bc4mul_id  = gHist.getSequentialID(kBC4, 0, kMulti);
    static const int bc4mulwt_id
      = gHist.getSequentialID(kBC4, 0, kMulti, 1+NumOfLayersBC4);

    // TDC & HitPat & Multi
    for(int l = 0; l<NumOfLayersBC4; ++l){

      int multiplicity    = 0;
      int multiplicity_wt = 0;
      for(int w = 0; w<NumOfWireBC4; ++w){
	int nhit = gUnpacker.get_entries(k_device, l, 0, w, k_tdc);
	if(nhit == 0){continue;}

	// This wire fired at least one times.
	++multiplicity;

	bool flag_hit_wt = false;
	int  tdc1st = 0;
	for(int m = 0; m<nhit; ++m){
	  int tdc = gUnpacker.get(k_device, l, 0, w, k_tdc, m);
	  hptr_array[bc4t_id + l]->Fill(tdc);
	  if( tdc1st<tdc ) tdc1st = tdc;
	  // Drift time check
	  if(tdc_min < tdc && tdc < tdc_max){
	    hptr_array[bc4hit_id + l]->Fill(w);
	    flag_hit_wt = true;
	  }
	}

	if( tdc1st!=0 ) hptr_array[bc4t1st_id + l]->Fill(tdc1st);
	if(flag_hit_wt){ ++multiplicity_wt; }
      }

      hptr_array[bc4mul_id + l]->Fill(multiplicity);
      hptr_array[bc4mulwt_id + l]->Fill(multiplicity_wt);
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // BH2 -----------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("BH2");
    static const int k_u      = 0; // up
    static const int k_d      = 1; // down
    static const int k_adc    = gUnpacker.get_data_id("BH2", "adc");
    static const int k_tdc    = gUnpacker.get_data_id("BH2", "tdc");

    // UP
    int bh2a_id   = gHist.getSequentialID(kBH2, 0, kADC);
    int bh2t_id   = gHist.getSequentialID(kBH2, 0, kTDC);
    int bh2awt_id = gHist.getSequentialID(kBH2, 0, kADCwTDC);
    for(int seg=0; seg<NumOfSegBH2; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc);
      if(nhit != 0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	hptr_array[bh2a_id + seg]->Fill(adc);
      }
      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      if( nhit!=0 ){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	if( tdc!=0 ){
	  hptr_array[bh2t_id + seg]->Fill(tdc);
	  // ADC w/TDC
	  if( gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	    hptr_array[bh2awt_id + seg]->Fill(adc);
	  }
	}
      }
    }

    // DOWN
    bh2a_id   = gHist.getSequentialID(kBH2, 0, kADC, NumOfSegBH2+1);
    bh2t_id   = gHist.getSequentialID(kBH2, 0, kTDC, NumOfSegBH2+1);
    bh2awt_id = gHist.getSequentialID(kBH2, 0, kADCwTDC, NumOfSegBH2+1);
    for(int seg=0; seg<NumOfSegBH2; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc);
      if(nhit != 0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
	hptr_array[bh2a_id + seg]->Fill(adc);
      }
      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      if( nhit!=0 ){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	if( tdc!=0 ){
	  hptr_array[bh2t_id + seg]->Fill(tdc);
	  // ADC w/TDC
	  if( gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
	    hptr_array[bh2awt_id + seg]->Fill( adc );
	  }
	}
      }
    }

    // Hit pattern &&  Multiplicity
    static const int bh2hit_id = gHist.getSequentialID(kBH2, 0, kHitPat);
    static const int bh2mul_id = gHist.getSequentialID(kBH2, 0, kMulti);
    int multiplicity = 0;
    for(int seg=0; seg<NumOfSegBH2; ++seg){
      int nhit_u = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      int nhit_d = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      // AND
      if( nhit_u!=0 && nhit_d!=0 ){
	unsigned int tdc_u = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	unsigned int tdc_d = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	// TDC AND
	if( tdc_u!=0 && tdc_d!=0 ){
	  hptr_array[bh2hit_id]->Fill( seg );
	  ++multiplicity;
	}
      }
    }

    hptr_array[bh2mul_id]->Fill(multiplicity);

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  //------------------------------------------------------------------
  // SFT
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device  = gUnpacker.get_device_id("SFT");
    static const int k_leading = gUnpacker.get_data_id("SFT", "leading");
    static const int k_trailing = gUnpacker.get_data_id("SFT", "trailing");

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SFT_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SFT_TDC", 1);

    // SequentialID
    int sft_t_id    = gHist.getSequentialID(kSFT, 0, kTDC,     1);
    int sft_ct_id   = gHist.getSequentialID(kSFT, 0, kTDC,    11);
    int sft_tot_id  = gHist.getSequentialID(kSFT, 0, kADC,     1);
    int sft_ctot_id = gHist.getSequentialID(kSFT, 0, kADC,    11);
    int sft_hit_id  = gHist.getSequentialID(kSFT, 0, kHitPat,  1);
    int sft_chit_id = gHist.getSequentialID(kSFT, 0, kHitPat, 11);
    int sft_mul_id   = gHist.getSequentialID(kSFT, 0, kMulti,   1);
    int sft_cmul_id  = gHist.getSequentialID(kSFT, 0, kMulti,  11);

    int sft_ct_2d_id = gHist.getSequentialID(kSFT, 0, kTDC2D,   1);
    int sft_ctot_2d_id = gHist.getSequentialID(kSFT, 0, kADC2D, 1);

    int multiplicity[4] ; // includes each layers.
    int cmultiplicity[4]; // includes each layers.

    for(int l=0; l<NumOfPlaneSFT; ++l){
      multiplicity[l]  = 0; // includes each layers.
      cmultiplicity[l] = 0; // includes each layers.
      int tdc_prev      = 0;
      for(int i = 0; i<NumOfSegSFT[l]; ++i){
        int nhit_l = gUnpacker.get_entries(k_device, l, 0, i, k_leading);
        int nhit_t = gUnpacker.get_entries(k_device, l, 0, i, k_trailing);
        int hit_l_max = 0;
        int hit_t_max = 0;
        if(nhit_l != 0){
          hit_l_max = gUnpacker.get(k_device, l, 0, i, k_leading,  nhit_l - 1);
        }
        if(nhit_t != 0){
          hit_t_max = gUnpacker.get(k_device, l, 0, i, k_trailing, nhit_t - 1);
        }

        for(int m = 0; m<nhit_l; ++m){
          int tdc = gUnpacker.get(k_device, l, 0, i, k_leading, m);
          hptr_array[sft_t_id+l]->Fill(tdc);
          if(tdc_min < tdc && tdc < tdc_max){
            ++multiplicity[l];
            hptr_array[sft_hit_id+l]->Fill(i);
          }
          if(tdc_prev==tdc) continue;
          tdc_prev = tdc;
	  hptr_array[sft_ct_id+l]->Fill(tdc);
	  hptr_array[sft_ct_2d_id+l]->Fill(i, tdc);
          if(tdc_min < tdc && tdc < tdc_max){
            ++cmultiplicity[l];
            hptr_array[sft_chit_id+l]->Fill(i);
          }
        }
        if(nhit_l == nhit_t && hit_l_max > hit_t_max){
          tdc_prev = 0;
          for(int m = 0; m<nhit_l; ++m){
            int tdc = gUnpacker.get(k_device, l, 0, i, k_leading, m);
            int tdc_t = gUnpacker.get(k_device, l, 0, i, k_trailing, m);
            int tot = tdc - tdc_t;
            hptr_array[sft_tot_id+l]->Fill(tot);
            if(tdc_prev==tdc) continue;
            tdc_prev = tdc;
            if(tot==0) continue;
            hptr_array[sft_ctot_id+l]->Fill(tot);
            hptr_array[sft_ctot_2d_id+l]->Fill(i, tot);
          }
        }
      }
    }
    // X & X'
    hptr_array[sft_mul_id]->Fill(multiplicity[0] + multiplicity[1]);
    hptr_array[sft_cmul_id]->Fill(cmultiplicity[0] + cmultiplicity[1]);
    // V
    hptr_array[sft_mul_id+1]->Fill(multiplicity[2]);
    hptr_array[sft_cmul_id+1]->Fill(cmultiplicity[2]);
    // U
    hptr_array[sft_mul_id+2]->Fill(multiplicity[3]);
    hptr_array[sft_cmul_id+2]->Fill(cmultiplicity[3]);
#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }//SFT

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // SCH -----------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("SCH");
    static const int k_leading  = gUnpacker.get_data_id("SCH", "leading");
    static const int k_trailing = gUnpacker.get_data_id("SCH", "trailing");

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SCH_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SCH_TDC", 1);

    // sequential id
    static const int sch_t_id    = gHist.getSequentialID(kSCH, 0, kTDC,      1);
    static const int sch_tot_id  = gHist.getSequentialID(kSCH, 0, kADC,      1);
    static const int sch_t_all_id   = gHist.getSequentialID(kSCH, 0, kTDC, NumOfSegSCH+1);
    static const int sch_tot_all_id = gHist.getSequentialID(kSCH, 0, kADC, NumOfSegSCH+1);
    static const int sch_hit_id  = gHist.getSequentialID(kSCH, 0, kHitPat,   1);
    static const int sch_mul_id  = gHist.getSequentialID(kSCH, 0, kMulti,    1);

    static const int sch_t_2d_id   = gHist.getSequentialID(kSCH, 0, kTDC2D,  1);
    static const int sch_tot_2d_id = gHist.getSequentialID(kSCH, 0, kADC2D,  1);

    int multiplicity  = 0;
    for( int i=0; i<NumOfSegSCH; ++i ){
      int nhit = gUnpacker.get_entries(k_device, 0, i, 0, k_leading);

      for(int m = 0; m<nhit; ++m){
	int tdc      = gUnpacker.get(k_device, 0, i, 0, k_leading,  m);
	int trailing = gUnpacker.get(k_device, 0, i, 0, k_trailing, m);
	int tot      = tdc - trailing;
	hptr_array[sch_t_id +i]->Fill(tdc);
	hptr_array[sch_t_all_id]->Fill(tdc);
	hptr_array[sch_tot_id +i]->Fill(tot);
	hptr_array[sch_tot_all_id]->Fill(tot);
	hptr_array[sch_t_2d_id]->Fill(i, tdc);
	hptr_array[sch_tot_2d_id]->Fill(i, tot);
	if( tdc_min<tdc && tdc<tdc_max ){
	  ++multiplicity;
	  hptr_array[sch_hit_id]->Fill(i);
	}
      }
    }
    hptr_array[sch_mul_id]->Fill(multiplicity);

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // SDC1 ------------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SDC1");
    static const int k_tdc    = 0;

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SDC1_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SDC1_TDC", 1);

    // sequential id
    static const int sdc1t_id    = gHist.getSequentialID(kSDC1, 0, kTDC, 1);
    static const int sdc1t1st_id = gHist.getSequentialID(kSDC1, 0, kTDC2D, 1);
    static const int sdc1hit_id  = gHist.getSequentialID(kSDC1, 0, kHitPat, 1);
    static const int sdc1mul_id  = gHist.getSequentialID(kSDC1, 0, kMulti, 1);
    static const int sdc1mulwt_id
      = gHist.getSequentialID(kSDC1, 0, kMulti, 1+NumOfLayersSDC1);

    // TDC & HitPat & Multi
    for(int l = 0; l<NumOfLayersSDC1; ++l){
      int multiplicity    = 0;
      int multiplicity_wt = 0;
      for(int w = 0; w<NumOfWireSDC1; ++w){
	int nhit = gUnpacker.get_entries(k_device, l, 0, w, k_tdc);
	if( nhit==0 ) continue;

	// This wire fired at least one times.
	++multiplicity;
	// hptr_array[sdc1hit_id + l]->Fill(w, nhit);

	bool flag_hit_wt = false;
	int  tdc1st = 0;
	for( int m=0; m<nhit; ++m ){
	  int tdc = gUnpacker.get(k_device, l, 0, w, k_tdc, m);
	  hptr_array[sdc1t_id + l]->Fill(tdc);
	  if( tdc1st<tdc ) tdc1st = tdc;

	  // Drift time check
	  if( tdc_min<tdc && tdc<tdc_max ){
	    flag_hit_wt = true;
	  }
	}

	if( tdc1st!=0 ) hptr_array[sdc1t1st_id + l]->Fill(tdc1st);
	if( flag_hit_wt ){
	  ++multiplicity_wt;
	  hptr_array[sdc1hit_id + l]->Fill( w );
	}
      }

      hptr_array[sdc1mul_id + l]->Fill(multiplicity);
      hptr_array[sdc1mulwt_id + l]->Fill(multiplicity_wt);
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device,0);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // SDC2 ------------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SDC2");
    static const int k_tdc    = 0;

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SDC2_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SDC2_TDC", 1);

    // sequential id
    static const int sdc2t_id    = gHist.getSequentialID(kSDC2, 0, kTDC);
    static const int sdc2t1st_id = gHist.getSequentialID(kSDC2, 0, kTDC2D);
    static const int sdc2hit_id  = gHist.getSequentialID(kSDC2, 0, kHitPat);
    static const int sdc2mul_id  = gHist.getSequentialID(kSDC2, 0, kMulti);
    static const int sdc2mulwt_id
      = gHist.getSequentialID(kSDC2, 0, kMulti, 1+NumOfLayersSDC2);

    // TDC & HitPat & Multi
    for(int l=0; l<NumOfLayersSDC2; ++l){
      int multiplicity    = 0;
      int multiplicity_wt = 0;
      for( int w=0; w<NumOfWireSDC2; ++w ){
	int nhit = gUnpacker.get_entries(k_device, l, 0, w, k_tdc);
	if( nhit == 0 ) continue;

	// This wire fired at least one times.
	++multiplicity;
	// hptr_array[sdc2hit_id + l]->Fill(w, nhit);

	bool flag_hit_wt = false;
	int  tdc1st = 0;
	for( int m = 0; m<nhit; ++m ){
	  int tdc = gUnpacker.get(k_device, l, 0, w, k_tdc, m);
	  hptr_array[sdc2t_id + l]->Fill(tdc);
	  if( tdc1st<tdc ) tdc1st = tdc;

	  // Drift time check
	  if( tdc_min < tdc && tdc < tdc_max ){
	    flag_hit_wt = true;
	  }
	}

	if( tdc1st!=0 ) hptr_array[sdc2t1st_id +l]->Fill( tdc1st );
	if( flag_hit_wt ){
	  ++multiplicity_wt;
	  hptr_array[sdc2hit_id + l]->Fill( w );
	}
      }

      hptr_array[sdc2mul_id + l]->Fill(multiplicity);
      hptr_array[sdc2mulwt_id + l]->Fill(multiplicity_wt);
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device,0);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // SDC3 ------------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SDC3");
    static const int k_tdc    = 0;

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SDC3_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SDC3_TDC", 1);

    // sequential id
    static const int sdc3t_id    = gHist.getSequentialID(kSDC3, 0, kTDC);
    static const int sdc3t1st_id = gHist.getSequentialID(kSDC3, 0, kTDC2D);
    static const int sdc3hit_id  = gHist.getSequentialID(kSDC3, 0, kHitPat);
    static const int sdc3mul_id  = gHist.getSequentialID(kSDC3, 0, kMulti);
    static const int sdc3mulwt_id
      = gHist.getSequentialID(kSDC3, 0, kMulti, 1+NumOfLayersSDC3);

    // TDC & HitPat & Multi
    for(int l=0; l<NumOfLayersSDC3; ++l){
      int multiplicity    = 0;
      int multiplicity_wt = 0;
      int sdc3_nwire = 0;
      if( l==0 || l==1 )
	sdc3_nwire = NumOfWireSDC3Y;
      if( l==2 || l==3 )
	sdc3_nwire = NumOfWireSDC3X;

      for( int w=0 ; w<sdc3_nwire; ++w ){
	int nhit = gUnpacker.get_entries(k_device, l, 0, w, k_tdc);
	if( nhit == 0 ) continue;

	// This wire fired at least one times.
	++multiplicity;
	// hptr_array[sdc3hit_id + l]->Fill(w, nhit);

	bool flag_hit_wt = false;
	int  tdc1st = 0;
	for( int m = 0; m<nhit; ++m ){
	  int tdc = gUnpacker.get(k_device, l, 0, w, k_tdc, m);
	  hptr_array[sdc3t_id + l]->Fill(tdc);
	  if( tdc1st<tdc ) tdc1st = tdc;

	  // Drift time check
	  if( tdc_min < tdc && tdc < tdc_max ){
	    flag_hit_wt = true;
	  }
	}

	if( tdc1st!=0 ) hptr_array[sdc3t1st_id +l]->Fill( tdc1st );
	if( flag_hit_wt ){
	  ++multiplicity_wt;
	  hptr_array[sdc3hit_id + l]->Fill( w );
	}
      }

      hptr_array[sdc3mul_id +l]->Fill( multiplicity );
      hptr_array[sdc3mulwt_id +l]->Fill( multiplicity_wt );
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // TOF -----------------------------------------------------------
  {
    // data typep
    static const int k_device = gUnpacker.get_device_id("TOF");
    static const int k_u      = 0; // up
    static const int k_d      = 1; // down
    static const int k_adc    = gUnpacker.get_data_id("TOF","adc");
    static const int k_tdc    = gUnpacker.get_data_id("TOF","tdc");

    // sequential id
    int tofa_id   = gHist.getSequentialID(kTOF, 0, kADC);
    int toft_id   = gHist.getSequentialID(kTOF, 0, kTDC);
    int tofawt_id = gHist.getSequentialID(kTOF, 0, kADCwTDC);
    for(int seg = 0; seg<NumOfSegTOF; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc);
      if( nhit!=0 ){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	hptr_array[tofa_id + seg]->Fill( adc );
      }
      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      if(nhit != 0){
	int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	if( tdc!=0 ){
	  hptr_array[toft_id + seg]->Fill(tdc);
	  // ADC w/TDC
	  if( gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	    hptr_array[tofawt_id + seg]->Fill( adc );
	  }
	}
      }
    }

    // Down PMT
    tofa_id   = gHist.getSequentialID(kTOF, 0, kADC, NumOfSegTOF+1);
    toft_id   = gHist.getSequentialID(kTOF, 0, kTDC, NumOfSegTOF+1);
    tofawt_id = gHist.getSequentialID(kTOF, 0, kADCwTDC, NumOfSegTOF+1);

    for(int seg = 0; seg<NumOfSegTOF; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc);
      if(nhit != 0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
	hptr_array[tofa_id + seg]->Fill(adc);
      }

      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      if(nhit != 0){
	int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	if(tdc != 0){
	  hptr_array[toft_id + seg]->Fill(tdc);
	  // ADC w/TDC
	  if( gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
	    hptr_array[tofawt_id + seg]->Fill( adc );
	  }
	}
      }
    }

    // Hit pattern && multiplicity
    static const int tofhit_id = gHist.getSequentialID(kTOF, 0, kHitPat);
    static const int tofmul_id = gHist.getSequentialID(kTOF, 0, kMulti);
    int multiplicity = 0;
    for(int seg=0; seg<NumOfSegTOF; ++seg){
      int nhit_tofu = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      int nhit_tofd = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      // AND
      if(nhit_tofu!=0 && nhit_tofd!=0){
	unsigned int tdc_u = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	unsigned int tdc_d = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	// TDC AND
	if(tdc_u!=0 && tdc_d!=0){
	  hptr_array[tofhit_id]->Fill(seg);
	  ++multiplicity;
	}
      }
    }

    hptr_array[tofmul_id]->Fill(multiplicity);

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // MsT -----------------------------------------------------------
  // {
  //   // data type
  //   static const int k_device = gUnpacker.get_device_id("MsT");
  //   // sequential id
  //   int tdc_id    = gHist.getSequentialID(kMsT, 0, kTDC);
  //   int tdc2d_id  = gHist.getSequentialID(kMsT, 0, kTDC2D);
  //   int tof_hp_id = gHist.getSequentialID(kMsT, 0, kHitPat, 0);
  //   int sch_hp_id = gHist.getSequentialID(kMsT, 0, kHitPat, 1);
  //   int flag_id   = gHist.getSequentialID(kMsT, 0, kHitPat, 2);
  //   int flag2d_id = gHist.getSequentialID(kMsT, 0, kHitPat2D, 0);

  //   // Flag
  //   int hul_flag  = -1;
  //   int soft_flag =  1;
  //   {
  //     int nhit = gUnpacker.get_entries(k_device, 2, 0, 0, 1);
  //     if( nhit>0 ){
  // 	int flag = gUnpacker.get(k_device, 2, 0, 0, 1);
  // 	if( std::bitset<3>(flag).count() == 1 ){
  // 	  hptr_array[flag_id]->Fill( flag );
  // 	  hul_flag = TMath::Log2(flag); // 1,2,4 -> 0,1,2
  // 	} else {
  // 	  std::cerr << "#W Invalid MsTFlag : " << flag << std::endl;
  // 	}
  //     }
  //   }

  //   int tof_nhits = 0;
  //   for(int seg=0; seg<NumOfSegTOF; ++seg){
  //     // TDC
  //     int nhit = gUnpacker.get_entries(k_device, 0, seg, 0, 1);
  //     if( nhit<=0 ) continue;
  //     int tdc = gUnpacker.get(k_device, 0, seg, 0, 1);
  //     if( tdc<=0 ) continue;
  //     hptr_array[tdc_id +seg]->Fill( tdc );
  //     hptr_array[tdc2d_id]->Fill( seg, tdc );
  //     if( hul_flag==0 )
  // 	hptr_array[tdc_id +NumOfSegTOF +seg]->Fill( tdc );
  //     // HitPat
  //     hptr_array[tof_hp_id]->Fill(seg);
  //     for( int seg2=0; seg2<NumOfSegSCH; ++seg2 ){
  // 	int nhit2 = gUnpacker.get_entries(k_device, 1, seg2, 0, 1);
  // 	if( nhit2<=0 ) continue;
  // 	int hit2 = gUnpacker.get(k_device, 1, seg2, 0, 1);
  // 	if( hit2<=0 ) continue;
  // 	if( hit2>0 && soft_flag!=0 ){
  // 	  if( gMsT.IsAccept( seg, seg2, tdc ) ){
  // 	    soft_flag = 0;
  // 	  }
  // 	}
  //     }
  //     tof_nhits++;
  //   }

  //   hptr_array[flag2d_id]->Fill( hul_flag, soft_flag );

  //   for(int seg=0; seg<NumOfSegSCH; ++seg){
  //     // HitPat
  //     int nhit = gUnpacker.get_entries(k_device, 1, seg, 0, 1);
  //     if( nhit!=0 ){
  // 	int flag = gUnpacker.get(k_device, 1, seg, 0, 1);
  // 	if( flag ) hptr_array[sch_hp_id]->Fill(seg);
  //     }
  //   }

// #if 0
//     // Debug, dump data relating this detector
//     gUnpacker.dump_data_device(k_device);
// #endif
//   }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  //------------------------------------------------------------------
  // TOF_HT
  //------------------------------------------------------------------
  {
    // data typep
    static const int k_device = gUnpacker.get_device_id("TOF_HT");
    static const int k_u      = 0; // up
    //    static const int k_d      = 1; // down
    //    static const int k_adc    = gUnpacker.get_data_id("TOF_HT","adc");
    static const int k_tdc    = gUnpacker.get_data_id("TOF_HT","tdc");

    // sequential id
    //    int tofa_id   = gHist.getSequentialID(kTOF_HT, 0, kADC);
    int toft_id   = gHist.getSequentialID(kTOF_HT, 0, kTDC);
    //    int tofawt_id = gHist.getSequentialID(kTOF_HT, 0, kADCwTDC);
    for(int seg = 0; seg<NumOfSegTOF_HT; ++seg){
      //     // ADC
      //     int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc);
      //     if( nhit!=0 ){
      //       unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
      //       hptr_array[tofa_id + seg]->Fill( adc );
      //     }
      // TDC
      //      nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      if(nhit != 0){
	int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	if( tdc!=0 ){
	  hptr_array[toft_id + seg]->Fill(tdc);
	  //	  // ADC w/TDC
	  //	  if( gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc)>0 ){
	  //	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	  //	    hptr_array[tofawt_id + seg]->Fill( adc );
	  //	  }
	}
      }
    }

    //   // Down PMT
    //   tofa_id   = gHist.getSequentialID(kTOF_HT, 0, kADC, NumOfSegTOF_HT+1);
    //   toft_id   = gHist.getSequentialID(kTOF_HT, 0, kTDC, NumOfSegTOF_HT+1);
    //   tofawt_id = gHist.getSequentialID(kTOF_HT, 0, kADCwTDC, NumOfSegTOF_HT+1);

    //   for(int seg = 0; seg<NumOfSegTOF_HT; ++seg){
    //     // ADC
    //     int nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc);
    //     if(nhit != 0){
    //       unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
    //       hptr_array[tofa_id + seg]->Fill(adc);
    //     }

    //     // TDC
    //     nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
    //     if(nhit != 0){
    //       int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
    //       if(tdc != 0){
    //         hptr_array[toft_id + seg]->Fill(tdc);
    //         // ADC w/TDC
    //         if( gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc)>0 ){
    //           unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
    //           hptr_array[tofawt_id + seg]->Fill( adc );
    //         }
    //       }
    //     }
    //   }

    // Hit pattern && multiplicity
    static const int tofhit_id = gHist.getSequentialID(kTOF_HT, 0, kHitPat);
    static const int tofmul_id = gHist.getSequentialID(kTOF_HT, 0, kMulti);
    int multiplicity = 0;
    for(int seg=0; seg<NumOfSegTOF_HT; ++seg){
      int nhit_tofu = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      //      int nhit_tofd = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      // AND
      //      if(nhit_tofu!=0 && nhit_tofd!=0){
      if(nhit_tofu!=0){
	unsigned int tdc_u = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	//	unsigned int tdc_d = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	// TDC AND
	//	if(tdc_u!=0 && tdc_d!=0){
	if(tdc_u!=0){
	  hptr_array[tofhit_id]->Fill(seg);
	  ++multiplicity;
	}
      }
    }

    hptr_array[tofmul_id]->Fill(multiplicity);

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif


  //------------------------------------------------------------------
  //LC
  //------------------------------------------------------------------
  {
    // data typep
    static const int k_device = gUnpacker.get_device_id("LC");
    static const int k_u      = 0; // up
    //    static const int k_d      = 1; // down
    //    static const int k_adc    = gUnpacker.get_data_id("LC","adc");
    static const int k_tdc    = gUnpacker.get_data_id("LC","tdc");

    // sequential id
    //    int lca_id   = gHist.getSequentialID(kLC, 0, kADC);
    int lct_id   = gHist.getSequentialID(kLC, 0, kTDC);
    for(int seg = 0; seg<NumOfSegLC; ++seg){
      //      // ADC
      //      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc);
      //      if( nhit!=0 ){
      //	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
      //	hptr_array[lca_id + seg]->Fill( adc );
      //      }
      // TDC
      //      nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      if(nhit != 0){
	int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	if( tdc!=0 ){
	  hptr_array[lct_id + seg]->Fill(tdc);
	}
      }
    }

    // Down PMT
    //    lca_id   = gHist.getSequentialID(kLC, 0, kADC, NumOfSegLC+1);
    //    lct_id   = gHist.getSequentialID(kLC, 0, kTDC, NumOfSegLC+1);
    //
    //    for(int seg = 0; seg<NumOfSegLC; ++seg){
    //      // ADC
    //      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc);
    //      if(nhit != 0){
    //	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
    //	hptr_array[lca_id + seg]->Fill(adc);
    //      }

    // TDC
    //      nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
    //      if(nhit != 0){
    //        int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
    //        if(tdc != 0){
    //          hptr_array[lct_id + seg]->Fill(tdc);
    //        }
    //      }
    //    }

    // Hit pattern && multiplicity
    static const int lchit_id = gHist.getSequentialID(kLC, 0, kHitPat);
    static const int lcmul_id = gHist.getSequentialID(kLC, 0, kMulti);
    int multiplicity = 0;
    for(int seg=0; seg<NumOfSegLC; ++seg){
      int nhit_lcu = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      //      int nhit_lcd = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      // AND
      //      if(nhit_lcu!=0 && nhit_lcd!=0){
      if(nhit_lcu!=0){
	unsigned int tdc_u = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	//	unsigned int tdc_d = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	// TDC AND
	//	if(tdc_u!=0 && tdc_d!=0){
	if(tdc_u!=0){
	  hptr_array[lchit_id]->Fill(seg);
	  ++multiplicity;
	}
      }
    }

    hptr_array[lcmul_id]->Fill(multiplicity);

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // Correlation (2D histograms) -------------------------------------
  {
    // data typep
    static const int k_device_bh1  = gUnpacker.get_device_id("BH1");
    static const int k_device_bh2  = gUnpacker.get_device_id("BH2");
    static const int k_device_sch  = gUnpacker.get_device_id("SCH");
    static const int k_device_tof  = gUnpacker.get_device_id("TOF");
    static const int k_device_bc3  = gUnpacker.get_device_id("BC3");
    static const int k_device_bc4  = gUnpacker.get_device_id("BC4");
    static const int k_device_sdc1 = gUnpacker.get_device_id("SDC1");
    static const int k_device_sdc2 = gUnpacker.get_device_id("SDC2");

    // sequential id
    int cor_id= gHist.getSequentialID(kCorrelation, 0, 0, 1);

    // BH1 vs BH2
    TH2* hcor_bh1bh2 = dynamic_cast<TH2*>(hptr_array[cor_id++]);
    for(int seg1 = 0; seg1<NumOfSegBH1; ++seg1){
      for(int seg2 = 0; seg2<NumOfSegBH2; ++seg2){
	int hitBH1 = gUnpacker.get_entries(k_device_bh1, 0, seg1, 0, 1);
	int hitBH2 = gUnpacker.get_entries(k_device_bh2, 0, seg2, 0, 1);
	if(hitBH1 == 0 || hitBH2 == 0)continue;
	int tdcBH1 = gUnpacker.get(k_device_bh1, 0, seg1, 0, 1);
	int tdcBH2 = gUnpacker.get(k_device_bh2, 0, seg2, 0, 1);
	if(tdcBH1 != 0 && tdcBH2 != 0){
	  hcor_bh1bh2->Fill(seg1, seg2);
	}
      }
    }

    // TOF vs SCH
    TH2* hcor_tofsch = dynamic_cast<TH2*>(hptr_array[cor_id++]);
    for(int seg1 = 0; seg1<NumOfSegSCH; ++seg1){
      for(int seg2 = 0; seg2<NumOfSegTOF; ++seg2){
	int hitSCH = gUnpacker.get_entries(k_device_sch, 0, seg1, 0, 1);
	int hitTOF = gUnpacker.get_entries(k_device_tof, 0, seg2, 0, 1);
	if(hitTOF == 0 || hitSCH == 0) continue;
	int tdcSCH = gUnpacker.get(k_device_sch, 0, seg1, 0, 1);
	int tdcTOF = gUnpacker.get(k_device_tof, 0, seg2, 0, 1);
	if(tdcTOF != 0 && tdcSCH != 0){
	  hcor_tofsch->Fill(seg1, seg2);
	}
      }
    }

    // BC3 vs BC4
    TH2* hcor_bc3bc4 = dynamic_cast<TH2*>(hptr_array[cor_id++]);
    for(int wire1 = 0; wire1<NumOfWireBC3; ++wire1){
      for(int wire2 = 0; wire2<NumOfWireBC4; ++wire2){
	int hitBC3 = gUnpacker.get_entries(k_device_bc3, 0, 0, wire1, 0);
	int hitBC4 = gUnpacker.get_entries(k_device_bc4, 5, 0, wire2, 0);
	if(hitBC3 == 0 || hitBC4 == 0)continue;
	hcor_bc3bc4->Fill(wire1, wire2);
      }
    }

    // SDC2 vs SDC1
    TH2* hcor_sdc1sdc2 = dynamic_cast<TH2*>(hptr_array[cor_id++]);
    for(int wire1 = 0; wire1<NumOfWireSDC1; ++wire1){
      for(int wire2 = 0; wire2<NumOfWireSDC2; ++wire2){
	int hitSDC1 = gUnpacker.get_entries(k_device_sdc1, 0, 0, wire1, 0);
	int hitSDC2 = gUnpacker.get_entries(k_device_sdc2, 0, 0, wire2, 0);
	if( hitSDC1 == 0 || hitSDC2 == 0 ) continue;
	hcor_sdc1sdc2->Fill( wire1, wire2 );
      }
    }
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  //------------------------------------------------------------------
  // BTOF
  //------------------------------------------------------------------
  {
    // Unpacker
    static const int k_d_bh1  = gUnpacker.get_device_id("BH1");
    static const int k_d_bh2  = gUnpacker.get_device_id("BH2");

    static const int k_u      = 0; // up
    static const int k_d      = 1; // down
    static const int k_tdc    = gUnpacker.get_data_id("BH1", "tdc");

    // HodoParam
    static const int cid_bh1  = 1;
    static const int cid_bh2  = 2;
    static const int plid     = 0;

    // Sequential ID
    static const int btof_id  = gHist.getSequentialID(kMisc, 0, kTDC);

    // BH2
    double t0  = -999;
    double ofs = 0;
    int    seg = 0;
    int nhitu = gUnpacker.get_entries(k_d_bh2, 0, seg, k_u, k_tdc);
    int nhitd = gUnpacker.get_entries(k_d_bh2, 0, seg, k_d, k_tdc);
    if( nhitu != 0 && nhitd != 0 ){
      int tdcu = gUnpacker.get(k_d_bh2, 0, seg, k_u, k_tdc);
      int tdcd = gUnpacker.get(k_d_bh2, 0, seg, k_d, k_tdc);
      if( tdcu != 0 && tdcd != 0 ){
	HodoParamMan& hodoMan = HodoParamMan::GetInstance();
	double bh2ut, bh2dt;
	hodoMan.GetTime(cid_bh2, plid, seg, k_u, tdcu, bh2ut);
	hodoMan.GetTime(cid_bh2, plid, seg, k_d, tdcd, bh2dt);
	t0 = (bh2ut+bh2dt)/2;
      }//if(tdc)
    }// if(nhit)

    // BH1
    for(int seg = 0; seg<NumOfSegBH1; ++seg){
      int nhitu = gUnpacker.get_entries(k_d_bh1, 0, seg, k_u, k_tdc);
      int nhitd = gUnpacker.get_entries(k_d_bh1, 0, seg, k_d, k_tdc);
      if(nhitu != 0 &&  nhitd != 0){
	int tdcu = gUnpacker.get(k_d_bh1, 0, seg, k_u, k_tdc);
	int tdcd = gUnpacker.get(k_d_bh1, 0, seg, k_d, k_tdc);
	if(tdcu != 0 && tdcd != 0){
	  HodoParamMan& hodoMan = HodoParamMan::GetInstance();
	  double bh1tu, bh1td;
	  hodoMan.GetTime(cid_bh1, plid, seg, k_u, tdcu, bh1tu);
	  hodoMan.GetTime(cid_bh1, plid, seg, k_d, tdcd, bh1td);
	  double mt = (bh1tu+bh1td)/2.;
	  double btof = mt-(t0+ofs);
	  hptr_array[btof_id]->Fill(btof);
	}// if(tdc)
      }// if(nhit)
    }// for(seg)
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // Efficiency
  http::UpdateBcOutEfficiency();
  http::UpdateSdcInOutEfficiency();

  return 0;
}

}
