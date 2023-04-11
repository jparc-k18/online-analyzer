// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispE72E90()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw BAC(1), SAC(2), BH2(2)
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(5,2);
    // E72BAC
    Int_t adc_id     = HistMaker::getUniqueID(kE72BAC, 0, kADC);
    //Int_t adcwtdc_id = HistMaker::getUniqueID(kE72BAC, 0, kADCwTDC);
      c->cd(1);
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id );
      //if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
     // TH1 *hh = (TH1*)GHist::get( adcwtdc_id );
     // if( !hh ) continue;
     // hh->GetXaxis()->SetRangeUser(0,2000);
     // hh->SetLineColor( kRed );
     // hh->Draw("same");
    // E90SAC
    adc_id     = HistMaker::getUniqueID(kE90SAC, 0, kADC);
    //Int_t adcwtdc_id = HistMaker::getUniqueID(kE90SAC, 0, kADCwTDC);
    for( Int_t i=0; i<NumOfSegE90SAC; ++i ){
      c->cd( 2+i );
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id + i );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
     // TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
     // if( !hh ) continue;
     // hh->GetXaxis()->SetRangeUser(0,2000);
     // hh->SetLineColor( kRed );
     // hh->Draw("same");
     }
    // E42BH2
    adc_id     = HistMaker::getUniqueID(kE42BH2, 0, kADC);
    //Int_t adcwtdc_id = HistMaker::getUniqueID(kE42BH2, 0, kADCwTDC);
    for( Int_t i=0; i<2; ++i ){
      c->cd( 4+i );
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id + i );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
     // TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
     // if( !hh ) continue;
     // hh->GetXaxis()->SetRangeUser(0,2000);
     // hh->SetLineColor( kRed );
     // hh->Draw("same");
    c->Update();
  }
}

  // KVC(12)
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(4,3);
    Int_t adc_id  = HistMaker::getUniqueID(kE72KVC, 0, kADC);
    //Int_t adcwtdc_id = HistMaker::getUniqueID(kE72KVC, 0, kADCwTDC, 1+NumOfSegE72KVC);
    for( Int_t j=0; j<3; ++j ){
    for( Int_t i=0; i<NumOfSegE72KVC; ++i ){
      c->cd(j*NumOfSegE72KVC+i+1);
      gPad->SetLogy();
      TH1 *h = GHist::get( adc_id + i + j*NumOfSegE72KVC );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
//      TH1 *hh = GHist::get( adcwtdc_id + i );
//      if( !hh ) continue;
//      hh->GetXaxis()->SetRangeUser(0,2000);
//      hh->SetLineColor( kRed );
//      hh->Draw("same");
    }
}
    c->Update();
  }
#if 0
  // TDC gate range
  static const unsigned int tdc_min = gUser.GetParameter("TdcBH1", 0);
  static const unsigned int tdc_max = gUser.GetParameter("TdcBH1", 1);

  // draw TDC U
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(4,3);
    Int_t tdc_id = HistMaker::getUniqueID(kBH1, 0, kTDC);
    for( Int_t i=0; i<NumOfSegBH1; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_id + i );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
      h->Draw();
    }
    c->Update();
  }

  // draw TDC D
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide(4,3);
    Int_t tdc_id = HistMaker::getUniqueID(kBH1, 0, kTDC, 1+NumOfSegBH1);
    for( Int_t i=0; i<NumOfSegBH1; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_id + i);
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
      h->Draw();
    }
    c->Update();
  }

#endif
  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
