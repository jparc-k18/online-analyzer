// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGFileBrowser.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TString.h>

#include "Controller.hh"
#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "ConfMan.hh"
#include "DCAnalyzer.hh"
#include "DetectorID.hh"
#include "HistMaker.hh"
#include "MacroBuilder.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  enum eBeam
    {
      kKbeam, kPibeam, nBeam
    };

  enum eDAQ
    {
      k1stReq, k1stAcc, nDAQ
    };

  TGraph  *g_beam[nBeam];
  TGraph  *g_daq[nDAQ];
  TLegend *leg_beam;
  TLegend *leg_daq;
  Color_t  col_beam[nBeam] = { kGreen, kBlue };
  Color_t  col_daq[nDAQ]   = { kBlue,  kRed };

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);
  gConfMan.initializeDCGeomMan();
  gConfMan.initializeDCTdcCalibMan();
  gConfMan.initializeDCDriftParamMan();
  if(!gConfMan.isGood()){return -1;}
  // unpacker and all the parameter managers are initialized at this stage

  // Make tabs
  hddaq::gui::Controller::getInstance();

  TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
  c->Divide(1,2);

  // Beam Monitor
  {
    //TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
    for(int i=0; i<nBeam; ++i){
      c->cd(1)->SetGrid();
      g_beam[i] = new TGraph();
      g_beam[i]->SetName("Beam Monitor");
      g_beam[i]->SetTitle("Beam Monitor");
      g_beam[i]->SetMarkerStyle(8);
      g_beam[i]->SetMarkerSize(1.5);
      g_beam[i]->SetMarkerColor(col_beam[i]);
      if(i==0) g_beam[i]->Draw("AP");
      else     g_beam[i]->Draw("P");
      g_beam[i]->SetPoint(1,0,0);
    }
    leg_beam = new TLegend(0.13, 0.13, 0.28, 0.32);
    leg_beam->SetTextSize(0.05);
    leg_beam->SetFillColor(0);
    leg_beam->SetBorderSize(4);
    leg_beam->AddEntry(g_beam[kKbeam],  "K beam",  "p");
    leg_beam->AddEntry(g_beam[kPibeam], "pi beam", "p");
    leg_beam->Draw();
  }

  // DAQ Monitor
  {
    //TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    for(int i=0; i<nDAQ; ++i){
      c->cd(2)->SetGrid();
      g_daq[i] = new TGraph();
      g_daq[i]->SetTitle("DAQ Monitor");
      g_daq[i]->SetMarkerStyle(8);
      g_daq[i]->SetMarkerSize(1.5);
      g_daq[i]->SetMarkerColor(col_daq[i]);
      if(i==0) g_daq[i]->Draw("AP");
      else     g_daq[i]->Draw("P");
      g_daq[i]->SetPoint(1,0,0);
    }
    leg_daq = new TLegend(0.13, 0.13, 0.28, 0.32);
    leg_daq->SetTextSize(0.05);
    leg_daq->SetFillColor(0);
    leg_daq->SetBorderSize(4);
    leg_daq->AddEntry(g_daq[k1stReq], "1st Request", "p");
    leg_daq->AddEntry(g_daq[k1stAcc], "1st Accept",  "p");
    leg_daq->Draw();
  }

  gROOT->SetStyle("Modern");
  gStyle->SetTitleW(.4);
  gStyle->SetTitleH(.11);

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
process_event()
{
  static UnpackerManager& g_unpacker = GUnpacker::get_instance();
  // static int run_number = g_unpacker.get_root()->get_run_number();

  static const int scaler_id = g_unpacker.get_device_id("Scaler");

  // Spill Increment
  static int spill = 0;
  bool spill_inc = false;
  {
    static const int module_id  = 0;
    static const int channel_id = 1;

    static int clock     = 0;
    static int clock_pre = 0;
    int hit = g_unpacker.get_entries( scaler_id, module_id, 0, channel_id, 0 );
    if(hit>0){
      clock = g_unpacker.get( scaler_id, module_id, 0, channel_id, 0 );
      if( clock<clock_pre ) spill_inc = true;
    }
    clock_pre = clock;
  }

  // Beam Monitor
  {
    static const int module_id[nBeam]  = {  2,  2 };
    static const int channel_id[nBeam] = { 19, 20 };

    static double beam[nBeam]     = {};
    static double beam_pre[nBeam] = {};

    for(int i=0; i<nBeam; ++i){
      int hit = g_unpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if(hit==0) continue;
      beam[i] = g_unpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
    }
    if( spill_inc ){
      for(int i=0; i<nBeam; ++i){
	g_beam[i]->SetPoint(spill, spill, beam_pre[i]);
	g_beam[i]->GetYaxis()->SetRangeUser(0, 5e5);
	g_beam[i]->GetXaxis()->SetLimits(spill-90, spill+10);
      }
      double kpi_ratio = beam_pre[kKbeam]/beam_pre[kPibeam];
      leg_beam->SetHeader(Form("  K/pi : %.3lf", kpi_ratio));
    }
    for(int i=0; i<nBeam; ++i) beam_pre[i] = beam[i];
  }

  // DAQ Monitor
  {
    static const int module_id[nDAQ]  = { 0, 0 };
    static const int channel_id[nDAQ] = { 6, 7 };

    static double daq[nDAQ]     = {};
    static double daq_pre[nDAQ] = {};

    for(int i=0; i<nDAQ; ++i){
      int hit = g_unpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if(hit==0) continue;
      daq[i] = g_unpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
    }
    if(spill_inc){
      for(int i=0; i<nDAQ; ++i){
	g_daq[i]->SetPoint(spill, spill, daq_pre[i]);
	g_daq[i]->GetYaxis()->SetRangeUser(0, 1.5e3);
	g_daq[i]->GetXaxis()->SetLimits(spill-90, spill+10);
      }
      double daq_eff = daq_pre[k1stAcc]/daq_pre[k1stReq];
      leg_daq->SetHeader( Form("  DAQ Eff. : %.3lf", daq_eff));
    }
    for(int i=0; i<nDAQ; ++i) daq_pre[i] = daq[i];
  }

  if( spill_inc ) spill++;

  return 0;
}

}
