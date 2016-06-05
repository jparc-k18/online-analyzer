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

  enum e2ndLevel
    {
      k2ndAcc, k2ndClear, n2ndLevel
    };

  TGraph  *g_beam[nBeam];
  TGraph  *g_2nd[nBeam];
  TLegend *leg_beam;
  TLegend *leg_2nd;
  Color_t  col_beam[nBeam] = { kGreen, kBlue };
  Color_t  col_2nd[nBeam]  = { kRed,   kBlue };

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
    leg_beam = new TLegend(0.13, 0.13, 0.37, 0.32);
    leg_beam->SetTextSize(0.05);
    leg_beam->SetFillColor(0);
    leg_beam->SetBorderSize(4);
    leg_beam->AddEntry(g_beam[kKbeam],  "K beam",  "p");
    leg_beam->AddEntry(g_beam[kPibeam], "pi beam", "p");
    leg_beam->Draw();
  }

  // 2nd Level Monitor
  {
    //TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    for(int i=0; i<n2ndLevel; ++i){
      c->cd(2)->SetGrid();
      g_2nd[i] = new TGraph();
      g_2nd[i]->SetTitle("Mass Trigger Monitor");
      g_2nd[i]->SetMarkerStyle(8);
      g_2nd[i]->SetMarkerSize(1.5);
      g_2nd[i]->SetMarkerColor(col_2nd[i]);
      if(i==0) g_2nd[i]->Draw("AP");
      else     g_2nd[i]->Draw("P");
      g_2nd[i]->SetPoint(1,0,0);
    }
    leg_2nd = new TLegend(0.13, 0.13, 0.37, 0.32);
    leg_2nd->SetTextSize(0.05);
    leg_2nd->SetFillColor(0);
    leg_2nd->SetBorderSize(4);
    leg_2nd->AddEntry(g_2nd[k2ndAcc],   "MsT Accept", "p");
    leg_2nd->AddEntry(g_2nd[k2ndClear], "MsT Clear",  "p");
    leg_2nd->Draw();
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

  // 2nd Level Monitor
  {
    static const int module_id[n2ndLevel]  = {  0,   0 };
    static const int channel_id[n2ndLevel] = {  12, 13 };

    static double l2[n2ndLevel]     = {};
    static double l2_pre[n2ndLevel] = {};

    for(int i=0; i<n2ndLevel; ++i){
      int hit = g_unpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if(hit==0) continue;
      l2[i] = g_unpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
    }
    if(spill_inc){
      for(int i=0; i<n2ndLevel; ++i){
	g_2nd[i]->SetPoint(spill, spill, l2_pre[i]);
	g_2nd[i]->GetYaxis()->SetRangeUser(0, 1.5e3);
	g_2nd[i]->GetXaxis()->SetLimits(spill-90, spill+10);
      }
      double accept_ratio = l2_pre[k2ndAcc]/(l2_pre[k2ndAcc]+l2_pre[k2ndClear]);
      leg_2nd->SetHeader(Form("  Acc. : %.3lf", accept_ratio));
    }
    for(int i=0; i<n2ndLevel; ++i) l2_pre[i] = l2[i];
  }

  if( spill_inc ) spill++;

  return 0;
}

}
