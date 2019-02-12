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
  gHttp.Register(gHist.createSDC1());
  gHttp.Register(gHist.createSAC());
  gHttp.Register(gHist.createSCH());
  gHttp.Register(gHist.createFHT1());
  gHttp.Register(gHist.createSDC2());
  gHttp.Register(gHist.createSDC3());
  gHttp.Register(gHist.createFHT2());
  gHttp.Register(gHist.createTOF());
  gHttp.Register(gHist.createTOF_HT());
  gHttp.Register(gHist.createLAC());
  gHttp.Register(gHist.createLC());
  gHttp.Register(gHist.createCorrelation());
  gHttp.Register(gHist.createTriggerFlag());
  gHttp.Register(gHist.createMsT());
  gHttp.Register(gHist.createCFT());
  gHttp.Register(gHist.createBGO());
  gHttp.Register(gHist.createPiID());
  gHttp.Register(gHist.createCorrelation_catch());
  gHttp.Register(gHist.createDAQ(false));
  gHttp.Register(gHist.createTimeStamp(false));
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
  gHttp.Register(http::BC3TDC());
  gHttp.Register(http::BC3HitMulti());
  gHttp.Register(http::BC4TDC());
  gHttp.Register(http::BC4HitMulti());
  gHttp.Register(http::BH2ADC());
  gHttp.Register(http::BH2TDC());
  gHttp.Register(http::SFTTDCTOT());
  gHttp.Register(http::SFTHitMulti());
  gHttp.Register(http::SFT2D());
  gHttp.Register(http::SDC1TDC());
  gHttp.Register(http::SDC1HitMulti());
  gHttp.Register(http::SAC());
  gHttp.Register(http::SCHTDC());
  gHttp.Register(http::SCHTOT());
  gHttp.Register(http::SCHHitMulti());
  gHttp.Register(http::FHT1TDC());
  gHttp.Register(http::FHT1TOT());
  gHttp.Register(http::FHT1HitMulti());
  // gHttp.Register(http::MsTTDC());
  gHttp.Register(http::SDC2TDCTOT());
  gHttp.Register(http::SDC2HitMulti());
  gHttp.Register(http::SDC3TDCTOT());
  gHttp.Register(http::SDC3HitMulti());
  gHttp.Register(http::FHT2TDC());
  gHttp.Register(http::FHT2TOT());
  gHttp.Register(http::FHT2HitMulti());
  gHttp.Register(http::TOFADC());
  gHttp.Register(http::TOFTDC());
  gHttp.Register(http::TOF_HT());
  gHttp.Register(http::LAC());
  gHttp.Register(http::LC());
  gHttp.Register(http::TriggerFlag());
  gHttp.Register(http::HitPattern());
  gHttp.Register(http::CFTTDC());
  gHttp.Register(http::CFTTDC2D());
  gHttp.Register(http::CFTTOT());
  gHttp.Register(http::CFTTOT2D());
  gHttp.Register(http::CFTHighGain());
  gHttp.Register(http::CFTHighGain2D());
  gHttp.Register(http::CFTLowGain());
  gHttp.Register(http::CFTLowGain2D());
  gHttp.Register(http::CFTPedestal());
  gHttp.Register(http::CFTPedestal2D());
  gHttp.Register(http::CFTHitPat());
  gHttp.Register(http::CFTMulti());
  gHttp.Register(http::BGOFADC());
  gHttp.Register(http::BGOADC());
  gHttp.Register(http::BGOTDC());
  gHttp.Register(http::BGOADCTDC2D());
  gHttp.Register(http::BGOHitMulti());
  gHttp.Register(http::PiIDTDC());
  gHttp.Register(http::PiIDHighGain());
  gHttp.Register(http::PiIDLowGain());
  gHttp.Register(http::PiIDHitMulti());
  gHttp.Register(http::BcOutEfficiency());
  gHttp.Register(http::SdcInOutEfficiency());
  gHttp.Register(http::CFTEfficiency());
  gHttp.Register(http::Correlation());
  gHttp.Register(http::CorrelationFHT());
  gHttp.Register(http::DAQ());

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

  // const RawData*      const rawData = event.GetRawData();
  const HodoAnalyzer* const hodoAna = event.GetHodoAnalyzer();
  // const DCAnalyzer*   const DCAna   = event.GetDCAnalyzer();

  // TriggerFlag ---------------------------------------------------
  std::bitset<NumOfSegTFlag> trigger_flag;
  //  bool matrix2d_flag = false;
  //  bool matrix3d_flag = false;
  {
    static const int k_device = gUnpacker.get_device_id("TFlag");
    static const int k_tdc    = gUnpacker.get_data_id("TFlag", "tdc");

    static const int tf_tdc_id = gHist.getSequentialID( kTriggerFlag, 0, kTDC );
    static const int tf_hit_id = gHist.getSequentialID( kTriggerFlag, 0, kHitPat );
    for( int seg=0; seg<NumOfSegTFlag; ++seg ){
      int nhit = gUnpacker.get_entries( k_device, 0, seg, 0, k_tdc );
      if( nhit>0 ){
	int tdc = gUnpacker.get( k_device, 0, seg, 0, k_tdc );
	if( tdc>0 ){
	  trigger_flag.set(seg);
	  hptr_array[tf_tdc_id+seg]->Fill( tdc );
	  hptr_array[tf_hit_id]->Fill( seg );
	  //	  if( seg==8 ) matrix2d_flag = true;
	  //	  if( seg==9 ) matrix3d_flag = true;
	}
      }
    }// for(seg)

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // TimeStamp --------------------------------------------------------
  {
    static const int k_device = gUnpacker.get_device_id("VME-RM");
    static const int k_time   = gUnpacker.get_data_id("VME-RM", "time");
    static const int hist_id  = gHist.getSequentialID(kTimeStamp, 0, kTDC);
    int time0 = 0;
    for( int i=0; i<NumOfVmeRm; ++i ){
      int nhit = gUnpacker.get_entries( k_device, i, 0, 0, k_time );
      if( nhit>0 ){
	int time = gUnpacker.get( k_device, i, 0, 0, k_time );
	if( i==0 ) time0 = time;
	TH1* h = dynamic_cast<TH1*>(hptr_array[hist_id +i]);
	h->Fill( time - time0 );
      }
    }
  }
#if FLAG_DAQ
  // DAQ -------------------------------------------------------------
  {
    // node id
    static const int k_eb      = gUnpacker.get_fe_id("k18eb");
    static const int k_vme     = gUnpacker.get_fe_id("vme01");
    static const int k_vme2     = gUnpacker.get_fe_id("vme02");
    static const int k_opt     = gUnpacker.get_fe_id("optlink01");
    static const int k_clite   = gUnpacker.get_fe_id("clite1");
    static const int k_hul_sdc   = gUnpacker.get_fe_id("hul02sdc-1");
    static const int k_hul_sft   = gUnpacker.get_fe_id("hul02sft-1");
    static const int k_easiroc = gUnpacker.get_fe_id("easiroc0");
    static const int k_Veasiroc = gUnpacker.get_fe_id("Veasiroc16");

    // sequential id
    static const int eb_id      = gHist.getSequentialID(kDAQ, kEB, kHitPat);
    static const int vme_id     = gHist.getSequentialID(kDAQ, kVME, kHitPat2D);
    static const int clite_id   = gHist.getSequentialID(kDAQ, kCLite, kHitPat2D);
    static const int opt_id   = gHist.getSequentialID(kDAQ, kOpt, kHitPat2D);
    static const int hul_id   = gHist.getSequentialID(kDAQ, kHUL, kHitPat2D);
    static const int easiroc_id = gHist.getSequentialID(kDAQ, kEASIROC, kHitPat2D);
//    static const int Veasiroc_id = gHist.getSequentialID(kDAQ, kVMEEASIROC, kHitPat2D);
    //    static const int misc_id    = gHist.getSequentialID(kDAQ, kMiscNode, kHitPat2D);

    { // EB
      int data_size = gUnpacker.get_node_header(k_eb, DAQNode::k_data_size);
      hptr_array[eb_id]->Fill(data_size);
    }

    { // VME node
      TH2* h = dynamic_cast<TH2*>(hptr_array[vme_id]);
	int data_size = gUnpacker.get_node_header( k_vme, DAQNode::k_data_size);
        int i = 0;
	h->Fill(i, data_size );
	data_size = gUnpacker.get_node_header( k_vme2, DAQNode::k_data_size);
        i = 1;
	h->Fill( i, data_size );
    }

    { // CLite node
      TH2* h = dynamic_cast<TH2*>(hptr_array[clite_id]);
      for(int i = 0; i<14; ++i){
	int data_size = gUnpacker.get_node_header(k_clite+i, DAQNode::k_data_size);
	h->Fill( i, data_size );
      }
    }

    { // EASIROC & VMEEASIROC node
      TH2* h = dynamic_cast<TH2*>(hptr_array[easiroc_id]);
      for(int i = 0; i<16; ++i){
	int data_size = gUnpacker.get_node_header(k_easiroc+i, DAQNode::k_data_size);
	h->Fill( i, data_size );
      }
      for(int i = 16; i<102; ++i){
	int data_size = gUnpacker.get_node_header(k_Veasiroc+i-16, DAQNode::k_data_size);
	h->Fill( i, data_size );
      }
    }

    { // HUL node
      TH2* h = dynamic_cast<TH2*>(hptr_array[hul_id]);
      for(int i = 0; i<7; ++i){
	int data_size = gUnpacker.get_node_header(k_hul_sdc+i, DAQNode::k_data_size);
	h->Fill( i, data_size );
      }
      for(int i = 10; i<9+10; ++i){
	int data_size = gUnpacker.get_node_header(k_hul_sft+i-10, DAQNode::k_data_size);
	h->Fill( i, data_size );
      }
    }

    { // Opt node
      TH2* h = dynamic_cast<TH2*>(hptr_array[opt_id]);
      for(int i = 0; i<2; ++i){
	int data_size = gUnpacker.get_node_header(k_opt+i, DAQNode::k_data_size);
	h->Fill( i, data_size );
      }
    }

    { // Misc node
      //      TH2* h = dynamic_cast<TH2*>(hptr_array[misc_id]);

    }


  }

#endif

  if( trigger_flag[SpillEndFlag] ) return 0;

  // MsT -----------------------------------------------------------
  {
    static const int k_device = gUnpacker.get_device_id("MsT");
    static const int k_tof    = gUnpacker.get_plane_id("MsT", "TOF");
    static const int k_sch    = gUnpacker.get_plane_id("MsT", "SCH");
    static const int k_tag    = gUnpacker.get_plane_id("MsT", "tag");
    static const int k_ch     = 0;
    static const int k_tdc    = 0;
    static const int k_n_flag = 7;

    static const int toft_id     = gHist.getSequentialID(kMsT, 0, kTDC);
    static const int scht_id     = gHist.getSequentialID(kMsT, 0, kTDC, NumOfSegTOF*2 +1);
    static const int toft_2d_id  = gHist.getSequentialID(kMsT, 0, kTDC2D);

    static const int tofhit_id   = gHist.getSequentialID(kMsT, 0, kHitPat, 0);
    static const int schhit_id   = gHist.getSequentialID(kMsT, 0, kHitPat, 1);
    static const int flag_id     = gHist.getSequentialID(kMsT, 0, kHitPat, 2);

    // TOF
    for(int seg=0; seg<NumOfSegTOF; ++seg){
      int nhit = gUnpacker.get_entries(k_device, k_tof, seg, k_ch, k_tdc);
      for(int m = 0; m<nhit; ++m){
	hptr_array[tofhit_id]->Fill(seg);

	unsigned int tdc = gUnpacker.get(k_device, k_tof, seg, k_ch, k_tdc, m);
	if(tdc!=0){
	  hptr_array[toft_id + seg]->Fill(tdc);
	  hptr_array[toft_2d_id]->Fill(seg, tdc);
	}
      }
    }// TOF

    // SCH
    for(int seg=0; seg<NumOfSegSCH; ++seg){
      int nhit = gUnpacker.get_entries(k_device, k_sch, seg, k_ch, k_tdc);
      if(nhit!=0){
	hptr_array[schhit_id]->Fill(seg);
      }
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, k_sch, seg, k_ch, k_tdc, m);
	if(tdc!=0){
	  hptr_array[scht_id + seg]->Fill(tdc);
	}
      }
    }// SCH

    // FLAG
    for(int i=0; i<k_n_flag; ++i){
      int nhit = gUnpacker.get_entries(k_device, k_tag, 0, k_ch, i);
      if(nhit!=0){
	int flag = gUnpacker.get(k_device, k_tag, 0, k_ch, i, 0);
	if(flag) hptr_array[flag_id]->Fill(i);
      }
    }// FLAG

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }// MsT

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // BH1 -----------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("BH1");
    static const int k_u        = 0; // up
    static const int k_d        = 1; // down
    static const int k_adc      = gUnpacker.get_data_id("BH1", "adc");
    static const int k_tdc      = gUnpacker.get_data_id("BH1", "fpga_leading");

    // TDC gate range
    static const unsigned int tdc_min = gUser.GetParameter("BH1_TDC_FPGA", 0);
    static const unsigned int tdc_max = gUser.GetParameter("BH1_TDC_FPGA", 1);

    // Up PMT
    int bh1a_id   = gHist.getSequentialID(kBH1, 0, kADC);
    int bh1t_id   = gHist.getSequentialID(kBH1, 0, kTDC);
    int bh1awt_id = gHist.getSequentialID(kBH1, 0, kADCwTDC);
    for(int seg=0; seg<NumOfSegBH1; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc);
      if(nhit!=0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	hptr_array[bh1a_id + seg]->Fill(adc);
      }

      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc, m);
	if(tdc!=0){
	  hptr_array[bh1t_id + seg]->Fill(tdc);
	  // ADC wTDC_FPGA
	  if( tdc>tdc_min && tdc<tdc_max && gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_u, k_adc);
	    hptr_array[bh1awt_id + seg]->Fill( adc );
	  }
	}
      }
    }

    // Down PMT
    bh1a_id   = gHist.getSequentialID(kBH1, 0, kADC, NumOfSegBH1+1);
    bh1t_id   = gHist.getSequentialID(kBH1, 0, kTDC, NumOfSegBH1+1);
    bh1awt_id = gHist.getSequentialID(kBH1, 0, kADCwTDC, NumOfSegBH1+1);
    for(int seg=0; seg<NumOfSegBH1; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc);
      if(nhit!=0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
	hptr_array[bh1a_id + seg]->Fill(adc);
      }

      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc, m);
	if( tdc!=0 ){
	  hptr_array[bh1t_id + seg]->Fill(tdc);
	  // ADC w/TDC_FPGA
	  if( tdc>tdc_min && tdc<tdc_max && gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, k_d, k_adc);
	    hptr_array[bh1awt_id + seg]->Fill(adc);
	  }
	}
      }
    }

    // Hit pattern && multiplicity
    static const int bh1hit_id = gHist.getSequentialID(kBH1, 0, kHitPat);
    static const int bh1mul_id = gHist.getSequentialID(kBH1, 0, kMulti);
    int multiplicity  = 0;
    int cmultiplicity = 0;
    for(int seg=0; seg<NumOfSegBH1; ++seg){
      int nhit_bh1u = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      int nhit_bh1d = gUnpacker.get_entries(k_device, 0, seg, k_d, k_tdc);
      // AND
      if(nhit_bh1u!=0 && nhit_bh1d!=0){
	unsigned int tdc_u = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	unsigned int tdc_d = gUnpacker.get(k_device, 0, seg, k_d, k_tdc);
	// TDC AND
	if(tdc_u != 0 && tdc_d != 0){
	  hptr_array[bh1hit_id]->Fill(seg);
	  ++multiplicity;
	  // TDC range
	  if(true
	     && tdc_min < tdc_u && tdc_u < tdc_max
	     && tdc_min < tdc_d && tdc_d < tdc_max
	     ){
	    hptr_array[bh1hit_id+1]->Fill(seg); // CHitPat
	    ++cmultiplicity;
	  }// TDC range OK
	}// TDC AND
      }// AND
    }// for(seg)

    hptr_array[bh1mul_id]->Fill(multiplicity);
    hptr_array[bh1mul_id+1]->Fill(cmultiplicity); // CMulti

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }// BH1

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

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
    static const int bft_mul_id   = gHist.getSequentialID(kBFT, 0, kMulti,   1);
    static const int bft_cmul_id  = gHist.getSequentialID(kBFT, 0, kMulti,  11);

    static const int bft_ct_2d_id = gHist.getSequentialID(kBFT, 0, kTDC2D,   1);
    static const int bft_ctot_2d_id = gHist.getSequentialID(kBFT, 0, kADC2D, 1);

    int multiplicity  = 0; // includes both u and d planes.
    int cmultiplicity = 0; // includes both u and d planes.
    for( Int_t l=0; l<NumOfPlaneBFT; ++l ){
      Int_t nh = hodoAna->GetNHitsBFT(l);
      enum { U, D };
      for( Int_t i=0; i<nh; ++i ){
	const FiberHit* const hit = hodoAna->GetHitBFT(l, i);
	if( !hit ) continue;
	Int_t mhit  = hit->GetNumOfHit();
	Int_t seg   = hit->SegmentId();
	Int_t prev = 0;
	for( Int_t m=0; m<mhit; ++m ){
	  Int_t leading  = hit->GetLeading(m);
	  Int_t tot      = hit->GetWidth(m);
	  hptr_array[bft_t_id +l]->Fill( leading );
	  hptr_array[bft_tot_id +l]->Fill(tot);
	  if( tdc_min < leading && leading < tdc_max ){
	    ++multiplicity;
	    hptr_array[bft_hit_id +l]->Fill(seg);
	  }
	  if( prev == leading ) continue;
	  prev = leading;
	  if( tot==0 ) continue;
	  hptr_array[bft_ct_id +l]->Fill(leading);
	  hptr_array[bft_ctot_id +l]->Fill(tot);
	  hptr_array[bft_ct_2d_id +l]->Fill(seg, leading);
	  hptr_array[bft_ctot_2d_id +l]->Fill(seg, tot);
	  if( tdc_min < leading && leading < tdc_max ){
	    ++cmultiplicity;
	    hptr_array[bft_chit_id +l]->Fill(seg);
	  }
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
    static const int k_tdc    = gUnpacker.get_data_id("BH2", "fpga_leading");
    static const int k_mt     = gUnpacker.get_data_id("BH2", "fpga_meantime");

    // TDC gate range
    static const unsigned int tdc_min = gUser.GetParameter("BH2_TDC_FPGA", 0);
    static const unsigned int tdc_max = gUser.GetParameter("BH2_TDC_FPGA", 1);

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
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc, m);
	if(tdc!=0){
	  hptr_array[bh2t_id + seg]->Fill(tdc);
	  // ADC w/TDC_FPGA
	  if( tdc_min<tdc && tdc<tdc_max && gUnpacker.get_entries(k_device, 0, seg, k_u, k_adc)>0){
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
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc, m);
	if( tdc!=0 ){
	  hptr_array[bh2t_id + seg]->Fill(tdc);
	  // ADC w/TDC_FPGA
	  if( tdc_min<tdc && tdc<tdc_max && gUnpacker.get_entries(k_device, 0, seg, k_d, k_adc)>0){
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

    // Mean Timer
    bh2t_id   = gHist.getSequentialID(kBH2, 0, kBH2MT, 1);
    for(int seg=0; seg<NumOfSegBH2; ++seg){
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_mt);
      for(int m = 0; m<nhit; ++m){
	unsigned int mt = gUnpacker.get(k_device, 0, seg, k_u, k_mt, m);
	if(mt!=0){
	  hptr_array[bh2t_id + seg]->Fill(mt);
	}
      }
    }

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
    int sft_mul_id  = gHist.getSequentialID(kSFT, 0, kMulti,   1);
    int sft_cmul_id = gHist.getSequentialID(kSFT, 0, kMulti,  11);

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

  // SAC -----------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SAC");
    static const int k_adc    = gUnpacker.get_data_id("SAC","adc");
    static const int k_tdc    = gUnpacker.get_data_id("SAC","tdc");

    // sequential id
    static const int saca_id   = gHist.getSequentialID(kSAC, 0, kADC,     1);
    static const int sact_id   = gHist.getSequentialID(kSAC, 0, kTDC,     1);
    static const int sacawt_id = gHist.getSequentialID(kSAC, 0, kADCwTDC, 1);
    static const int sach_id   = gHist.getSequentialID(kSAC, 0, kHitPat,  1);
    static const int sacm_id   = gHist.getSequentialID(kSAC, 0, kMulti,   1);

    int multiplicity = 0;
    for(int seg = 0; seg<NumOfRoomsSAC; ++seg){
      // ADC
      int nhit_a = gUnpacker.get_entries(k_device, 0, seg, 0, k_adc);
      if( nhit_a!=0 ){
	int adc = gUnpacker.get(k_device, 0, seg, 0, k_adc);
	hptr_array[saca_id + seg]->Fill( adc );
      }
      // TDC
      int nhit_t = gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
      if( nhit_t!=0 ){
	int tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc);
	if( tdc!=0 ){
	  hptr_array[sact_id + seg]->Fill( tdc );
	  // ADC w/TDC
	  if( gUnpacker.get_entries(k_device, 0, seg, 0, k_adc)>0 ){
	    int adc = gUnpacker.get(k_device, 0, seg, 0, k_adc);
	    hptr_array[sacawt_id + seg]->Fill( adc );
	  }
	}
	hptr_array[sach_id]->Fill(seg);
	++multiplicity;
      }
    }

    hptr_array[sacm_id]->Fill( multiplicity );

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }//SAC

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

  //------------------------------------------------------------------
  // SDC2
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SDC2");
    static const int k_leading = gUnpacker.get_data_id("SDC2", "leading");
    static const int k_trailing = gUnpacker.get_data_id("SDC2", "trailing");

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SDC2_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SDC2_TDC", 1);
    // TOT gate range
    static const int tot_min = gUser.GetParameter("SDC2_TOT", 0);
    // static const int tot_max = gUser.GetParameter("SDC2_TOT", 1);

    // sequential id
    static const int sdc2t_id    = gHist.getSequentialID(kSDC2, 0, kTDC);
    static const int sdc2tot_id  = gHist.getSequentialID(kSDC2, 0, kADC);
    static const int sdc2t1st_id = gHist.getSequentialID(kSDC2, 0, kTDC2D);
    static const int sdc2hit_id  = gHist.getSequentialID(kSDC2, 0, kHitPat);
    static const int sdc2mul_id  = gHist.getSequentialID(kSDC2, 0, kMulti);
    static const int sdc2mulwt_id
      = gHist.getSequentialID(kSDC2, 0, kMulti, 1+NumOfLayersSDC2);

    static const int sdc2t_ctot_id    = gHist.getSequentialID(kSDC2, 0, kTDC, 11);
    static const int sdc2tot_ctot_id  = gHist.getSequentialID(kSDC2, 0, kADC, 11);
    static const int sdc2t1st_ctot_id = gHist.getSequentialID(kSDC2, 0, kTDC2D, 11);
    static const int sdc2hit_ctot_id  = gHist.getSequentialID(kSDC2, 0, kHitPat, 11);
    static const int sdc2mul_ctot_id  = gHist.getSequentialID(kSDC2, 0, kMulti, 11);
    static const int sdc2mulwt_ctot_id
      = gHist.getSequentialID(kSDC2, 0, kMulti, 1+NumOfLayersSDC2 + 10);

    // TDC & HitPat & Multi
    for(int l=0; l<NumOfLayersSDC2; ++l){
      int tdc = 0;
      int tdc_t = 0;
      int tot = 0;
      int tdc1st = 0;
      int multiplicity    = 0;
      int multiplicity_wt = 0;
      int multiplicity_ctot    = 0;
      int multiplicity_wt_ctot = 0;
      for( int w=0; w<NumOfWireSDC2; ++w ){
	int nhit_l = gUnpacker.get_entries(k_device, l, 0, w, k_leading);
	int nhit_t = gUnpacker.get_entries(k_device, l, 0, w, k_trailing);
	if( nhit_l == 0 ) continue;

        int hit_l_max = 0;
        int hit_t_max = 0;

        if(nhit_l != 0){
          hit_l_max = gUnpacker.get(k_device, l, 0, w, k_leading,  nhit_l - 1);
        }
        if(nhit_t != 0){
          hit_t_max = gUnpacker.get(k_device, l, 0, w, k_trailing, nhit_t - 1);
        }

	// This wire fired at least one times.
	++multiplicity;
	// hptr_array[sdc2hit_id + l]->Fill(w, nhit);

	bool flag_hit_wt = false;
	bool flag_hit_wt_ctot = false;
	for( int m = 0; m<nhit_l; ++m ){
	  tdc = gUnpacker.get(k_device, l, 0, w, k_leading, m);
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

	tdc1st = 0;
        if(nhit_l == nhit_t && hit_l_max > hit_t_max){
	  ++multiplicity_ctot;
          for(int m = 0; m<nhit_l; ++m){
            tdc = gUnpacker.get(k_device, l, 0, w, k_leading, m);
            tdc_t = gUnpacker.get(k_device, l, 0, w, k_trailing, m);
            tot = tdc - tdc_t;
            hptr_array[sdc2tot_id+l]->Fill(tot);
            if(tot < tot_min) continue;
	    hptr_array[sdc2t_ctot_id + l]->Fill(tdc);
	    hptr_array[sdc2tot_ctot_id+l]->Fill(tot);
	    if( tdc1st<tdc ) tdc1st = tdc;
	    if( tdc_min < tdc && tdc < tdc_max ){
	      flag_hit_wt_ctot = true;
	    }
          }
        }

	if( tdc1st!=0 ) hptr_array[sdc2t1st_ctot_id +l]->Fill( tdc1st );
	if( flag_hit_wt_ctot ){
	  ++multiplicity_wt_ctot;
	  hptr_array[sdc2hit_ctot_id + l]->Fill( w );

        }
      }

      hptr_array[sdc2mul_id + l]->Fill(multiplicity);
      hptr_array[sdc2mulwt_id + l]->Fill(multiplicity_wt);
      hptr_array[sdc2mul_ctot_id   + l]->Fill(multiplicity_ctot);
      hptr_array[sdc2mulwt_ctot_id + l]->Fill(multiplicity_wt_ctot);
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  //------------------------------------------------------------------
  // SDC3
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SDC3");
    static const int k_leading = gUnpacker.get_data_id("SDC3", "leading");
    static const int k_trailing = gUnpacker.get_data_id("SDC3", "trailing");

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("SDC3_TDC", 0);
    static const int tdc_max = gUser.GetParameter("SDC3_TDC", 1);
    // TOT gate range
    static const int tot_min = gUser.GetParameter("SDC3_TOT", 0);
    static const int tot_max = gUser.GetParameter("SDC3_TOT", 1);


    // sequential id
    static const int sdc3t_id    = gHist.getSequentialID(kSDC3, 0, kTDC);
    static const int sdc3tot_id  = gHist.getSequentialID(kSDC3, 0, kADC);
    static const int sdc3t1st_id = gHist.getSequentialID(kSDC3, 0, kTDC2D);
    static const int sdc3hit_id  = gHist.getSequentialID(kSDC3, 0, kHitPat);
    static const int sdc3mul_id  = gHist.getSequentialID(kSDC3, 0, kMulti);
    static const int sdc3mulwt_id
      = gHist.getSequentialID(kSDC3, 0, kMulti, 1+NumOfLayersSDC3);

    static const int sdc3t_ctot_id    = gHist.getSequentialID(kSDC3, 0, kTDC, 11);
    static const int sdc3tot_ctot_id  = gHist.getSequentialID(kSDC3, 0, kADC, 11);
    static const int sdc3t1st_ctot_id = gHist.getSequentialID(kSDC3, 0, kTDC2D, 11);
    static const int sdc3hit_ctot_id  = gHist.getSequentialID(kSDC3, 0, kHitPat, 11);
    static const int sdc3mul_ctot_id  = gHist.getSequentialID(kSDC3, 0, kMulti, 11);
    static const int sdc3mulwt_ctot_id
      = gHist.getSequentialID(kSDC3, 0, kMulti, 1+NumOfLayersSDC3 + 10);

    // TDC & HitPat & Multi
    for(int l=0; l<NumOfLayersSDC3; ++l){
      int tdc = 0;
      int tdc_t = 0;
      int tot = 0;
      int tdc1st = 0;
      int multiplicity    = 0;
      int multiplicity_wt = 0;
      int multiplicity_ctot    = 0;
      int multiplicity_wt_ctot = 0;
      int sdc3_nwire = 0;
      if( l==0 || l==1 )
	sdc3_nwire = NumOfWireSDC3Y;
      if( l==2 || l==3 )
	sdc3_nwire = NumOfWireSDC3X;

      for( int w=0; w<sdc3_nwire; ++w ){
	int nhit_l = gUnpacker.get_entries(k_device, l, 0, w, k_leading);
	int nhit_t = gUnpacker.get_entries(k_device, l, 0, w, k_trailing);
	if( nhit_l == 0 ) continue;

        int hit_l_max = 0;
        int hit_t_max = 0;

        if(nhit_l != 0){
          hit_l_max = gUnpacker.get(k_device, l, 0, w, k_leading,  nhit_l - 1);
        }
        if(nhit_t != 0){
          hit_t_max = gUnpacker.get(k_device, l, 0, w, k_trailing, nhit_t - 1);
        }

	// This wire fired at least one times.
	++multiplicity;
	// hptr_array[sdc3hit_id + l]->Fill(w, nhit);

	bool flag_hit_wt = false;
	bool flag_hit_wt_ctot = false;
	for( int m = 0; m<nhit_l; ++m ){
	  tdc = gUnpacker.get(k_device, l, 0, w, k_leading, m);
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

	tdc1st = 0;
        if(nhit_l == nhit_t && hit_l_max > hit_t_max){
	  ++multiplicity_ctot;
          for(int m = 0; m<nhit_l; ++m){
            tdc = gUnpacker.get(k_device, l, 0, w, k_leading, m);
            tdc_t = gUnpacker.get(k_device, l, 0, w, k_trailing, m);
            tot = tdc - tdc_t;
            hptr_array[sdc3tot_id+l]->Fill(tot);
            if(tot < tot_min || tot >tot_max) continue;
	    hptr_array[sdc3t_ctot_id + l]->Fill(tdc);
	    hptr_array[sdc3tot_ctot_id+l]->Fill(tot);
	    if( tdc1st<tdc ) tdc1st = tdc;
	    if( tdc_min < tdc && tdc < tdc_max ){
	      flag_hit_wt_ctot = true;
	    }
          }
        }

	if( tdc1st!=0 ) hptr_array[sdc3t1st_ctot_id +l]->Fill( tdc1st );
	if( flag_hit_wt_ctot ){
	  ++multiplicity_wt_ctot;
	  hptr_array[sdc3hit_ctot_id + l]->Fill( w );

        }
      }

      hptr_array[sdc3mul_id + l]->Fill(multiplicity);
      hptr_array[sdc3mulwt_id + l]->Fill(multiplicity_wt);
      hptr_array[sdc3mul_ctot_id   + l]->Fill(multiplicity_ctot);
      hptr_array[sdc3mulwt_ctot_id + l]->Fill(multiplicity_wt_ctot);
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // FHT1 -----------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("FHT1");
    static const int k_leading  = gUnpacker.get_data_id("FHT1", "leading");
    static const int k_trailing = gUnpacker.get_data_id("FHT1", "trailing");

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("FHT1_TDC", 0);
    static const int tdc_max = gUser.GetParameter("FHT1_TDC", 1);

    // sequential id
    for( int l=0; l<NumOfLayersFHT1; ++l ){
      for( int ud=0; ud<NumOfUDStructureFHT; ++ud ){
	//          int fht1_tdc_id     = gHist.getSequentialID(kFHT1, l, kTDC,    1+ ud*FHTOffset);
	//          int fht1_tot_id     = gHist.getSequentialID(kFHT1, l, kADC,    1+ ud*FHTOffset);
	int fht1_t_all_id   = gHist.getSequentialID(kFHT1, l, kTDC,
						    NumOfSegFHT1+1+ ud*FHTOffset);
	int fht1_tot_all_id = gHist.getSequentialID(kFHT1, l, kADC,
						    NumOfSegFHT1+1+ ud*FHTOffset);
	int fht1_hit_id     = gHist.getSequentialID(kFHT1, l, kHitPat, 1+ ud*FHTOffset);
	int fht1_mul_id     = gHist.getSequentialID(kFHT1, l, kMulti,  1+ ud*FHTOffset);

	int fht1_t_2d_id   = gHist.getSequentialID(kFHT1, l, kTDC2D, 1+ ud*FHTOffset);
	int fht1_tot_2d_id = gHist.getSequentialID(kFHT1, l, kADC2D, 1+ ud*FHTOffset);

	int multiplicity  = 0;

	for( int seg=0; seg<NumOfSegFHT1; ++seg ){
	  int nhit_l = gUnpacker.get_entries(k_device, l, seg, ud, k_leading);
	  std::vector<int> vtdc;
	  for(int m = 0; m<nhit_l; ++m){
	    int tdc      = gUnpacker.get(k_device, l, seg, ud, k_leading,  m);
	    hptr_array[fht1_t_all_id]->Fill(tdc);
	    hptr_array[fht1_t_2d_id]->Fill(seg, tdc);
	    if( tdc_min<tdc && tdc<tdc_max ){
	      ++multiplicity;
	      hptr_array[fht1_hit_id]->Fill(seg);
	    }
	    vtdc.push_back(tdc);
	  }
	  int nhit_t = gUnpacker.get_entries(k_device, l, seg, ud, k_trailing);
	  for(int m = 0; m<nhit_t; ++m){
	    int trailing = gUnpacker.get(k_device, l, seg, ud, k_trailing, m);
	    if( nhit_l == nhit_t ){
	      int tot      = vtdc[m] - trailing;
	      hptr_array[fht1_tot_all_id]->Fill(tot);
	      hptr_array[fht1_tot_2d_id]->Fill(seg, tot);
	    }
	  }
	  // hptr_array[fht1_tdc_id +i]->Fill(tdc);
	  //  hptr_array[fht1_tot_id +i]->Fill(tot);
	}
	hptr_array[fht1_mul_id]->Fill(multiplicity);
      }
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }//FHT1

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // FHT2 -----------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("FHT2");
    static const int k_leading  = gUnpacker.get_data_id("FHT2", "leading");
    static const int k_trailing = gUnpacker.get_data_id("FHT2", "trailing");

    // TDC gate range
    static const int tdc_min = gUser.GetParameter("FHT2_TDC", 0);
    static const int tdc_max = gUser.GetParameter("FHT2_TDC", 1);

    // sequential id
    for( int l=0; l<NumOfLayersFHT2; ++l ){
      for( int ud=0; ud<NumOfUDStructureFHT; ++ud ){
	//          int fht2_tdc_id     = gHist.getSequentialID(kFHT2, l, kTDC,    1+ ud*FHTOffset);
	//          int fht2_tot_id     = gHist.getSequentialID(kFHT2, l, kADC,    1+ ud*FHTOffset);
	int fht2_t_all_id   = gHist.getSequentialID(kFHT2, l, kTDC,
						    NumOfSegFHT2+1+ ud*FHTOffset);
	int fht2_tot_all_id = gHist.getSequentialID(kFHT2, l, kADC,
						    NumOfSegFHT2+1+ ud*FHTOffset);
	int fht2_hit_id     = gHist.getSequentialID(kFHT2, l, kHitPat, 1+ ud*FHTOffset);
	int fht2_mul_id     = gHist.getSequentialID(kFHT2, l, kMulti,  1+ ud*FHTOffset);

	int fht2_t_2d_id   = gHist.getSequentialID(kFHT2, l, kTDC2D, 1+ ud*FHTOffset);
	int fht2_tot_2d_id = gHist.getSequentialID(kFHT2, l, kADC2D, 1+ ud*FHTOffset);

	int multiplicity  = 0;

	for( int seg=0; seg<NumOfSegFHT2; ++seg ){
	  int nhit_l = gUnpacker.get_entries(k_device, l, seg, ud, k_leading);
	  std::vector<int> vtdc;
	  for(int m = 0; m<nhit_l; ++m){
	    int tdc      = gUnpacker.get(k_device, l, seg, ud, k_leading,  m);
	    hptr_array[fht2_t_all_id]->Fill(tdc);
	    hptr_array[fht2_t_2d_id]->Fill(seg, tdc);
	    if( tdc_min<tdc && tdc<tdc_max ){
	      ++multiplicity;
	      hptr_array[fht2_hit_id]->Fill(seg);
	    }
	    vtdc.push_back(tdc);
	  }
	  int nhit_t = gUnpacker.get_entries(k_device, l, seg, ud, k_trailing);
	  for(int m = 0; m<nhit_t; ++m){
	    int trailing = gUnpacker.get(k_device, l, seg, ud, k_trailing, m);
	    if( nhit_l == nhit_t ){
	      int tot      = vtdc[m] - trailing;
	      hptr_array[fht2_tot_all_id]->Fill(tot);
	      hptr_array[fht2_tot_2d_id]->Fill(seg, tot);
	    }
	  }
	  // hptr_array[fht2_tdc_id +i]->Fill(tdc);
	  //  hptr_array[fht2_tot_id +i]->Fill(tot);
	}
	hptr_array[fht2_mul_id]->Fill(multiplicity);
      }
    }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }//FHT2

  //------------------------------------------------------------------
  // TOF
  //------------------------------------------------------------------
  {
    // data typep
    static const int k_device = gUnpacker.get_device_id("TOF");
    static const int k_u      = 0; // up
    static const int k_d      = 1; // down
    static const int k_adc    = gUnpacker.get_data_id("TOF","adc");
    static const int k_tdc    = gUnpacker.get_data_id("TOF","fpga_leading");

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
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc, m);
	if(tdc!=0){
	  hptr_array[toft_id + seg]->Fill(tdc);
	  // ADC wTDC_FPGA
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
      for(int m = 0; m<nhit; ++m){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_d, k_tdc, m);
	if(tdc != 0){
	  hptr_array[toft_id + seg]->Fill(tdc);
	  // ADC w/TDC_FPGA
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
    for(int seg = 0; seg<NumOfSegHtTOF; ++seg){
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
    for(int seg=0; seg<NumOfSegHtTOF; ++seg){
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
  //LAC
  //------------------------------------------------------------------
  {
    // data typep
    static const int k_device = gUnpacker.get_device_id("LAC");
    static const int k_u      = 0; // up
    static const int k_tdc    = gUnpacker.get_data_id("LAC","tdc");

    int lact_id   = gHist.getSequentialID(kLAC, 0, kTDC);
    for(int seg = 0; seg<NumOfSegLAC; ++seg){
      int nhit = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      if(nhit != 0){
	int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	if( tdc!=0 ){
	  hptr_array[lact_id + seg]->Fill(tdc);
	}
      }
    }

    // Hit pattern && multiplicity
    static const int lachit_id = gHist.getSequentialID(kLAC, 0, kHitPat);
    static const int lacmul_id = gHist.getSequentialID(kLAC, 0, kMulti);
    int multiplicity = 0;
    for(int seg=0; seg<NumOfSegLAC; ++seg){
      int nhit_lac = gUnpacker.get_entries(k_device, 0, seg, k_u, k_tdc);
      if(nhit_lac!=0){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, k_u, k_tdc);
	if(tdc!=0){
	  hptr_array[lachit_id]->Fill(seg);
	  ++multiplicity;
	}
      }
    }

    hptr_array[lacmul_id]->Fill(multiplicity);

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
    static const int k_device_bft  = gUnpacker.get_device_id("BFT");
    static const int k_device_bh2  = gUnpacker.get_device_id("BH2");
    static const int k_device_sch  = gUnpacker.get_device_id("SCH");
    static const int k_device_tof  = gUnpacker.get_device_id("TOF");
    static const int k_device_bc3  = gUnpacker.get_device_id("BC3");
    static const int k_device_bc4  = gUnpacker.get_device_id("BC4");
    static const int k_device_sdc1 = gUnpacker.get_device_id("SDC1");
    static const int k_device_sdc2 = gUnpacker.get_device_id("SDC2");
    static const int k_device_sdc3 = gUnpacker.get_device_id("SDC3");

    // sequential id
    int cor_id= gHist.getSequentialID(kCorrelation, 0, 0, 1);

    // BH1 vs BFT
    TH2* hcor_bh1bft = dynamic_cast<TH2*>(hptr_array[cor_id++]);
    for(int seg1 = 0; seg1<NumOfSegBH1; ++seg1){
      for(int seg2 = 0; seg2<NumOfSegBFT; ++seg2){
	int nhitBH1 = gUnpacker.get_entries(k_device_bh1, 0, seg1, 0, 1);
	int nhitBFT = gUnpacker.get_entries(k_device_bft, 0, 0, seg2, 0);
	if( nhitBH1 == 0 || nhitBFT == 0 ) continue;
	bool hitBFT = false;
	for( int m = 0; m<nhitBFT; ++m ){
	  int tdc = gUnpacker.get(k_device_bft, 0, 0, seg2, 0, m);
	  if( !hitBFT && tdc > 0 ) hitBFT = true;
	}
	int tdcBH1 = gUnpacker.get(k_device_bh1, 0, seg1, 0, 1);
	bool hitBH1 = ( tdcBH1 > 0 );
	if( hitBFT && hitBH1 ){
	  hcor_bh1bft->Fill(seg1, seg2);
	}
      }
    }

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

    // TOF vs SDC3
    TH2* hcor_tofsdc3 = dynamic_cast<TH2*>(hptr_array[cor_id++]);
    for( int seg=0; seg<NumOfSegTOF; ++seg ){
      for( int wire=0; wire<NumOfWireSDC3X; ++wire ){
	int hitTOF  = gUnpacker.get_entries(k_device_tof, 0, seg, 0, 1);
	int hitSDC3 = gUnpacker.get_entries(k_device_sdc3, 2, 0, wire, 0);
	if( hitTOF == 0 || hitSDC3 == 0 ) continue;
	hcor_tofsdc3->Fill( wire, seg );
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
    static const int k_tdc    = gUnpacker.get_data_id("BH1", "fpga_leading");

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

  //------------------------------------------------------------------
  // BGO
  //------------------------------------------------------------------
  Bool_t bgo_is_hit = false;
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("BGO");
    //    static const int k_adc      = gUnpacker.get_data_id("BGO", "adc");
    //    static const int k_tdc      = gUnpacker.get_data_id("BGO", "tdc");
    static const int k_fadc      = gUnpacker.get_data_id("BGO", "fadc");
    static const int k_leading   = gUnpacker.get_data_id("BGO", "leading");
//    static const int k_trailing   = gUnpacker.get_data_id("BGO", "trailing");

    // TDC gate range
    static const unsigned int tdc_min = gUser.GetParameter("BGO_TDC", 0);
    static const unsigned int tdc_max = gUser.GetParameter("BGO_TDC", 1);

    int bgo_fa_id   = gHist.getSequentialID(kBGO, 0, kFADC);
    int bgo_fawt_id = gHist.getSequentialID(kBGO, 0, kFADCwTDC);
    int bgo_a_id    = gHist.getSequentialID(kBGO, 0, kADC);
    int bgo_awt_id  = gHist.getSequentialID(kBGO, 0, kADCwTDC);
    int bgo_a2d_id  = gHist.getSequentialID(kBGO, 0, kADC2D);
    int bgo_t_id    = gHist.getSequentialID(kBGO, 0, kTDC);
    int bgo_t2d_id  = gHist.getSequentialID(kBGO, 0, kTDC2D);
    int bgo_hit_id  = gHist.getSequentialID(kBGO, 0, kHitPat,  1);
    int bgo_chit_id = gHist.getSequentialID(kBGO, 0, kHitPat,  2);
    int bgo_mul_id  = gHist.getSequentialID(kBGO, 0, kMulti,   1);
    int bgo_cmul_id = gHist.getSequentialID(kBGO, 0, kMulti,   2);
    unsigned int multiplicity  = 0;
    unsigned int cmultiplicity = 0;
    std::vector<Int_t> de_array( NumOfSegBGO );
    for(int seg=0; seg<NumOfSegBGO; ++seg){
      Int_t de  = 0;
      Int_t ped = 0;
      // FADC
      int nhit_f = gUnpacker.get_entries(k_device, 0, seg, 0, k_fadc);
      if(nhit_f==0){
	de_array.at(seg) = 0;
	continue;
      }
      for (int i=0; i<nhit_f; ++i) {
	unsigned int fadc = gUnpacker.get(k_device, 0, seg, 0, k_fadc ,i);
	if( fadc == 0xffff )
	  continue;
	if( ped == 0 )
	  ped = fadc;
	hptr_array[bgo_fa_id + seg]->Fill( i+1, fadc);
	de += ped - fadc;
      }
      hptr_array[bgo_a_id + seg]->Fill( de );
      hptr_array[bgo_a2d_id]->Fill( seg, de );
      // TDC && Hit pattern && multiplicity
      unsigned int nhit_l = gUnpacker.get_entries(k_device, 0, seg, 0, k_leading);
//      unsigned int nhit_t = gUnpacker.get_entries(k_device, 0, seg, 0, k_trailing);
//      unsigned int hit_l_max = 0;
//      unsigned int hit_t_max = 0;
//
//      if(nhit_l != 0){
//        hit_l_max = gUnpacker.get(k_device, 0, seg, 0, k_leading,  nhit_l - 1);
//      }
//      if(nhit_t != 0){
//        hit_t_max = gUnpacker.get(k_device, 0, seg, 0, k_trailing, nhit_t - 1);
//      }

      if(nhit_l!=0){
	// ADC wTDC
	for (int i=0; i<nhit_f; ++i) {
	  unsigned int fadc = gUnpacker.get(k_device, 0, seg, 0, k_fadc ,i);
	  hptr_array[bgo_fawt_id + seg]->Fill( i+1, fadc);
	}
	Bool_t is_hit = false;
        for(unsigned int m = 0; m<nhit_l; ++m){
	  unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_leading, m);
      	  if(tdc!=0){
      	    hptr_array[bgo_t_id + seg]->Fill(tdc);
      	    hptr_array[bgo_t2d_id]->Fill( seg, tdc );
	    hptr_array[bgo_hit_id]->Fill( seg );
	    ++multiplicity;
	    if(true && tdc_min < tdc && tdc < tdc_max){
	      bgo_is_hit = true;
	      is_hit = true;
              hptr_array[bgo_chit_id]->Fill(seg);
	      ++cmultiplicity;
	    }
      	  }
        }
	if( is_hit ){
	  de_array.at(seg) = de;
	  hptr_array[bgo_awt_id + seg]->Fill( de );
	  hptr_array[bgo_a2d_id + 1]->Fill( seg, de );
	}
      } else {
	de_array.at(seg) = 0;
      }
    }

    for( Int_t i=0, n=de_array.size(); i<n; ++i ){
      for( Int_t j=i+1; j<n; ++j ){
	if( de_array[i] > 0 && de_array[j] > 0 )
	  hptr_array[bgo_a2d_id+2]->Fill( de_array[i], de_array[j] );
      }
    }

    for(int seg=0; seg<NumOfSegBGO_T; ++seg){
      unsigned int nhit_l = gUnpacker.get_entries(k_device, 1, seg, 0, k_leading);
      nhit_l = gUnpacker.get_entries(k_device, 1, seg, 0, k_leading);
      if(nhit_l!=0){
        for(unsigned int m = 0; m<nhit_l; ++m){
      	unsigned int tdc = gUnpacker.get(k_device, 1, seg, 0, k_leading, m);
      	  if(tdc!=0){
      	    hptr_array[bgo_t_id + NumOfSegBGO + seg]->Fill(tdc);
      	  }
        }
      }
    }

    hptr_array[bgo_mul_id]->Fill(multiplicity);
    hptr_array[bgo_cmul_id]->Fill(cmultiplicity); // CMulti

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }// BGO

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  //------------------------------------------------------------------
  // CFT
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("CFT");
    static const int k_leading  = gUnpacker.get_data_id("CFT" , "leading");
    static const int k_trailing = gUnpacker.get_data_id("CFT", "trailing");
    static const int k_highgain = gUnpacker.get_data_id("CFT" , "highgain");
    static const int k_lowgain  = gUnpacker.get_data_id("CFT", "lowgain");
    // TDC gate range
    static const int tdc_min = gUser.GetParameter("CFT_TDC", 0);
    static const int tdc_max = gUser.GetParameter("CFT_TDC", 1);

    // SequentialID
    int cft_t_id    = gHist.getSequentialID(kCFT, 0, kTDC,     1);
    int cft_t_2d_id = gHist.getSequentialID(kCFT, 0, kTDC2D,   1);

    int cft_tot_id     = gHist.getSequentialID(kCFT, 0, kADC,    1);
    int cft_tot_2d_id  = gHist.getSequentialID(kCFT, 0, kADC2D,  1);
    int cft_ctot_id    = gHist.getSequentialID(kCFT, 0, kADC,   11);
    int cft_ctot_2d_id = gHist.getSequentialID(kCFT, 0, kADC2D, 11);

    int cft_hg_id = gHist.getSequentialID(kCFT, 0, kHighGain, 1);
    int cft_pe_id = gHist.getSequentialID(kCFT, 0, kPede    , 1);
    int cft_lg_id = gHist.getSequentialID(kCFT, 0, kLowGain,  1);

    int cft_hg_2d_id = gHist.getSequentialID(kCFT, 0, kHighGain, 11);
    int cft_pe_2d_id = gHist.getSequentialID(kCFT, 0, kPede    , 11);
    int cft_lg_2d_id = gHist.getSequentialID(kCFT, 0, kLowGain,  11);

    int cft_chg_id = gHist.getSequentialID(kCFT, 0, kHighGain, 21);
    int cft_clg_id = gHist.getSequentialID(kCFT, 0, kLowGain,  21);

    int cft_chg_2d_id = gHist.getSequentialID(kCFT, 0, kHighGain, 31);
    int cft_clg_2d_id = gHist.getSequentialID(kCFT, 0, kLowGain,  31);

    int cft_hg_tot_id = gHist.getSequentialID(kCFT, 0, kHighGain, 41);
    int cft_lg_tot_id = gHist.getSequentialID(kCFT, 0, kLowGain,  41);

    int cft_hit_id     = gHist.getSequentialID(kCFT, 0, kHitPat,  1);
    int cft_chit_id    = gHist.getSequentialID(kCFT, 0, kHitPat, 11);
    int cft_chitbgo_id = gHist.getSequentialID(kCFT, 0, kMulti,  21);
    int cft_mul_id     = gHist.getSequentialID(kCFT, 0, kMulti,   1);
    int cft_cmul_id    = gHist.getSequentialID(kCFT, 0, kMulti,  11);
    int cft_cmulbgo_id = gHist.getSequentialID(kCFT, 0, kMulti,  21);

    int multiplicity[NumOfLayersCFT] ;
    int cmultiplicity[NumOfLayersCFT];
    int cbgomultiplicity[NumOfLayersCFT];

    for(int l=0; l<NumOfLayersCFT; ++l){
      multiplicity[l]     = 0;
      cmultiplicity[l]    = 0;
      cbgomultiplicity[l] = 0;
      for(int i = 0; i<NumOfSegCFT[l]; ++i){
        int nhit_l = gUnpacker.get_entries(k_device, l, i, 0, k_leading );
        int nhit_t = gUnpacker.get_entries(k_device, l, i, 0, k_trailing );
        int hit_l_max = 0;
        int hit_t_max = 0;
        if(nhit_l==0)
	  continue;
	for(int m = 0; m<nhit_l; ++m){
	  int tdc = gUnpacker.get(k_device, l, i, 0, k_leading, m);
	  hptr_array[cft_t_id+l]->Fill(tdc);
	  hptr_array[cft_t_2d_id+l]->Fill(i, tdc);
	  hptr_array[cft_hit_id+l]->Fill(i);
	  ++multiplicity[l];
	  if(tdc_min < tdc && tdc < tdc_max){
	    ++cmultiplicity[l];
	    if( bgo_is_hit )
	      ++cbgomultiplicity[l];
	    hptr_array[cft_chit_id+l]->Fill(i);
	    if( bgo_is_hit )
	      hptr_array[cft_chitbgo_id+l]->Fill(i);
	  }
	}

	if(nhit_l != 0){
	  hit_l_max = gUnpacker.get(k_device, l, i, 0, k_leading,  nhit_l - 1);
	}
	if(nhit_t != 0){
	  hit_t_max = gUnpacker.get(k_device, l, i, 0, k_trailing, nhit_t - 1);
	}
	if(nhit_l == nhit_t && hit_l_max > hit_t_max){
	  for(int m = 0; m<nhit_l; ++m){
	    int tdc = gUnpacker.get(k_device, l, i, 0, k_leading, m);
	    int tdc_t = gUnpacker.get(k_device, l, i, 0, k_trailing, m);
	    int tot = tdc - tdc_t;
	    hptr_array[cft_tot_id+l]->Fill(tot);
	    hptr_array[cft_tot_2d_id+l]->Fill(i, tot);

	    if(tdc_min < tdc && tdc < tdc_max){
	      hptr_array[cft_ctot_id+l]->Fill(tot);
	      hptr_array[cft_ctot_2d_id+l]->Fill(i, tot);

	      if(m!= nhit_l-1 )continue;
	      for(int j=0; j<2; j++){
		if(j==0){
		  int nhit_hg = gUnpacker.get_entries(k_device, l, i, 0, k_highgain);
		  if(nhit_hg==0)continue;
		  int adc_hg = gUnpacker.get(k_device, l, i, 0, k_highgain, 0);
		  hptr_array[cft_chg_id+l]->Fill(adc_hg);
		  hptr_array[cft_chg_2d_id+l]->Fill(i, adc_hg);
		  hptr_array[cft_hg_tot_id+l]->Fill(adc_hg, tot);
		}
		if(j==1){
		  int nhit_lg = gUnpacker.get_entries(k_device, l, i, 0, k_lowgain);
		  if(nhit_lg==0)continue;
		  int adc_lg = gUnpacker.get(k_device, l, i, 0, k_lowgain, 0);
		  hptr_array[cft_clg_id+l]->Fill(adc_lg);
		  hptr_array[cft_clg_2d_id+l]->Fill(i, adc_lg);
		  hptr_array[cft_lg_tot_id+l]->Fill(adc_lg, tot);
		}
	      }
	    }
	  }
	}
      }

      hptr_array[cft_mul_id+l]->Fill(multiplicity[l]);
      hptr_array[cft_cmul_id+l]->Fill(cmultiplicity[l]);
      hptr_array[cft_cmulbgo_id+l]->Fill(cbgomultiplicity[l]);

      for(int i = 0; i<NumOfSegCFT[l]; ++i){
        int nhit_hg = gUnpacker.get_entries(k_device, l, i, 0, k_highgain);
        if(nhit_hg==0)continue;
	for(int m = 0; m<nhit_hg; ++m){
	  int adc_hg = gUnpacker.get(k_device, l, i, 0, k_highgain, m);
	  hptr_array[cft_hg_id+l]->Fill(adc_hg);
	  hptr_array[cft_hg_2d_id+l]->Fill(i, adc_hg);
	  double pedeCFT = 0.;
	  double gainCFT = 100.;
	  //                pedeCFT = hman->GetPedestal(DetIdCFT,layer,i,0);
	  //                gainCFT = hman->GetGain(DetIdCFT,layer,i,0);
	  double adc_pe = ((double)adc_hg - pedeCFT)/gainCFT;
	  hptr_array[cft_pe_id+l]->Fill(adc_pe);
	  hptr_array[cft_pe_2d_id+l]->Fill(i, adc_pe);
	}
      }
      for(int i = 0; i<NumOfSegCFT[l]; ++i){
        int nhit_lg = gUnpacker.get_entries(k_device, l, i, 0, k_lowgain );
        if(nhit_lg==0)continue;
	for(int m = 0; m<nhit_lg; ++m){
	  int adc_lg = gUnpacker.get(k_device, l, i, 0, k_lowgain, m);
	  hptr_array[cft_lg_id+l]->Fill(adc_lg);
	  hptr_array[cft_lg_2d_id+l]->Fill(i, adc_lg);
	}
      }
    }
    hptr_array[cft_mul_id+NumOfLayersCFT]->Fill(multiplicity[0] + multiplicity[2]
						+ multiplicity[4] + multiplicity[6]);
    hptr_array[cft_mul_id+NumOfLayersCFT + 1]->Fill(multiplicity[1] + multiplicity[3]
						    + multiplicity[5] + multiplicity[7]);
    hptr_array[cft_cmul_id+NumOfLayersCFT]->Fill(cmultiplicity[0] + cmultiplicity[2]
						 + cmultiplicity[4] + cmultiplicity[6]);
    hptr_array[cft_cmul_id+NumOfLayersCFT + 1]->Fill(cmultiplicity[1] + cmultiplicity[3]
						     + cmultiplicity[5] + cmultiplicity[7]);

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }//CFT

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  //------------------------------------------------------------------
  // PiID
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("PiID");
    static const int k_hg      = gUnpacker.get_data_id("PiID", "highgain");
    static const int k_lg      = gUnpacker.get_data_id("PiID", "lowgain");
    static const int k_tdc      = gUnpacker.get_data_id("PiID", "leading");

    // TDC gate range
    static const unsigned int tdc_min = gUser.GetParameter("PiID_TDC", 0);
    static const unsigned int tdc_max = gUser.GetParameter("PiID_TDC", 1);

    int piidhg_id   = gHist.getSequentialID(kPiID, 0, kHighGain);
    int piidlg_id   = gHist.getSequentialID(kPiID, 0, kLowGain);
    int piidt_id   = gHist.getSequentialID(kPiID, 0, kTDC);
    int piidhgwt_id = gHist.getSequentialID(kPiID, 0, kADCwTDC);
    int piidlgwt_id = gHist.getSequentialID(kPiID, 0, kADCwTDC,NumOfSegPiID +1);
    for(int seg=0; seg<NumOfSegPiID; ++seg){
      // High ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_hg);
      if(nhit!=0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, 0, k_hg);
	hptr_array[piidhg_id + seg]->Fill(adc);
      }

      // Low ADC
      nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_lg);
      if(nhit!=0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, 0, k_lg);
	hptr_array[piidlg_id + seg]->Fill(adc);
      }

      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
      if(nhit!=0){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc);
	if(tdc!=0){
	  hptr_array[piidt_id + seg]->Fill(tdc);
	  // ADCwTDC
	  if( gUnpacker.get_entries(k_device, 0, seg, 0, k_hg)>0 ){
	    unsigned int hadc = gUnpacker.get(k_device, 0, seg, 0, k_hg);
	    hptr_array[piidhgwt_id + seg]->Fill( hadc );
	  }
	  if( gUnpacker.get_entries(k_device, 0, seg, 0, k_lg)>0 ){
	    unsigned int ladc = gUnpacker.get(k_device, 0, seg, 0, k_lg);

	    hptr_array[piidlgwt_id + seg]->Fill( ladc );
	  }
	}
      }
    }

    // Hit pattern && multiplicity
    static const int piidhit_id = gHist.getSequentialID(kPiID, 0, kHitPat);
    static const int piidmul_id = gHist.getSequentialID(kPiID, 0, kMulti);
    int multiplicity  = 0;
    int cmultiplicity = 0;
    for(int seg=0; seg<NumOfSegPiID; ++seg){
      int nhit_piid = gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
      // AND
      if(nhit_piid!=0){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc);
	// TDC AND
	if(tdc != 0){
	  hptr_array[piidhit_id]->Fill(seg);
	  ++multiplicity;
	  // TDC range
	  if(true
	     && tdc_min < tdc && tdc < tdc_max
	     ){
	    hptr_array[piidhit_id+1]->Fill(seg); // CHitPat
	    ++cmultiplicity;
	  }// TDC range OK
	}// TDC AND
      }// AND
    }// for(seg)

    hptr_array[piidmul_id]->Fill(multiplicity);
    hptr_array[piidmul_id+1]->Fill(cmultiplicity); // CMulti

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }// PiID

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // Correlation (2D histograms) -------------------------------------
  {
    static const int k_device_cft   = gUnpacker.get_device_id("CFT");
    static const int k_device_bgo   = gUnpacker.get_device_id("BGO");
    static const int k_device_piid   = gUnpacker.get_device_id("PiID");
    static const int k_leading  = gUnpacker.get_data_id("CFT" , "leading");
//    static const int k_fadc      = gUnpacker.get_data_id("BGO", "fadc");
    static const int k_BGOleading      = gUnpacker.get_data_id("BGO", "leading");
    static const int k_PiIDleading      = gUnpacker.get_data_id("PiID", "leading");
//    int BGO_Vth = 1000;
//    static const int BGO_Vth = gUser.GetParameter("BGO_Vth", 0);

    // sequential id
    int cor_id= gHist.getSequentialID(kCorrelation_catch, 0, 0, 1);

    // CFT vs BGO
    for(int l = 0; l<NumOfLayersCFT-4; ++l){
        for(int seg1 = 0; seg1<NumOfSegCFT[l*2+1]; ++seg1){
            for(int seg2 = 0; seg2<NumOfSegBGO; ++seg2){
                int hitCFT = gUnpacker.get_entries(k_device_cft, l*2+1, seg1, 0, k_leading);
                int hitBGO = gUnpacker.get_entries(k_device_bgo, 0, seg2, 0, k_BGOleading);
                if(hitCFT == 0 || hitBGO == 0)continue;
                int tdcCFT = gUnpacker.get(k_device_cft, l*2+1, seg1, 0, k_leading);
                int tdcBGO = gUnpacker.get(k_device_bgo, 0, seg2, 0, k_BGOleading);
                if(tdcCFT != 0&&tdcBGO !=0){
                            hptr_array[cor_id + l]->Fill(seg1, seg2);
                }
            }
        }
    }

   //  BGO vs PiID
   cor_id= gHist.getSequentialID(kCorrelation_catch, 1, 0);
   for(int seg1 = 0; seg1<NumOfSegBGO; ++seg1){
       for(int seg2 = 0; seg2<NumOfSegPiID; ++seg2){
           int hitBGO = gUnpacker.get_entries(k_device_bgo, 0, seg1, 0, k_BGOleading);
           int hitPiID = gUnpacker.get_entries(k_device_piid, 0, seg2, 0, k_PiIDleading);
           if(hitBGO == 0 || hitPiID == 0)
	     continue;
           int tdcBGO = gUnpacker.get(k_device_bgo, 0, seg1, 0, k_BGOleading);
           int tdcPiID = gUnpacker.get(k_device_piid, 0, seg2, 0, k_PiIDleading);
           if(tdcBGO != 0 && tdcPiID !=0){
	     hptr_array[cor_id]->Fill(seg1, seg2);
           }
       }
   }

  }// Correlation
#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  // Efficiency
  http::UpdateBcOutEfficiency();
  http::UpdateSdcInOutEfficiency();
  http::UpdateCFTEfficiency();

  return 0;
}

}
