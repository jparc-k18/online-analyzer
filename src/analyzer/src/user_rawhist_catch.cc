// -*- C++ -*-

// Author: Tomonori Takahashi
// Change 2017/09/ S.Hoshino

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>

#include <DAQNode.hh>
#include <filesystem_util.hh>
#include <Unpacker.hh>
#include <UnpackerManager.hh>

#include "Controller.hh"
#include "user_analyzer.hh"

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "GuiPs.hh"
#include "HistMaker.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "MacroBuilder.hh"
#include "MatrixParamMan.hh"
#include "MsTParamMan.hh"
#include "ProcInfo.hh"
#include "PsMaker.hh"
#include "SsdAnalyzer.hh"
#include "UserParamMan.hh"

#define DEBUG    0
#define FLAG_DAQ 1

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    const UserParamMan& gUser = UserParamMan::GetInstance();
    std::vector<TH1*> hptr_array;
    bool flag_event_cut = false;
    int event_cut_factor = 1; // for fast semi-online analysis
  }

//____________________________________________________________________________
int
process_begin( const std::vector<std::string>& argv )
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  gConfMan.InitializeParameter<HodoParamMan>("HDPRM");
  gConfMan.InitializeParameter<HodoPHCMan>("HDPHC");
  gConfMan.InitializeParameter<DCGeomMan>("DCGEOM");
  gConfMan.InitializeParameter<DCTdcCalibMan>("TDCCALIB");
  gConfMan.InitializeParameter<DCDriftParamMan>("DRFTPM");
//  gConfMan.InitializeParameter<MatrixParamMan>("MATRIX2D", "MATRIX3D");
  gConfMan.InitializeParameter<MsTParamMan>("MASS");
  gConfMan.InitializeParameter<UserParamMan>("USER");
  if( !gConfMan.IsGood() ) return -1;
  // unpacker and all the parameter managers are initialized at this stage

  if( argv.size()==4 ){
    int factor = std::strtod( argv[3].c_str(), NULL );
    if( factor!=0 ) event_cut_factor = std::abs( factor );
    flag_event_cut = true;
    std::cout << "#D Event cut flag on : factor="
	      << event_cut_factor << std::endl;
  }

  // Make tabs
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();
  TGFileBrowser *tab_hist  = gCon.makeFileBrowser("Hist");
  TGFileBrowser *tab_macro = gCon.makeFileBrowser("Macro");

  // Add macros to the Macro tab
  //tab_macro->Add(hoge());
  tab_macro->Add(macro::Get("clear_all_canvas"));
  tab_macro->Add(macro::Get("clear_canvas"));
  tab_macro->Add(macro::Get("split22"));
  tab_macro->Add(macro::Get("split32"));
  tab_macro->Add(macro::Get("split33"));
  tab_macro->Add(macro::Get("dispBGO"));
  tab_macro->Add(macro::Get("dispPiID"));
  tab_macro->Add(macro::Get("dispDAQ"));
  // tab_macro->Add(macro::Get("auto_monitor_all"));

  // Add histograms to the Hist tab
  HistMaker& gHist = HistMaker::getInstance();
  tab_hist->Add(gHist.createCFT());
  tab_hist->Add(gHist.createBGO());
  tab_hist->Add(gHist.createPiID());
  tab_hist->Add(gHist.createCorrelation_catch());

  // Set histogram pointers to the vector sequentially.
  // This vector contains both TH1 and TH2.
  // Then you need to do down cast when you use TH2.
  if(0 != gHist.setHistPtr(hptr_array)){ return -1; }

  // Users don't have to touch this section (Make Ps tab),
  // but the file path should be changed.
  // ----------------------------------------------------------
  PsMaker& gPsMaker = PsMaker::getInstance();
  std::vector<TString> detList;
  std::vector<TString> optList;
  gHist.getListOfPsFiles(detList);
  gPsMaker.getListOfOption(optList);

  hddaq::gui::GuiPs& gPsTab = hddaq::gui::GuiPs::getInstance();
  gPsTab.setFilename(Form("%s/PSFile/pro/default_catch.ps", std::getenv("HOME")));
  gPsTab.initialize(optList, detList);
  // ----------------------------------------------------------

  gStyle->SetOptStat(1110);
  gStyle->SetTitleW(.400);
  gStyle->SetTitleH(.100);
  // gStyle->SetStatW(.420);
  // gStyle->SetStatH(.350);
  gStyle->SetStatW(.320);
  gStyle->SetStatH(.250);

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
  static UnpackerManager& gUnpacker = GUnpacker::get_instance();
  static HistMaker&       gHist     = HistMaker::getInstance();
//  static MatrixParamMan&  gMatrix   = MatrixParamMan::GetInstance();
//  static MsTParamMan&     gMsT      = MsTParamMan::GetInstance();

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  const int event_number = gUnpacker.get_event_number();
  if( flag_event_cut && event_number%event_cut_factor!=0 )
    return 0;

