// -*- C++ -*-

// Author: Shuhei Hayakawa

#include <iostream>
#include <string>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGFileBrowser.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TString.h>

#include <THttpServer.h>
#include <TKey.h>
#include <TSystem.h>
#include <TTimeStamp.h>

#include <DAQNode.hh>
#include <filesystem_util.hh>
#include <Unpacker.hh>
#include <UnpackerManager.hh>

#include "user_analyzer.hh"

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "HistMaker.hh"
#include "Updater.hh"

#include "HttpServer.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace
  {
    HttpServer& gHttp = HttpServer::GetInstance();
  }

  enum eBeam { kBeam, kPibeam, kPbeam, nBeam };
  enum eDAQ  { kDAQEff, kL2Eff, kDuty, nDAQ };
  TString sBeam[nBeam] = { "K-Beam", "pi-Beam", "Beam" };
  TString sDAQ[nDAQ] = { "DAQ-Eff", "L2-Eff", "Duty" };

  TGraph  *g_beam[nBeam];
  TGraph  *g_daq[nDAQ];
  TLegend *leg_beam;
  TLegend *leg_daq;
  Color_t  col_beam[nBeam] = { kGreen, kBlue, kBlack };
  Color_t  col_daq[nDAQ]   = { kRed, kOrange+1, kBlue };

//____________________________________________________________________________
Int_t
process_begin( const std::vector<std::string>& argv )
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  gConfMan.InitializeParameter<DCGeomMan>("DCGEOM");
  gConfMan.InitializeParameter<DCTdcCalibMan>("TDCCALIB");
  gConfMan.InitializeParameter<DCDriftParamMan>("DRFTPM");
  if( !gConfMan.IsGood() ) return -1;
  // unpacker and all the parameter managers are initialized at this stage

  TCanvas *c = new TCanvas("beam_monitor","beam_monitor");
  c->Divide( 1, 2 );

  gHttp.SetPort(9091);
  gHttp.Open();
  gHttp.Register(c);

  Double_t legX = 0.13;
  Double_t legY = 0.13;
  Double_t legW = 0.14;
  Double_t legH = 0.26;

  // Beam Monitor
  {
    leg_beam = new TLegend( legX, legY, legX+legW, legY+legH );
    leg_beam->SetTextSize(0.05);
    leg_beam->SetFillColor(0);
    leg_beam->SetBorderSize(4);
    for( Int_t i=0; i<nBeam; ++i ){
      c->cd(1)->SetGrid();
      g_beam[i] = new TGraph();
      g_beam[i]->SetName("Beam Monitor");
      g_beam[i]->SetTitle("Beam Monitor");
      g_beam[i]->SetMarkerStyle(8);
      g_beam[i]->SetMarkerSize(1.5);
      g_beam[i]->SetMarkerColor(col_beam[i]);
      g_beam[i]->SetLineWidth(3);
      g_beam[i]->SetLineColor(col_beam[i]);
      if(i==0) g_beam[i]->Draw("AL");
      else     g_beam[i]->Draw("L");
      g_beam[i]->SetPoint(0,0,0);
      leg_beam->AddEntry( g_beam[i], sBeam[i], "P" );
    }
    leg_beam->Draw();
  }

  // DAQ Monitor
  {
    leg_daq = new TLegend( legX, legY, legX+legW, legY+legH );
    leg_daq->SetTextSize(0.05);
    leg_daq->SetFillColor(0);
    leg_daq->SetBorderSize(4);
    for( Int_t i=0; i<nDAQ; ++i ){
      c->cd(2)->SetGrid();
      g_daq[i] = new TGraph();
      g_daq[i]->SetTitle("DAQ Monitor");
      g_daq[i]->SetMarkerStyle(8);
      g_daq[i]->SetMarkerSize(1.5);
      g_daq[i]->SetMarkerColor(col_daq[i]);
      g_daq[i]->SetLineWidth(3);
      g_daq[i]->SetLineColor(col_daq[i]);
      if(i==0) g_daq[i]->Draw("AL");
      else     g_daq[i]->Draw("L");
      g_daq[i]->SetPoint(0,0,0);
      leg_daq->AddEntry( g_daq[i], sDAQ[i], "P" );
    }
    leg_daq->Draw();
  }

  gROOT->SetStyle("Modern");
  gStyle->SetTitleW(.4);
  gStyle->SetTitleH(.11);

  return 0;
}

//____________________________________________________________________________
Int_t
process_end()
{
  return 0;
}

