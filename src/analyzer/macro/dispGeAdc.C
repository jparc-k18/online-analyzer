#include "DetectorID.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispGeAdc()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  ////////// Ge ADC
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kGe, 0, kADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      if( i >= 12 ) break;
      c->cd(i+1);
      // gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(base_id +i);
      // h->GetXaxis()->SetRangeUser(0.,3000.); //tmp
      h->Draw();
    }
    c->Update();
  }
  // SL5-8
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kGe, 0, kADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      // gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(base_id +i);
      // h->GetXaxis()->SetRangeUser(0.,3000.); //tmp
      h->Draw();
    }
    c->Update();
  }

  // draw ADC-2D
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(2,1);
    int hist_id[] = {
		     HistMaker::getUniqueID(kGe, 0, kADC2D,  1),
		     HistMaker::getUniqueID(kGe, 0, kHitPat, 1)
    };
    for(int i=0; i<2; i++){
      c->cd(i+1)->SetLogz(1);
      TH1 *h = (TH1*)GHist::get(hist_id[i]);
      h->Draw("colz");
      }
    c->Update();
  }

  // draw ADCwTFA
  // SL1-4
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide( 4, 3 );
    int adc_id  = HistMaker::getUniqueID(kGe, 0, kADC, 1);
    int adcwtdc_id = HistMaker::getUniqueID(kGe, 0, kADCwTDC, 1);
    for( int i=0; i<NumOfSegGe; ++i ){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( adc_id + i );
      h->GetXaxis()->SetRangeUser( 0, 8192 );
      h->Draw();
      TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
      hh->GetXaxis()->SetRangeUser( 0, 8192 );
      hh->SetLineColor( kRed );
      hh->Draw("same");
      TText *text;
      text = new TText(0.8,0.7,"w/973CRM");
      text->SetNDC(1);
      text->SetTextAlign(32);
      text->SetTextSize(0.1);
      text->Draw();
    }
    c->Update();
  }
  // SL5-8
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
    c->Clear();
    c->Divide( 4, 3 );
    int adc_id  = HistMaker::getUniqueID(kGe, 0, kADC, 1);
    int adcwtdc_id = HistMaker::getUniqueID(kGe, 0, kADCwTDC, 1);
    for( int i=0; i<NumOfSegGe; ++i ){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH1 *h = (TH1*)GHist::get( adc_id + i );
      h->GetXaxis()->SetRangeUser( 0, 8192 );
      h->Draw();
      TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
      hh->GetXaxis()->SetRangeUser( 0, 8192 );
      hh->SetLineColor( kRed );
      hh->Draw("same");
      TText *text;
      text = new TText(0.8,0.7,"w/973CRM");
      text->SetNDC(1);
      text->SetTextAlign(32);
      text->SetTextSize(0.1);
      text->Draw();
    }
    c->Update();
  }

  // // draw ADCwCRM
  // // SL1-4
  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c6");
  //   c->Clear();
  //   c->Divide( 4, 4 );
  //   int adc_id  = HistMaker::getUniqueID(kGe, 0, kADC, 1);
  //   int adcwtdc_id = HistMaker::getUniqueID(kGe, 0, kADCwTDC, NumOfSegGe+1);
  //   for( int i=0; i<NumOfSegGe; ++i ){
  //     if( i >= 16 ) break;
  //     c->cd(i+1);
  //     TH1 *h = (TH1*)GHist::get( adc_id + i );
  //     h->GetXaxis()->SetRangeUser( 0, 8192 );
  //     h->Draw();
  //     TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
  //     hh->GetXaxis()->SetRangeUser( 0, 8192 );
  //     hh->SetLineColor( kRed );
  //     hh->Draw("same");
  //     TText *text;
  //     text = new TText(0.8,0.7,"w/671CRM");
  //     text->SetNDC(1);
  //     text->SetTextAlign(32);
  //     text->SetTextSize(0.1);
  //     text->Draw();
  //   }
  //   c->Update();
  // }
  // // SL5-8
  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c7");
  //   c->Clear();
  //   c->Divide( 4, 4 );
  //   int adc_id  = HistMaker::getUniqueID(kGe, 0, kADC, 1);
  //   int adcwtdc_id = HistMaker::getUniqueID(kGe, 0, kADCwTDC, NumOfSegGe+1);
  //   for( int i=0; i<NumOfSegGe; ++i ){
  //     if( i < 16 ) continue;
  //     c->cd((i-16)+1);
  //     TH1 *h = (TH1*)GHist::get( adc_id + i );
  //     h->GetXaxis()->SetRangeUser( 0, 8192 );
  //     h->Draw();
  //     TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
  //     hh->GetXaxis()->SetRangeUser( 0, 8192 );
  //     hh->SetLineColor( kRed );
  //     hh->Draw("same");
  //     TText *text;
  //     text = new TText(0.8,0.7,"w/671CRM");
  //     text->SetNDC(1);
  //     text->SetTextAlign(32);
  //     text->SetTextSize(0.1);
  //     text->Draw();
  //   }
  //   c->Update();
  // }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