//  // DAQ -------------------------------------------------------------
//  {
//    // node id
//    static const int k_eb      = gUnpacker.get_fe_id("k18eb");
//    static const int k_vme     = gUnpacker.get_fe_id("vme03");
//    static const int k_vmeeasiroc = gUnpacker.get_fe_id("vmeeasiroc0");
//
//    // sequential id
//    static const int eb_id      = gHist.getSequentialID(kDAQ, kEB, kHitPat);
//    static const int vme_id     = gHist.getSequentialID(kDAQ, kVME, kHitPat2D);
//    static const int vmeeasiroc_id = gHist.getSequentialID(kDAQ, kVMEEASIROC, kHitPat2D);
//
//    { // EB
//      int data_size = gUnpacker.get_node_header(k_eb, DAQNode::k_data_size);
//      hptr_array[eb_id]->Fill(data_size);
//    }
//
//    { // VME node
//      TH2* h = dynamic_cast<TH2*>(hptr_array[vme_id]);
//      for( int i=0; i<10; ++i ){
//	if( i==1 || i==5 ) continue;
//	int node_id = k_vme+i;
//	int data_size = gUnpacker.get_node_header( node_id, DAQNode::k_data_size);
//	h->Fill( i, data_size );
//      }
//    }
//
//    { // VMEEASIROC node
//      TH2* h = dynamic_cast<TH2*>(hptr_array[easiroc_id]);
//      for(int i = 0; i<20; ++i){
//	int data_size = gUnpacker.get_node_header(k_easiroc+i, DAQNode::k_data_size);
//	h->Fill( i, data_size );
//      }
//    }
//
//  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

