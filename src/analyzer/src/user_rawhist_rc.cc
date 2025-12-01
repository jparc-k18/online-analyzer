// -*- C++ -*-

// Author: Tomonori Takahashi
// Change 2025/11 Y.Hong

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

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

#define DEBUG 0
#define FLAG_DAQ 1

struct LUIndexer
{
  int prefix[NumOfPlaneRC + 1];
  constexpr LUIndexer() : prefix{}
  {
    prefix[0] = 0;
    for (int i = 0; i < NumOfPlaneRC; ++i)
      prefix[i + 1] = prefix[i] + NumOfUorDRC[i];
  }
  // (l,ud) -> 0-origin
  inline int idx(int l, int ud) const { return prefix[l] + ud; }
  inline int size() const { return prefix[NumOfPlaneRC]; }
};

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    const UserParamMan &gUser = UserParamMan::GetInstance();
    std::vector<TH1 *> hptr_array;
    bool flag_event_cut = false;
    int event_cut_factor = 1; // for fast semi-online analysis
  }

  //____________________________________________________________________________
  int process_begin(const std::vector<std::string> &argv)
  {
    ConfMan &gConfMan = ConfMan::GetInstance();
    gConfMan.Initialize(argv);
    gConfMan.InitializeParameter<UserParamMan>("USER");
    if (!gConfMan.IsGood())
      return -1;
    // unpacker and all the parameter managers are initialized at this stage

    if (argv.size() == 4)
    {
      int factor = std::strtod(argv[3].c_str(), NULL);
      if (factor != 0)
        event_cut_factor = std::abs(factor);
      flag_event_cut = true;
      std::cout << "#D Event cut flag on : factor="
                << event_cut_factor << std::endl;
    }

    // Make tabs
    hddaq::gui::Controller &gCon = hddaq::gui::Controller::getInstance();
    TGFileBrowser *tab_hist = gCon.makeFileBrowser("Hist");
    TGFileBrowser *tab_macro = gCon.makeFileBrowser("Macro");

    // Add macros to the Macro tab
    // tab_macro->Add(hoge());
    tab_macro->Add(macro::Get("clear_all_canvas"));
    tab_macro->Add(macro::Get("clear_canvas"));
    tab_macro->Add(macro::Get("split22"));
    tab_macro->Add(macro::Get("split32"));
    tab_macro->Add(macro::Get("split33"));
    tab_macro->Add(macro::Get("dispRcRaw"));
    tab_macro->Add(macro::Get("dispRcHitpat"));

    // Add histograms to the Hist tab
    HistMaker &gHist = HistMaker::getInstance();
    tab_hist->Add(gHist.createRC());

    // Set histogram pointers to the vector sequentially.
    // This vector contains both TH1 and TH2.
    // Then you need to do down cast when you use TH2.
    if (0 != gHist.setHistPtr(hptr_array))
    {
      return -1;
    }

    // Users don't have to touch this section (Make Ps tab),
    // but the file path should be changed.
    // ----------------------------------------------------------
    PsMaker &gPsMaker = PsMaker::getInstance();
    std::vector<TString> detList;
    std::vector<TString> optList;
    gHist.getListOfPsFiles(detList);
    gPsMaker.getListOfOption(optList);

    hddaq::gui::GuiPs &gPsTab = hddaq::gui::GuiPs::getInstance();
    gPsTab.setFilename(Form("%s/PSFile/pro/default_rc.ps", std::getenv("HOME")));
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
  int process_end(void)
  {
    hptr_array.clear();
    return 0;
  }

  //____________________________________________________________________________
  int process_event(void)
  {
    static UnpackerManager &gUnpacker = GUnpacker::get_instance();
    static HistMaker &gHist = HistMaker::getInstance();

#if DEBUG
    std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

    const int event_number = gUnpacker.get_event_number();
    if (flag_event_cut && event_number % event_cut_factor != 0)
      return 0;

    // DAQ -------------------------------------------------------------
    // {
    //   // node id
    //   static const int k_eb      = gUnpacker.get_fe_id("k18eb");
    //   static const int k_vme     = gUnpacker.get_fe_id("vme03");
    //   static const int k_rc = gUnpacker.get_fe_id("rc0");

    //   // sequential id
    //   static const int eb_id      = gHist.getSequentialID(kDAQ, kEB, kHitPat);
    //   static const int vme_id     = gHist.getSequentialID(kDAQ, kVME, kHitPat2D);
    //   static const int rc_id = gHist.getSequentialID(kDAQ, krc, kHitPat2D);

    //   { // EB
    //     int data_size = gUnpacker.get_node_header(k_eb, DAQNode::k_data_size);
    //     hptr_array[eb_id]->Fill(data_size);
    //   }

    //   { // VME node
    //     TH2* h = dynamic_cast<TH2*>(hptr_array[vme_id]);
    //     for( int i=0; i<10; ++i ){
    // 	if( i==1 || i==5 ) continue;
    // 	int node_id = k_vme+i;
    // 	int data_size = gUnpacker.get_node_header( node_id, DAQNode::k_data_size);
    // 	h->Fill( i, data_size );
    //     }
    //   }

    //   { // rc node
    //     TH2* h = dynamic_cast<TH2*>(hptr_array[easiroc_id]);
    //     for(int i = 0; i<20; ++i){
    // 	int data_size = gUnpacker.get_node_header(k_easiroc+i, DAQNode::k_data_size);
    // 	h->Fill( i, data_size );
    //     }
    //   }

    // }

#if DEBUG
    std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

    //  if( trigger_flag[SpillEndFlag] ) return 0;

    //------------------------------------------------------------------
    // RC
    //------------------------------------------------------------------
    {
      // data type
      static const int k_device = gUnpacker.get_device_id("RC");
      static const int k_leading = gUnpacker.get_data_id("RC", "leading");
      static const int k_trailing = gUnpacker.get_data_id("RC", "trailing");
      static const int k_highgain = gUnpacker.get_data_id("RC", "highgain");
      static const int k_lowgain = gUnpacker.get_data_id("RC", "lowgain");

      // SequentialID
      int rc_t_id = gHist.getSequentialID(kRC, 0, kTDC, 1);
      int rc_tot_id = gHist.getSequentialID(kRC, 0, kTOT, 1);
      int rc_ctot_id = gHist.getSequentialID(kRC, 0, kTOT, 201);
      int rc_hg_id = gHist.getSequentialID(kRC, 0, kHighGain, 1);
      int rc_lg_id = gHist.getSequentialID(kRC, 0, kLowGain, 1);
      int rc_chg_id = gHist.getSequentialID(kRC, 0, kHighGain, 201);
      int rc_clg_id = gHist.getSequentialID(kRC, 0, kLowGain, 201);

      int rc_t_2d_id = gHist.getSequentialID(kRC, 0, kTDC2D, 1);
      int rc_tot_2d_id = gHist.getSequentialID(kRC, 0, kTOT2D, 1);
      int rc_ctot_2d_id = gHist.getSequentialID(kRC, 0, kTOT2D, 101);
      int rc_hg_2d_id = gHist.getSequentialID(kRC, 0, kHighGain2D, 1);
      int rc_chg_2d_id = gHist.getSequentialID(kRC, 0, kHighGain2D, 101);
      int rc_lg_2d_id = gHist.getSequentialID(kRC, 0, kLowGain2D, 1);
      int rc_clg_2d_id = gHist.getSequentialID(kRC, 0, kLowGain2D, 101);

      int rc_hitpat_id = gHist.getSequentialID(kRC, 0, kHitPat, 1);
      int rc_chitpat_id = gHist.getSequentialID(kRC, 0, kHitPat, 101);
      int rc_chitpat_wa_id = gHist.getSequentialID(kRC, 0, kHitPat, 201);
      int rc_multihit_id = gHist.getSequentialID(kRC, 0, kMulti, 1);

      // int rc_HgXTDC_id = gHist.getSequentialID(kRC, 0, kHighGainXTDC, 1);

      // TDC gate range
      static const int tdc_min = gUser.GetParameter("TdcRC", 0);
      static const int tdc_max = gUser.GetParameter("TdcRC", 1);

      // std::cout << "debug: RC TDC cut min=" << tdc_min << " max=" << tdc_max << std::endl;

      static const LUIndexer luindex;

      for (int l = 0; l < NumOfPlaneRC; ++l)
      {
        Int_t plane = l; // 0 origin
        // Int_t tdc1st               = 0;
        // Int_t multiplicity         = 0;
        // Int_t multiplicity_wt      = 0;
        // Int_t multiplicity_ctot    = 0;
        // Int_t multiplicity_wt_ctot = 0;

        for (int seg = 0; seg < NumOfSegRC[plane]; ++seg)
        {
          for (int ud = 0; ud < NumOfUorDRC[plane]; ++ud)
          {
            const int idx = luindex.idx(plane, ud);
            { // tdc
              int nhit_l = gUnpacker.get_entries(k_device, plane, seg, ud, k_leading);
              bool flag_hit_wt = false;

              if (nhit_l != 0)
              { // Hitpat
                if (l == 0 || l == 1 || l == 5 || l == 6)
                  hptr_array[rc_hitpat_id + l]->Fill(seg);
                else
                {
                  int nhit_l_u = gUnpacker.get_entries(k_device, plane, seg, 0, k_leading);
                  int nhit_l_d = gUnpacker.get_entries(k_device, plane, seg, 1, k_leading);
                  if (nhit_l_u != 0 && nhit_l_d != 0)
                    hptr_array[rc_hitpat_id + l]->Fill(seg);
                }
              }

              for (int m = 0; m < nhit_l; ++m)
              {
                int tdc = gUnpacker.get(k_device, plane, seg, ud, k_leading, m);
                hptr_array[rc_t_2d_id + idx]->Fill(seg, tdc);
                hptr_array[rc_t_id + l]->Fill(tdc);
                if (tdc_min < tdc && tdc < tdc_max)
                { // w/ TDC cut
                  flag_hit_wt = true;
                }
              }
              if (flag_hit_wt)
              {
                // highgain w/ TDC cut
                int nhit_hg = gUnpacker.get_entries(k_device, plane, seg, ud, k_highgain);
                if (nhit_hg != 0)
                {
                  for (int m = 0; m < nhit_hg; ++m)
                  {
                    int adc_hg = gUnpacker.get(k_device, plane, seg, ud, k_highgain, m);
                    hptr_array[rc_chg_2d_id + idx]->Fill(seg, adc_hg);
                    hptr_array[rc_chg_id + l]->Fill(adc_hg);
                    // CHitpat w/ ADC cut
                    if (adc_hg > 1000)
                    {
                      if (l == 0 || l == 1 || l == 5 || l == 6)
                        hptr_array[rc_chitpat_wa_id + l]->Fill(seg);
                      else
                      {
                        int nhit_l_u = gUnpacker.get_entries(k_device, plane, seg, 0, k_leading);
                        int nhit_l_d = gUnpacker.get_entries(k_device, plane, seg, 1, k_leading);
                        if (nhit_l_u != 0 && nhit_l_d != 0)
                          hptr_array[rc_chitpat_wa_id + l]->Fill(seg);
                      }
                    }
                  }
                }

                // lowgain w/ TDC cut
                int nhit_lg = gUnpacker.get_entries(k_device, plane, seg, ud, k_lowgain);
                if (nhit_lg != 0)
                {
                  for (int m = 0; m < nhit_lg; ++m)
                  {
                    int adc_lg = gUnpacker.get(k_device, plane, seg, ud, k_lowgain, m);
                    hptr_array[rc_clg_2d_id + idx]->Fill(seg, adc_lg);
                    hptr_array[rc_clg_id + l]->Fill(adc_lg);
                  }
                }

                // Hitpat w/ TDC cut (CHitpat)
                if (l == 0 || l == 1 || l == 5 || l == 6)
                  hptr_array[rc_chitpat_id + l]->Fill(seg);
                else
                {
                  int nhit_l_u = gUnpacker.get_entries(k_device, plane, seg, 0, k_leading);
                  int nhit_l_d = gUnpacker.get_entries(k_device, plane, seg, 1, k_leading);
                  if (nhit_l_u != 0 && nhit_l_d != 0)
                    hptr_array[rc_chitpat_id + l]->Fill(seg);
                }
              }
            }

            { // tot
              int nhit_l = gUnpacker.get_entries(k_device, plane, seg, ud, k_leading);
              int nhit_t = gUnpacker.get_entries(k_device, plane, seg, ud, k_trailing);
              Int_t hit_l_max = 0;
              Int_t hit_t_max = 0;
              if (nhit_l != 0)
                hit_l_max = gUnpacker.get(k_device, plane, seg, ud, k_leading, nhit_l - 1);
              if (nhit_t != 0)
                hit_t_max = gUnpacker.get(k_device, plane, seg, ud, k_trailing, nhit_t - 1);
              // tdc1st = 0;
              if (nhit_l == nhit_t && hit_l_max > hit_t_max)
              {
                for (Int_t m = 0; m < nhit_l; ++m)
                {
                  int tdc = gUnpacker.get(k_device, plane, seg, ud, k_leading, m);
                  int tdc_t = gUnpacker.get(k_device, plane, seg, ud, k_trailing, m);
                  int tot = tdc - tdc_t;
                  hptr_array[rc_tot_2d_id + idx]->Fill(seg, tot);
                  hptr_array[rc_tot_id + l]->Fill(tot);

                  if (tdc_min < tdc && tdc < tdc_max)
                  { // TOT w/ TDC cut
                    hptr_array[rc_ctot_2d_id + idx]->Fill(seg, tot);
                    hptr_array[rc_ctot_id + l]->Fill(tot);
                  }
                }
              }
            }

            { // High gain
              int nhit_hg = gUnpacker.get_entries(k_device, plane, seg, ud, k_highgain);
              for (int m = 0; m < nhit_hg; ++m)
              {
                int adc_hg = gUnpacker.get(k_device, plane, seg, ud, k_highgain, m);
                hptr_array[rc_hg_2d_id + idx]->Fill(seg, adc_hg);
                hptr_array[rc_hg_id + l]->Fill(adc_hg);
              }
            }

            { // Low gain
              int nhit_lg = gUnpacker.get_entries(k_device, plane, seg, ud, k_lowgain);
              for (int m = 0; m < nhit_lg; ++m)
              {
                int adc_lg = gUnpacker.get(k_device, plane, seg, ud, k_lowgain, m);
                hptr_array[rc_lg_2d_id + idx]->Fill(seg, adc_lg);
                hptr_array[rc_lg_id + l]->Fill(adc_lg);
              }
            }

            // { // Hg x TDC
            //   int nhit_hg = gUnpacker.get_entries(k_device, plane, seg, ud, k_highgain);
            //   int nhit_l = gUnpacker.get_entries(k_device, plane, seg, ud, k_leading);
            //   if (nhit_hg != 0 && nhit_l != 0)
            //   {
            //     int adc_hg = gUnpacker.get(k_device, plane, seg, ud, k_highgain, 0);
            //     for (int m = 0; m < nhit_l; ++m)
            //     {
            //       int tdc = gUnpacker.get(k_device, plane, seg, ud, k_leading, m);
            //       hptr_array[rc_HgXTDC_id + l + seg + ud]->Fill(tdc, adc_hg);
            //     }
            //   }
            // }
          }
        }
      }
      { // Multiplicity of RC
        Int_t multiplicity_m = 0;
        Int_t multiplicity_p = 0;

        for (int l = 0; l < 3; ++l)
        {
          // RC-X
          Int_t plane_m = l + 2;
          Int_t plane_p = l + 7;
          for (int seg = 0; seg < NumOfSegRC[plane_m]; ++seg)
          {
            int nhit_l_u = gUnpacker.get_entries(k_device, plane_m, seg, 0, k_leading);
            int nhit_l_d = gUnpacker.get_entries(k_device, plane_m, seg, 1, k_leading);
            if (nhit_l_u != 0 && nhit_l_d != 0)
            {
              int tdc_u = gUnpacker.get(k_device, plane_m, seg, 0, k_leading, 0);
              int tdc_d = gUnpacker.get(k_device, plane_m, seg, 1, k_leading, 0);
              if (tdc_min < tdc_u && tdc_u < tdc_max &&
                  tdc_min < tdc_d && tdc_d < tdc_max)
                ++multiplicity_m;
            }
          }
          // RC+X
          for (int seg = 0; seg < NumOfSegRC[plane_p]; ++seg)
          {
            int nhit_l_u = gUnpacker.get_entries(k_device, plane_p, seg, 0, k_leading);
            int nhit_l_d = gUnpacker.get_entries(k_device, plane_p, seg, 1, k_leading);
            if (nhit_l_u != 0 && nhit_l_d != 0)
            {
              int tdc_u = gUnpacker.get(k_device, plane_p, seg, 0, k_leading, 0);
              int tdc_d = gUnpacker.get(k_device, plane_p, seg, 1, k_leading, 0);
              if (tdc_min < tdc_u && tdc_u < tdc_max &&
                  tdc_min < tdc_d && tdc_d < tdc_max)
                ++multiplicity_p;
            }
          }
        }
        hptr_array[rc_multihit_id]->Fill(multiplicity_m);
        hptr_array[rc_multihit_id + 1]->Fill(multiplicity_p);
      }

#if 0
    // Debug, dump data relating this detector
    gUnpacker.dump_data_device(k_device);
#endif
    } // rc

#if DEBUG
    std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

    return 0;
  } // process_event()

} // analyzer
