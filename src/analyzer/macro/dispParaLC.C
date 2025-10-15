// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispParaLC( void )
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
    c->Divide(3,2);
    int adcu_id     = HistMaker::getUniqueID( kParaLC, 0, kADC );
    int adcwtdcu_id = HistMaker::getUniqueID( kParaLC, 0, kADCwTDC );
    int adcd_id     = HistMaker::getUniqueID(kParaLC, 0, kADC, 1+NumOfSegParaLC);
    int adcwtdcd_id = HistMaker::getUniqueID(kParaLC, 0, kADCwTDC, 1+NumOfSegParaLC);
    for( int i=0; i<NumOfSegParaLC; ++i ){
      // UP
      c->cd(i+1);
      gPad->SetLogy();
      TH1 *hu = (TH1*)GHist::get( adcu_id + i );
      if( !hu ) continue;
      hu->GetXaxis()->SetRangeUser( xmin, xmax );
      hu->Draw();
      TH1 *hhu = (TH1*)GHist::get( adcwtdcu_id + i );
      if( !hhu ) continue;
      hhu->GetXaxis()->SetRangeUser( 0, 4096 );
      hhu->SetLineColor( kRed );
      hhu->Draw("same");
      // DOWN
      c->cd(i+4);
      gPad->SetLogy();
      TH1 *hd = (TH1*)GHist::get( adcd_id + i );
      if( !hd ) continue;
      hd->GetXaxis()->SetRangeUser( xmin, xmax );
      hd->Draw();
      TH1 *hhd = (TH1*)GHist::get( adcwtdcd_id + i );
      if( !hhd ) continue;
      hhd->GetXaxis()->SetRangeUser( 0, 4096 );
      hhd->SetLineColor( kRed );
      hhd->Draw("same");
    }
    c->Update();
  }

  // TOF TDC gate range
  static const unsigned int tdc_min = gUser.GetParameter("TdcParaLC", 0);
  static const unsigned int tdc_max = gUser.GetParameter("TdcParaLC", 1);

  // draw TDC
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(3,2);
    int tdcu_id = HistMaker::getUniqueID( kParaLC, 0, kTDC );
    int tdcd_id = HistMaker::getUniqueID( kParaLC, 0, kTDC, 1+NumOfSegParaLC );
    for( int i=0; i<NumOfSegParaLC; ++i ){
      // UP	    
      c->cd(i+1);
      TH1 *hu = (TH1*)GHist::get( tdcu_id + i );
      hu->GetXaxis()->SetRangeUser( tdc_min, tdc_max );
      if( hu ) hu->Draw();
      // DOWN
      c->cd(i+4);
      TH1 *hd = (TH1*)GHist::get( tdcd_id + i );
      hd->GetXaxis()->SetRangeUser( tdc_min, tdc_max );
      if( hd ) hd->Draw();
    }
    c->Update();
  }

  // Performance Evaluation for Each particle 
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(3,2);
    int kau_id = HistMaker::getUniqueID( kParaLC, 0, kADCKaon );
    int pru_id = HistMaker::getUniqueID( kParaLC, 0, kADCProton );
    int piu_id = HistMaker::getUniqueID( kParaLC, 0, kADCPion );
    int kad_id = HistMaker::getUniqueID( kParaLC, 0, kADCKaon, 1+NumOfSegParaLC );
    int prd_id = HistMaker::getUniqueID( kParaLC, 0, kADCProton, 1+NumOfSegParaLC );
    int pid_id = HistMaker::getUniqueID( kParaLC, 0, kADCPion, 1+NumOfSegParaLC );
    for( int i=0; i<NumOfSegParaLC; ++i ){
      // UP
      c->cd(i+1);
      gPad->SetLogy();
      TH1 *hu = (TH1*)GHist::get( kau_id + i );
      if( !hu ) continue;
      hu->GetXaxis()->SetRangeUser( xmin, xmax );
      hu->SetLineColor( kRed );
      hu->Draw();
      TH1 *hhu = (TH1*)GHist::get( pru_id + i );
      if( !hhu ) continue;
      hhu->GetXaxis()->SetRangeUser( 0, 4096 );
      hhu->SetLineColor( kBlue );
      hhu->Draw("same");
      TH1 *hhhu = (TH1*)GHist::get( piu_id + i );
      if( !hhhu ) continue;
      hhhu->GetXaxis()->SetRangeUser( 0, 4096 );
      hhhu->SetLineColor( kGreen );
      hhhu->Draw("same");
      // DOWN
      c->cd(i+4);
      gPad->SetLogy();
      TH1 *hd = (TH1*)GHist::get( kau_id + i );
      if( !hd ) continue;
      hd->GetXaxis()->SetRangeUser( xmin, xmax );
      hd->SetLineColor( kRed );
      hd->Draw();
      TH1 *hhd = (TH1*)GHist::get( pru_id + i );
      if( !hhd ) continue;
      hhd->GetXaxis()->SetRangeUser( 0, 4096 );
      hhd->SetLineColor( kBlue );
      hhd->Draw("same");
      TH1 *hhhd = (TH1*)GHist::get( piu_id + i );
      if( !hhhd ) continue;
      hhhd->GetXaxis()->SetRangeUser( 0, 4096 );
      hhhd->SetLineColor( kGreen );
      hhhd->Draw("same");
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------

}
