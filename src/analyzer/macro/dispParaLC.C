// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

void dispParaLC( )
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const Double_t xmin =    0.;
  const Double_t xmax = 4096.;

  // draw ADC 
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,2);
    int adc_id     = HistMaker::getUniqueID( kParaLC, 0, kADC );
    int adcwtdc_id = HistMaker::getUniqueID( kParaLC, 0, kADCwTDC );
    int adcref_id     = HistMaker::getUniqueID( kParaLCRef, 0, kADC );
    int adcwtdcref_id = HistMaker::getUniqueID( kParaLCRef, 0, kADCwTDC );
    // LC
    for( int i=0; i < NumOfSegParaLC; ++i ){
      // UP
      c->cd(i+1);
      gPad->SetLogy();
      TH1 *hu = (TH1*)GHist::get( adc_id + i );
      if( !hu ) continue;
      hu->GetXaxis()->SetRangeUser( xmin, xmax );
      hu->Draw();
      TH1 *hhu = (TH1*)GHist::get( adcwtdc_id + i );
      if( !hhu ) continue;
      hhu->GetXaxis()->SetRangeUser( 0, 4096 );
      hhu->SetLineColor( kRed );
      hhu->Draw("same");
      // DOWN
      c->cd(i+1+NumOfSegParaLC+1);
      gPad->SetLogy();
      TH1 *hd = (TH1*)GHist::get( adc_id + i + NumOfSegParaLC );
      if( !hd ) continue;
      hd->GetXaxis()->SetRangeUser( xmin, xmax );
      hd->Draw();
      TH1 *hhd = (TH1*)GHist::get( adcwtdc_id + i+ NumOfSegParaLC );
      if( !hhd ) continue;
      hhd->GetXaxis()->SetRangeUser( 0, 4096 );
      hhd->SetLineColor( kRed );
      hhd->Draw("same");
    }//LC
    c->Update();
    //Ref
    //UP
    c->cd(4);
    gPad->SetLogy();
    TH1 *hu = (TH1*)GHist::get( adcref_id );
    hu->GetXaxis()->SetRangeUser( xmin, xmax );
    hu->Draw();
    TH1 *hhu = (TH1*)GHist::get( adcwtdcref_id );
    hhu->GetXaxis()->SetRangeUser( 0, 4096 );
    hhu->SetLineColor( kRed );
    hhu->Draw("same");
    c->Update();
    // DOWN
    c->cd(8);
    gPad->SetLogy();
    TH1 *hd = (TH1*)GHist::get( adcref_id + NumOfSegParaLCRef );
    hd->GetXaxis()->SetRangeUser( xmin, xmax );
    hd->Draw();
    TH1 *hhd = (TH1*)GHist::get( adcwtdcref_id + NumOfSegParaLCRef );
    hhd->GetXaxis()->SetRangeUser( 0, 4096 );
    hhd->SetLineColor( kRed );
    hhd->Draw("same");
    c->Update();
  }

  // TOF TDC gate range
  static const unsigned int tdc_min = gUser.GetParameter("TdcParaLC", 0);
  static const unsigned int tdc_max = gUser.GetParameter("TdcParaLC", 1);
  static const unsigned int tdcref_min = gUser.GetParameter("TdcParaLCRef", 0);
  static const unsigned int tdcref_max = gUser.GetParameter("TdcParaLCRef", 1);

  // draw TDC
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(5,2);
    int tdc_id = HistMaker::getUniqueID( kParaLC, 0, kTDC );
    int tdcref_id = HistMaker::getUniqueID( kParaLCRef, 0, kTDC );
    for( int i=0; i < NumOfSegParaLC; ++i ){
      // UP         
      c->cd(i+1);
      TH1 *hu = (TH1*)GHist::get( tdc_id + i );
      hu->GetXaxis()->SetRangeUser( tdc_min, tdc_max );
      if( hu ) hu->Draw();
      // DOWN
      c->cd(i+1+2+NumOfSegParaLC);
      TH1 *hd = (TH1*)GHist::get( tdc_id + i + NumOfSegParaLC);
      hd->GetXaxis()->SetRangeUser( tdc_min, tdc_max );
      if( hd ) hd->Draw();
    }
    c->Update();

    //Ref
    // UP         
    c->cd(4);
    TH1 *hu = (TH1*)GHist::get( tdcref_id );
    hu->GetXaxis()->SetRangeUser( tdcref_min, tdcref_max );
    if( hu ) hu->Draw();
    c->Update();
    // DOWN
    c->cd(9);
    TH1 *hd = (TH1*)GHist::get( tdcref_id + NumOfSegParaLCRef );
    hd->GetXaxis()->SetRangeUser( tdcref_min, tdcref_max );
    if( hd ) hd->Draw();
    c->Update();
    // OR
    c->cd(10);
    TH1 *h = (TH1*)GHist::get( tdcref_id + NumOfSegParaLCRef + 1);
    hd->GetXaxis()->SetRangeUser( tdcref_min, tdcref_max );
    if( h ) h->Draw();
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------

}


