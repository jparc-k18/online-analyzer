// // Updater belongs to the namespace hddaq::gui
// using namespace hddaq::gui;

// void dispSAC()
// {
//   // You must write these lines for the thread safe
//   // ----------------------------------
//   if(Updater::isUpdating()){return;}
//   Updater::setUpdating(true);
//   // ----------------------------------

//   // TDC gate range
//   const UserParamMan& gUser = UserParamMan::GetInstance();
//   static const unsigned int tdc_min = gUser.GetParameter("SAC_TDC", 0);
//   static const unsigned int tdc_max = gUser.GetParameter("SAC_TDC", 1);

// {
//   const int n_seg_ac = 4;

//   // draw SAC ADC
//   TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
//   c->Clear();
//   c->Divide(2,2);
//   int idac[n_seg_ac] = {
//     HistMaker::getUniqueID(kSAC,  0, kADC, 1),
//     HistMaker::getUniqueID(kSAC,  0, kADC, 2),
//     HistMaker::getUniqueID(kSAC,  0, kADC, 3),
//     HistMaker::getUniqueID(kSAC,  0, kADC, 4),
//   };
//   int idac_wtdc[n_seg_ac] = {
//     HistMaker::getUniqueID(kSAC,  0, kADCwTDC, 1),
//     HistMaker::getUniqueID(kSAC,  0, kADCwTDC, 2),
//     HistMaker::getUniqueID(kSAC,  0, kADCwTDC, 3),
//     HistMaker::getUniqueID(kSAC,  0, kADCwTDC, 4),
//   };

//   for(int i = 0; i<n_seg_ac; ++i){
//     c->cd(i+1);
//     gPad->SetLogy();
//     TH1 *h = (TH1*)GHist::get(idac[i]);
//     h->GetXaxis()->SetRangeUser(0,4000);
//     h->Draw();
//     TH1 *hh = (TH1*)GHist::get(idac_wtdc[i]);
//     hh->SetLineColor( kRed );
//     hh->Draw("same");
//   }

//   c->Update();

//   c->cd(0);
// }

// {
//   const int n_seg_ac = 4;

//   // draw SAC TDC for Rooms
//   TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
//   c->Clear();
//   c->Divide(2,2);
//   int idac[n_seg_ac] = {
//     HistMaker::getUniqueID(kSAC,  0, kTDC, 1),
//     HistMaker::getUniqueID(kSAC,  0, kTDC, 2),
//     HistMaker::getUniqueID(kSAC,  0, kTDC, 3),
//     HistMaker::getUniqueID(kSAC,  0, kTDC, 4),
//   };

//   for(int i = 0; i<n_seg_ac; ++i){
//     c->cd(i+1);
//     //    gPad->SetLogy();
//     TH1 *h = (TH1*)GHist::get(idac[i]);
//     h->GetXaxis()->SetRangeUser(tdc_min-100, tdc_max+100);
//     h->Draw();
//   }

//   c->Update();

//   c->cd(0);
// }

//   // You must write these lines for the thread safe
//   // ----------------------------------
//   Updater::setUpdating(false);
//   // ----------------------------------
// }

// -*- C++ -*-
// aded Oct 2025/10/29 for E63 SAC display

#include "DetectorID.hh"

using hddaq::gui::Updater;

//_____________________________________________________________________________
void
dispSAC( void )
{
  if(Updater::isUpdating())
    return;
  Updater::setUpdating(true);

  {
    auto c1 = dynamic_cast<TCanvas*>(gROOT->FindObject("c1"));
    c1->Clear();
    c1->Divide(3, 2);
    auto adc_hid = HistMaker::getUniqueID(kSAC, 0, kADC);
    auto awt_hid = HistMaker::getUniqueID(kSAC, 0, kADCwTDC);
    auto tdc_hid = HistMaker::getUniqueID(kSAC, 0, kTDC);
    auto multi_hid = HistMaker::getUniqueID(kSAC, 0, kMulti);
    for (int i =0; i<2;++i){
      // ADC
      c1->cd(3*i+1)->SetLogy();
      auto h1 = dynamic_cast<TH1*>(GHist::get(adc_hid+i));
      // auto h1->GetXaxis()->SetRangeUser(0, 0x1000);
      h1->Draw();
      // ADCwTDC
      auto h2 = dynamic_cast<TH1*>(GHist::get(awt_hid+i));
      h2->SetLineColor(kRed+1);
      h2->Draw("same");
      // TDC
      c1->cd(3*i+2);//->SetLogy();
      auto h3 = dynamic_cast<TH1*>(GHist::get(tdc_hid+i));
      // h3->GetXaxis()->SetRangeUser(0, 2000000);
      h3->Draw();
    //   // Multiplicity
    //   c1->cd(3*i+3);
    //   auto h4 = dynamic_cast<TH1*>(GHist::get(multi_hid+i));
    //   h4->Draw();
    }
    c1->Update();
  }

  Updater::setUpdating(false);
}

