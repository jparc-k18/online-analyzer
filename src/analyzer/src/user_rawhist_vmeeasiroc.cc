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
  gConfMan.InitializeParameter<MatrixParamMan>("MATRIX2D", "MATRIX3D");
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
  tab_hist->Add(gHist.createVMEEASIROC());
  tab_hist->Add(gHist.createBGO());
  tab_hist->Add(gHist.createPiID());

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
  gPsTab.setFilename(Form("%s/PSFile/pro/default_vmeeasiroc.ps", std::getenv("HOME")));
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
  // VMEEASIROC
  //------------------------------------------------------------------
  {
    // data type
    static const int k_device   = gUnpacker.get_device_id("VMEEASIROC");
    static const int k_leading  = gUnpacker.get_data_id("VMEEASIROC" , "leading");
    static const int k_highgain = gUnpacker.get_data_id("VMEEASIROC" , "highgain");
    static const int k_lowgain  = gUnpacker.get_data_id("VMEEASIROC", "lowgain");

    // SequentialID
    int vmeeasiroc_t_2d_id  = gHist.getSequentialID(kVMEEASIROC, 0, kTDC2D,   1);
    int vmeeasiroc_hg_2d_id = gHist.getSequentialID(kVMEEASIROC, 0, kHighGain, 11);
    int vmeeasiroc_lg_2d_id = gHist.getSequentialID(kVMEEASIROC, 0, kLowGain, 11);

    for(int l=0; l<NumOfLayersVMEEASIROC; ++l){

      for(int i = 0; i<NumOfSegVMEEASIROC; ++i){
        int nhit_l = gUnpacker.get_entries(k_device, l, 0, i, k_leading );

        if(nhit_l==0)continue;
            for(int m = 0; m<nhit_l; ++m){
              int tdc = gUnpacker.get(k_device, l, 0, i, k_leading, m);
              hptr_array[vmeeasiroc_t_2d_id+l]->Fill(i, tdc);
              }
      }

     for(int i = 0; i<NumOfSegVMEEASIROC; ++i){
        int nhit_hg = gUnpacker.get_entries(k_device, l, 0, i, k_highgain);
        if(nhit_hg==0)continue;
            for(int m = 0; m<nhit_hg; ++m){
                int adc_hg = gUnpacker.get(k_device, l, 0, i, k_highgain, m);
                hptr_array[vmeeasiroc_hg_2d_id+l]->Fill(i, adc_hg);
            }
     }

     for(int i = 0; i<NumOfSegVMEEASIROC; ++i){
        int nhit_lg = gUnpacker.get_entries(k_device, l, 0, i, k_lowgain );
        if(nhit_lg==0)continue;
            for(int m = 0; m<nhit_lg; ++m){
                int adc_lg = gUnpacker.get(k_device, l, 0, i, k_lowgain, m);
                hptr_array[vmeeasiroc_lg_2d_id+l]->Fill(i, adc_lg);
            }
     }
   }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
  }//VMEEASIROC

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

//    // TDC gate range
//    static const unsigned int tdc_min = gUser.GetParameter("BGO_TDC", 0);
//    static const unsigned int tdc_max = gUser.GetParameter("BGO_TDC", 1);

//    int bgoa_id   = gHist.getSequentialID(kBGO, 0, kADC);
//    int bgot_id   = gHist.getSequentialID(kBGO, 0, kTDC);
//    int bgoawt_id = gHist.getSequentialID(kBGO, 0, kADCwTDC);
    int bgofa_id = gHist.getSequentialID(kBGO, 0, kFADC);
    for(int seg=0; seg<NumOfSegBGO; ++seg){
//      // ADC
//      int nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_adc);
//      if(nhit!=0){
//	unsigned int adc = gUnpacker.get(k_device, 0, seg, 0, k_adc);
//	hptr_array[bgoa_id + seg]->Fill(adc);
//      }

      // FADC
      int nhit_f = gUnpacker.get_entries(k_device, 0, seg, 0, k_fadc);
      if(nhit_f==0) continue;
      for (int i=0; i<nhit_f; ++i) {
          unsigned int fadc = gUnpacker.get(k_device, 0, seg, 0, k_fadc ,i);
          hptr_array[bgofa_id + seg]->Fill( i+1, fadc);
      }


//      // TDC
//      nhit = gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
//      if(nhit!=0){
//	unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc);
//	if(tdc!=0){
//	  hptr_array[bgot_id + seg]->Fill(tdc);
//	  // ADC wTDC
//	  if( gUnpacker.get_entries(k_device, 0, seg, 0, k_adc)>0 ){
//	    unsigned int adc = gUnpacker.get(k_device, 0, seg, 0, k_adc);
//	    hptr_array[bgoawt_id + seg]->Fill( adc );
//	  }
//	}
//      }
    }
//
//    // Hit pattern && multiplicity
//    static const int bgohit_id = gHist.getSequentialID(kBGO, 0, kHitPat);
//    static const int bgomul_id = gHist.getSequentialID(kBGO, 0, kMulti);
//    int multiplicity  = 0;
//    int cmultiplicity = 0;
//    for(int seg=0; seg<NumOfSegBGO; ++seg){
//      int nhit_bgo = gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
//      // AND
//      if(nhit_bgo!=0){
//	unsigned int tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc);
//	// TDC AND
//	if(tdc != 0){
//	  hptr_array[bgohit_id]->Fill(seg);
//	  ++multiplicity;
//	  // TDC range
//	  if(true
//	     && tdc_min < tdc && tdc < tdc_max
//	     ){
//	    hptr_array[bgohit_id+1]->Fill(seg); // CHitPat
//	    ++cmultiplicity;
//	  }// TDC range OK
//	}// TDC AND
//      }// AND
//    }// for(seg)
//
//    hptr_array[bgomul_id]->Fill(multiplicity);
//    hptr_array[bgomul_id+1]->Fill(cmultiplicity); // CMulti

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

  return 0;
} //process_event()

} //analyzer
