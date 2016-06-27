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

  enum eBeam { kKbeam, kPibeam, nBeam };
  enum eDAQ  { kDAQEff, kDuty, nDAQ   };
  enum eSSD1 { kSSD1Y0, kSSD1X0, kSSD1Y1, kSSD1X1, nSSD1 };
  enum eSSD2 { kSSD2X0, kSSD2Y0, kSSD2X1, kSSD2Y1, nSSD2 };

  TGraph  *g_beam[nBeam];
  TGraph  *g_daq[nDAQ];
  TGraph  *g_ssd1[nSSD1];
  TGraph  *g_ssd2[nSSD2];
  TLegend *leg_beam;
  TLegend *leg_daq;
  TLegend *leg_ssd1;
  TLegend *leg_ssd2;
  Color_t  col_beam[nBeam] = { kGreen, kBlue };
  Color_t  col_daq[nDAQ]   = { kRed,   kBlue };
  Color_t  col_ssd1[nSSD1];
  Color_t  col_ssd2[nSSD2];

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
  c->Divide(2,2);

  double legX = 0.13;
  double legY = 0.13;
  double legW = 0.28;
  double legH = 0.25;

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
    leg_beam = new TLegend( legX, legY, legX+legW, legY+legH );
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
    leg_daq = new TLegend( legX, legY, legX+legW, legY+legH );
    leg_daq->SetTextSize(0.05);
    leg_daq->SetFillColor(0);
    leg_daq->SetBorderSize(4);
    leg_daq->AddEntry(g_daq[kDAQEff], "DAQ Eff.",    "p");
    leg_daq->AddEntry(g_daq[kDuty],   "Duty Factor", "p");
    leg_daq->Draw();
  }

  legX = 0.13;
  legY = 0.13;
  legW = 0.21;
  legH = 0.27;

  // SSD1 Multiplicity Monitor
  {
    for( int i=kSSD1Y0; i<nSSD1; ++i ){
      c->cd(3)->SetGrid();
      g_ssd1[i] = new TGraph();
      g_ssd1[i]->SetTitle("SSD1 Multiplicity Monitor");
      g_ssd1[i]->SetMarkerStyle(8);
      g_ssd1[i]->SetMarkerSize(1.);
      col_ssd1[i] = i+1;
      g_ssd1[i]->SetMarkerColor(col_ssd1[i]);
      if(i==0) g_ssd1[i]->Draw("AP");
      else     g_ssd1[i]->Draw("P");
      g_ssd1[i]->SetPoint(1,0,0);
    }
    leg_ssd1 = new TLegend( legX, legY, legX+legW, legY+legH );
    leg_ssd1->SetTextSize(0.05);
    leg_ssd1->SetFillColor(0);
    leg_ssd1->SetBorderSize(4);
    leg_ssd1->AddEntry(g_ssd1[kSSD1Y0], "SSD1-Y0", "p");
    leg_ssd1->AddEntry(g_ssd1[kSSD1X0], "SSD1-X0", "p");
    leg_ssd1->AddEntry(g_ssd1[kSSD1Y1], "SSD1-Y1", "p");
    leg_ssd1->AddEntry(g_ssd1[kSSD1X1], "SSD1-X1", "p");
    leg_ssd1->Draw();
  }

  // SSD2 Multiplicity Monitor
  {
    for( int i=kSSD2X0; i<nSSD2; ++i ){
      c->cd(4)->SetGrid();
      g_ssd2[i] = new TGraph();
      g_ssd2[i]->SetTitle("SSD2 Multiplicity Monitor");
      g_ssd2[i]->SetMarkerStyle(8);
      g_ssd2[i]->SetMarkerSize(1.);
      col_ssd2[i] = i+1;
      g_ssd2[i]->SetMarkerColor(col_ssd2[i]);
      if(i==0) g_ssd2[i]->Draw("AP");
      else     g_ssd2[i]->Draw("P");
      g_ssd2[i]->SetPoint(1,0,0);
    }
    leg_ssd2 = new TLegend( legX, legY, legX+legW, legY+legH );
    leg_ssd2->SetTextSize(0.05);
    leg_ssd2->SetFillColor(0);
    leg_ssd2->SetBorderSize(4);
    leg_ssd2->AddEntry(g_ssd2[kSSD2X0], "SSD2-X0", "p");
    leg_ssd2->AddEntry(g_ssd2[kSSD2Y0], "SSD2-Y0", "p");
    leg_ssd2->AddEntry(g_ssd2[kSSD2X1], "SSD2-X1", "p");
    leg_ssd2->AddEntry(g_ssd2[kSSD2Y1], "SSD2-Y1", "p");
    leg_ssd2->Draw();
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
    static const int module_id[2]  = { 0, 0 };
    static const int channel_id[2] = { 6, 7 };
    static double daq[2]     = {};
    static double daq_pre[2] = {};

    static const int rl_module_id[2]  = { 0, 0 };
    static const int rl_channel_id[2] = { 4, 5 };
    static double real_live[2]     = {};
    static double real_live_pre[2] = {};
    
    for(int i=0; i<2; ++i){
      int hit = g_unpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if( hit==0 ) continue;
      daq[i] = (double)g_unpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
    }
    for(int i=0; i<2; ++i){
      int hit = g_unpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if( hit==0 ) continue;
      real_live[i] = (double)g_unpacker.get( scaler_id, rl_module_id[i], 0, rl_channel_id[i], 0 );
    }
    if( spill_inc ){
      double daq_eff = daq_pre[1]/daq_pre[0];
      double duty    = 0.;
      if( 1-daq_eff==0. )
	duty = 100.;
      else
	duty = daq_eff/(1-daq_eff)*(real_live_pre[0]/real_live_pre[1] - 1);
      
      g_daq[kDAQEff]->SetPoint(spill, spill, daq_eff);
      g_daq[kDAQEff]->GetYaxis()->SetRangeUser(0, 1.0);
      g_daq[kDAQEff]->GetXaxis()->SetLimits(spill-90, spill+10);
      g_daq[kDuty]->SetPoint(spill, spill, duty);
      leg_daq->SetHeader( Form("  DAQ Eff. : %.3lf", daq_eff));
    }
    for( int i=0; i<2; ++i ){
      daq_pre[i] = daq[i];
      real_live_pre[i] = real_live[i];
    }
  }

  // SSD1 Multiplicity Monitor
  {
    static const int k_device = g_unpacker.get_device_id("SSD1");
    static const int k_adc    = g_unpacker.get_data_id("SSD1","adc");
    static double    multi[NumOfLayersSSD1] = {};
    static double    event[NumOfLayersSSD1] = {};
    for( int l=0; l<NumOfLayersSSD1; ++l ){
      int  m=0;
      for( int seg=0; seg<NumOfSegSSD1; ++seg ){
	int nhit = g_unpacker.get_entries( k_device, l, seg, 0, k_adc );
	if( nhit>0 ) ++m;
      } // for(seg)
      multi[l] += m;
      event[l]++;
    } // for(l)

    if( spill_inc ){
      static double max = 0.;
      for( int i=0; i<NumOfLayersSSD1; ++i ){
	double multiplicity = multi[i]/event[i];
	max = std::max( max, multiplicity );
	g_ssd1[i]->SetPoint( spill, spill, multiplicity );
	g_ssd1[i]->GetYaxis()->SetRangeUser( 0., max+20 );
	g_ssd1[i]->GetXaxis()->SetLimits( spill-90, spill+10 );
	multi[i] = 0.;
	event[i] = 0.;
      }
    }
  }

  // SSD2 Multiplicity Monitor
  {
    static const int k_device = g_unpacker.get_device_id("SSD2");
    static const int k_adc    = g_unpacker.get_data_id("SSD2","adc");
    static double    multi[NumOfLayersSSD2] = {};
    static double    event[NumOfLayersSSD2] = {};
    for( int l=0; l<NumOfLayersSSD2; ++l ){
      int  m=0;
      for( int seg=0; seg<NumOfSegSSD2; ++seg ){
	int nhit = g_unpacker.get_entries( k_device, l, seg, 0, k_adc );
	if( nhit>0 ) ++m;
      } // for(seg)
      multi[l] += m;
      event[l]++;
    } // for(l)

    if( spill_inc ){
      static double max = 0.;
      for( int i=0; i<NumOfLayersSSD2; ++i ){
	double multiplicity = multi[i]/event[i];
	max = std::max( max, multiplicity );
	g_ssd2[i]->SetPoint( spill, spill, multiplicity );
	g_ssd2[i]->GetYaxis()->SetRangeUser( 0., max+20 );
	g_ssd2[i]->GetXaxis()->SetLimits( spill-90, spill+10 );
	multi[i] = 0.;
	event[i] = 0.;
      }
    }
  }

  if( spill_inc ) spill++;

  return 0;
}

}
