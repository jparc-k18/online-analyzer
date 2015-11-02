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

  enum eMonitor
    {
      kKbeam, kPibeam, nMonitor
    };

  TGraph  *graph[nMonitor];
  TLegend *leg;
  Color_t  color[nMonitor] = { kGreen, kBlue };

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
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();

  // Beam Monitor
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
    for(int i=0; i<nMonitor; ++i){
      c->cd()->SetGrid();
      graph[i] = new TGraph();
      graph[i]->SetTitle("Beam Monitor");
      graph[i]->SetMarkerStyle(8);
      graph[i]->SetMarkerSize(1);
      graph[i]->SetMarkerColor(color[i]);
      if(i==0) graph[i]->Draw("AP");
      else     graph[i]->Draw("P");
      graph[i]->SetPoint(1,0,0);
    }
    leg = new TLegend(0.13, 0.13, 0.37, 0.32);
    leg->SetTextSize(0.05);
    leg->SetFillColor(0);
    leg->SetBorderSize(4);
    leg->AddEntry(graph[kKbeam],  "K beam",  "p");
    leg->AddEntry(graph[kPibeam], "pi beam", "p");
    leg->Draw();
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

  {
    static const int scaler_id = g_unpacker.get_device_id("scaler");

    static const int module_id[nMonitor]  = {  1,  1 };
    static const int channel_id[nMonitor] = { 28, 29 };

    static int spill = 0;
    static double beam[nMonitor]     = {};
    static double beam_pre[nMonitor] = {};

    for(int i=0; i<nMonitor; ++i){
      int hit = g_unpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if(hit==0) continue;
      beam[i] = g_unpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if(beam[i]<beam_pre[i]){
	graph[i]->SetPoint(spill, spill, beam_pre[i]);
	graph[i]->GetYaxis()->SetRangeUser(0, 1e6);
	graph[i]->GetXaxis()->SetLimits(spill-90, spill+10);
	if(i==nMonitor-1){
	  double kpi_ratio = beam_pre[kKbeam]/beam_pre[kPibeam];
	  leg->SetHeader(Form("  K/pi : %.3lf", kpi_ratio));
	  spill++;
	}
      }
    }

    for(int i=0; i<nMonitor; ++i) beam_pre[i] = beam[i];

  }

  return 0;
}

}