//____________________________________________________________________________
Int_t
process_event( void )
{
  static UnpackerManager& gUnpacker = GUnpacker::get_instance();
  // static Int_t run_number = gUnpacker.get_root()->get_run_number();

  static const Int_t scaler_id = gUnpacker.get_device_id("Scaler");

  // Spill Increment
  static Int_t spill = 0;
  Bool_t spill_inc = false;
  {
    static const Int_t module_id  =  0;
    static const Int_t channel_id = 50;

    static Int_t clock     = 0;
    static Int_t clock_pre = 0;
    Int_t hit = gUnpacker.get_entries( scaler_id, module_id, 0, channel_id, 0 );
    if(hit>0){
      clock = gUnpacker.get( scaler_id, module_id, 0, channel_id, 0 );
      if( clock<clock_pre ) spill_inc = true;
    }
    clock_pre = clock;
  }

  // Beam Monitor
  {
    static const Int_t module_id[nBeam]  = {  0,  0, 0 };
    static const Int_t channel_id[nBeam] = { 34, 36, 0 };
    // static const Int_t module_id[nBeam]  = {  0, 0, 0 };
    // static const Int_t channel_id[nBeam] = {  0, 1, 2 };

    static Double_t beam[nBeam]     = {};
    static Double_t beam_pre[nBeam] = {};

    for(Int_t i=0; i<nBeam; ++i){
      Int_t hit = gUnpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if(hit==0) continue;
      beam[i] = gUnpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
    }
    if( spill_inc ){
      for(Int_t i=0; i<nBeam; ++i){
	g_beam[i]->SetPoint(spill, spill, beam_pre[i]);
	g_beam[i]->GetYaxis()->SetRangeUser(0, 1.2e6);
	g_beam[i]->GetXaxis()->SetLimits(spill-90, spill+10);
      }
      // Double_t kpi_ratio = beam_pre[kKbeam]/beam_pre[kPibeam];
      // leg_beam->SetHeader(Form("  K/pi : %.3lf", kpi_ratio));
      Double_t kpi = beam_pre[kBeam]/beam_pre[kPibeam];
      leg_beam->SetHeader( Form("  K/pi Ratio : %5.3lf",
				kpi ) );
    }
    for(Int_t i=0; i<nBeam; ++i) beam_pre[i] = beam[i];
  }

  // DAQ Monitor
  {
    enum eVal { kL1Req, kL1Acc, kL2Acc, kRealTime, kLiveTime, nVal };

    static const Int_t module_id[nVal]  = {  0,  0,  0,  0,  0 };
    static const Int_t channel_id[nVal] = { 55, 56, 62, 53, 54 };
    static Double_t val[nVal]     = {};
    static Double_t val_pre[nVal] = {};

    for(Int_t i=0; i<nVal; ++i){
      Int_t hit = gUnpacker.get_entries( scaler_id, module_id[i], 0, channel_id[i], 0 );
      if( hit==0 ) continue;
      val[i] = (Double_t)gUnpacker.get( scaler_id, module_id[i], 0, channel_id[i], 0 );
    }
    if( spill_inc ){
      Double_t daq_eff = val_pre[kL1Acc]/val_pre[kL1Req];
      Double_t l2_eff  = val_pre[kL2Acc]/val_pre[kL1Acc];
      Double_t real    = val_pre[kRealTime];
      Double_t live    = val_pre[kLiveTime];
      Double_t duty    = daq_eff/(1.-daq_eff)*(real/live-1.);
      if( TMath::IsNaN(daq_eff) )
	daq_eff = 1.;
      if( TMath::IsNaN(l2_eff) )
	l2_eff = 1.;
      if( duty>1. || TMath::IsNaN(duty) )
	duty = 1.;
      g_daq[kDAQEff]->SetPoint(spill, spill, daq_eff);
      g_daq[kDAQEff]->GetYaxis()->SetRangeUser(0, 1.05);
      g_daq[kDAQEff]->GetXaxis()->SetLimits(spill-90, spill+10);
      g_daq[kL2Eff]->SetPoint(spill, spill, l2_eff);
      g_daq[kDuty]->SetPoint(spill, spill, duty);
      leg_daq->SetHeader( Form("  DAQ Eff. : %.3lf", daq_eff));
    }
    for( Int_t i=0; i<nVal; ++i ){
      val_pre[i] = val[i];
    }
  }

  if( spill_inc ) spill++;

  return 0;
}

}