//  if( trigger_flag[SpillEndFlag] ) return 0;

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
    int cft_t_2d_id   = gHist.getSequentialID(kCFT, 0, kTDC2D,   1);

    int cft_tot_id  = gHist.getSequentialID(kCFT, 0, kADC,     1);
    int cft_tot_2d_id = gHist.getSequentialID(kCFT, 0, kADC2D,   1);
    int cft_ctot_id  = gHist.getSequentialID(kCFT, 0, kADC,     11);
    int cft_ctot_2d_id = gHist.getSequentialID(kCFT, 0, kADC2D,   11);

    int cft_hg_id = gHist.getSequentialID(kCFT, 0, kHighGain, 1);
    int cft_pe_id = gHist.getSequentialID(kCFT, 0, kPede    , 1);
    int cft_lg_id = gHist.getSequentialID(kCFT, 0, kLowGain, 1);

    int cft_hg_2d_id = gHist.getSequentialID(kCFT, 0, kHighGain, 11);
    int cft_pe_2d_id = gHist.getSequentialID(kCFT, 0, kPede    , 11);
    int cft_lg_2d_id = gHist.getSequentialID(kCFT, 0, kLowGain, 11);

    int cft_chg_id = gHist.getSequentialID(kCFT, 0, kHighGain, 21);
    int cft_clg_id = gHist.getSequentialID(kCFT, 0, kLowGain, 21);

    int cft_chg_2d_id = gHist.getSequentialID(kCFT, 0, kHighGain, 31);
    int cft_clg_2d_id = gHist.getSequentialID(kCFT, 0, kLowGain, 31);

    int cft_hg_tot_id = gHist.getSequentialID(kCFT, 0, kHighGain, 41);
    int cft_lg_tot_id = gHist.getSequentialID(kCFT, 0, kLowGain, 41);

    int cft_hit_id  = gHist.getSequentialID(kCFT, 0, kHitPat,  1);
    int cft_chit_id = gHist.getSequentialID(kCFT, 0, kHitPat, 11);
    int cft_mul_id   = gHist.getSequentialID(kCFT, 0, kMulti,   1);
    int cft_cmul_id  = gHist.getSequentialID(kCFT, 0, kMulti,  11);

    int multiplicity[NumOfLayersCFT] ; // includes each layers.
    int cmultiplicity[NumOfLayersCFT]; // includes each layers.

    for(int l=0; l<NumOfLayersCFT; ++l){
      multiplicity[l]  = 0; // includes each layers.
      cmultiplicity[l] = 0; // includes each layers.

      for(int i = 0; i<NumOfSegCFT[l]; ++i){
        int nhit_l = gUnpacker.get_entries(k_device, l, i, 0, k_leading );
        int nhit_t = gUnpacker.get_entries(k_device, l, i, 0, k_trailing );
        int hit_l_max = 0;
        int hit_t_max = 0;

        if(nhit_l==0)continue;
	for(int m = 0; m<nhit_l; ++m){
	  int tdc = gUnpacker.get(k_device, l, i, 0, k_leading, m);
	  hptr_array[cft_t_id+l]->Fill(tdc);
	  hptr_array[cft_t_2d_id+l]->Fill(i, tdc);
	  hptr_array[cft_hit_id+l]->Fill(i);
	  ++multiplicity[l];
	  if(tdc_min < tdc && tdc < tdc_max){
	    ++cmultiplicity[l];
	    hptr_array[cft_chit_id+l]->Fill(i);
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
  // BGO
  //------------------------------------------------------------------
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

    int bgo_fa_id = gHist.getSequentialID(kBGO, 0, kFADC);
    int bgo_fawt_id = gHist.getSequentialID(kBGO, 0, kADCwTDC);
    int bgo_t_id = gHist.getSequentialID(kBGO, 0, kTDC);
    int bgo_hit_id  = gHist.getSequentialID(kBGO, 0, kHitPat,  1);
    int bgo_chit_id = gHist.getSequentialID(kBGO, 0, kHitPat,  2);
    int bgo_mul_id  = gHist.getSequentialID(kBGO, 0, kMulti,   1);
    int bgo_cmul_id = gHist.getSequentialID(kBGO, 0, kMulti,   2);
    unsigned int multiplicity  = 0;
    unsigned int cmultiplicity = 0;

    for(int seg=0; seg<NumOfSegBGO; ++seg){
      // FADC
      int nhit_f = gUnpacker.get_entries(k_device, 0, seg, 0, k_fadc);
      if(nhit_f==0) continue;
      for (int i=0; i<nhit_f; ++i) {
	unsigned int fadc = gUnpacker.get(k_device, 0, seg, 0, k_fadc ,i);
	hptr_array[bgo_fa_id + seg]->Fill( i+1, fadc);
      }


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
        for(unsigned int m = 0; m<nhit_l; ++m){
      	unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_leading, m);
      	  if(tdc!=0){
      	    hptr_array[bgo_t_id + seg]->Fill(tdc);
	    hptr_array[bgo_hit_id]->Fill(seg);
	    ++multiplicity;
	    if(true && tdc_min < tdc && tdc < tdc_max){
              hptr_array[bgo_chit_id]->Fill(seg);
	      ++cmultiplicity;
	    }

      	    // ADC wTDC
            for (int i=0; i<nhit_f; ++i) {
              unsigned int fadc = gUnpacker.get(k_device, 0, seg, 0, k_fadc ,i);
              hptr_array[bgo_fawt_id + seg]->Fill( i+1, fadc);
            }
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
  // PiID
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("PiID");
    static const int k_adc      = gUnpacker.get_data_id("PiID", "highgain");
    static const int k_tdc      = gUnpacker.get_data_id("PiID", "leading");

    // TDC gate range
    static const unsigned int tdc_min = gUser.GetParameter("PiID_TDC", 0);
    static const unsigned int tdc_max = gUser.GetParameter("PiID_TDC", 1);

    int piida_id   = gHist.getSequentialID(kPiID, 0, kADC);
    int piidt_id   = gHist.getSequentialID(kPiID, 0, kTDC);
    int piidawt_id = gHist.getSequentialID(kPiID, 0, kADCwTDC);
    for(int seg=0; seg<NumOfSegPiID; ++seg){
      // ADC
      int nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_adc);
      if(nhit!=0){
	unsigned int adc = gUnpacker.get(k_device, 0, seg, 0, k_adc);
	hptr_array[piida_id + seg]->Fill(adc);
      }

      // TDC
      nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
      if(nhit!=0){
	unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc);
	if(tdc!=0){
	  hptr_array[piidt_id + seg]->Fill(tdc);
	  // ADC wTDC
	  if( gUnpacker.get_entries(k_device, 0, seg, 0, k_adc)>0 ){
	    unsigned int adc = gUnpacker.get(k_device, 0, seg, 0, k_adc);
	    hptr_array[piidawt_id + seg]->Fill( adc );
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
    static const int k_leading  = gUnpacker.get_data_id("CFT" , "leading");
    static const int k_fadc      = gUnpacker.get_data_id("BGO", "fadc");
//    int BGO_Vth = 1000;
    static const int BGO_Vth = gUser.GetParameter("BGO_Vth", 0);

    // sequential id
    int cor_id= gHist.getSequentialID(kCorrelation_catch, 0, 0, 1);

    // CFT vs BGO
    for(int l = 0; l<NumOfLayersCFT-4; ++l){
        for(int seg1 = 0; seg1<NumOfSegCFT[l*2+1]; ++seg1){
            for(int seg2 = 0; seg2<NumOfSegBGO; ++seg2){
                int hitCFT = gUnpacker.get_entries(k_device_cft, l*2+1, seg1, 0, k_leading);
                int hitBGO = gUnpacker.get_entries(k_device_bgo, 0, seg2, 0, k_fadc   );
                if(hitCFT == 0 || hitBGO == 0)continue;
                int tdcCFT = gUnpacker.get(k_device_cft, l*2+1, seg1, 0, k_leading, 1);
                if(tdcCFT != 0){
                    for(int i=0; i<hitBGO; ++i){
                        int adcBGO = gUnpacker.get(k_device_bgo, 0, seg2, 0, k_fadc   , i);
                        if(adcBGO <= BGO_Vth){
                            hptr_array[cor_id + l]->Fill(seg1, seg2);
                            continue;
                        }
                    }
                }
            }
        }
    }

  }// Correlation 
#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  return 0;
} //process_event()


} //analyzer
