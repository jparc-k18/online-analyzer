// -*- C++ -*-

// Author: Shuhei Hayakawa

#include <iostream>
#include <string>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TString.h>

#include "Controller.hh"
#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "ConfMan.hh"
#include "DetectorID.hh"
#include "HistMaker.hh"
#include "Updater.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    std::vector<TH1*> hptr_array;
  }

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);
  if(!gConfMan.isGood()){return -1;}
  // unpacker and all the parameter managers are initialized at this stage

  // Make tabs
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();
  TGFileBrowser *tab_emc  = gCon.makeFileBrowser("EMC");
  HistMaker& gHist = HistMaker::getInstance();
  tab_emc->Add(gHist.createEMC());
  if(0 != gHist.setHistPtr(hptr_array)){ return -1; }

  gStyle->SetOptStat(10);
  gStyle->SetStatX(.900);
  gStyle->SetStatY(.900);
  gStyle->SetStatW(.250);
  gStyle->SetStatH(.200);
  gStyle->SetTitleW(.400);
  gStyle->SetTitleH(.090);

  TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
  c->cd();
  hptr_array[gHist.getSequentialID(kEMC, 0, kXYpos)]->Draw("colz");
  c->Update();


  gui::Updater::getInstance().setUpdateMode(gui::Updater::k_seconds);
  gui::Updater::getInstance().setInterval(2);

  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  return 0;
}

//____________________________________________________________________________
int
process_event( void )
{
  static UnpackerManager& gUnpacker = GUnpacker::get_instance();
  static HistMaker&       gHist     = HistMaker::getInstance();
  // static int run_number = g_unpacker.get_root()->get_run_number();

  // EMC -----------------------------------------------------------
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("EMC");
    static const int k_xpos   = gUnpacker.get_data_id("EMC", "xpos");
    static const int k_ypos   = gUnpacker.get_data_id("EMC", "ypos");
    // sequential id
    static const int xpos_id  = gHist.getSequentialID(kEMC, 0, kXpos);
    static const int ypos_id  = gHist.getSequentialID(kEMC, 0, kYpos);
    static const int xypos_id = gHist.getSequentialID(kEMC, 0, kXYpos);

    for(int seg=0; seg<NumOfSegEMC; ++seg){
      double xpos = 0;
      double ypos = 0;
      // Xpos
      int xpos_nhit = gUnpacker.get_entries(k_device, 0, 0, 0, k_xpos);
      if(xpos_nhit != 0){
	xpos = gUnpacker.get(k_device, 0, 0, 0, k_xpos);
	xpos = 500. - ( xpos / 1000. );
	hptr_array[xpos_id + seg]->Fill(xpos);
      }
      // Ypos
      int ypos_nhit = gUnpacker.get_entries(k_device, 0, 0, 0, k_ypos);
      if(ypos_nhit != 0){
	ypos = gUnpacker.get(k_device, 0, 0, 0, k_ypos);
	ypos = 500. - ( ypos / 1000. );
	hptr_array[ypos_id + seg]->Fill(ypos);
      }
      // XYpos
      if(xpos_nhit !=0 && ypos_nhit != 0){
	hptr_array[xypos_id + seg]->Fill(xpos, ypos);
      }
    }
  }

  return 0;
}

}
