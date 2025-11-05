// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui;

//_____________________________________________________________________________
void
dispRC( void )
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  if(Updater::isUpdating())
    return;
  Updater::setUpdating(true);

  {
    auto c1 = dynamic_cast<TCanvas*>(gROOT->FindObject("c1"));
    c1->Clear();
    c1->Divide(3, 2);
    auto adc_hid = HistMaker::getUniqueID(kVMEEASIROC, 0, kHighGain);
    for (int i =0; i<4;++i){
      // HighGain2D
      c1->cd(1+i);
      auto h1 = dynamic_cast<TH2*>(GHist::get(adc_hid+i+32));
      h1->Draw();
    }
    c1->cd(5);
    auto h11 = dynamic_cast<TH2*>(GHist::get(adc_hid+36));
    h11->Draw();
    c1->Update();
  }

  {
    auto c2 = dynamic_cast<TCanvas*>(gROOT->FindObject("c2"));
    c2->Clear();
    c2->Divide(3, 2);
    auto tdc_hid = HistMaker::getUniqueID(kVMEEASIROC, 0, kTDC2D);
    for (int i =0; i<4;++i){
      // TDC2D
      c2->cd(1+i);
      auto h2 = dynamic_cast<TH2*>(GHist::get(tdc_hid+i+22));
      h2->Draw();
    }
    c2->cd(5);
    auto h21 = dynamic_cast<TH2*>(GHist::get(tdc_hid+26));
    h21->Draw();
    c2->Update();
  }

  {
    auto c3 = dynamic_cast<TCanvas*>(gROOT->FindObject("c3"));
    c3->Clear();
    c3->Divide(3, 2);
    auto tot_hid = HistMaker::getUniqueID(kVMEEASIROC, 0, kTOT2D);
    for (int i =0; i<4;++i){
      // TOT2D
      c3->cd(1+i);
      auto h3 = dynamic_cast<TH2*>(GHist::get(tot_hid+i+22));
      h3->Draw();
    }
    c3->cd(5);
    auto h31 = dynamic_cast<TH2*>(GHist::get(tot_hid+26));
    h31->Draw();
    c3->Update();
  }

  {
    auto c4 = dynamic_cast<TCanvas*>(gROOT->FindObject("c4"));
    c4->Clear();
    c4->Divide(3, 2);
    auto depth_hid = HistMaker::getUniqueID(kVMEEASIROC, 0, kMultiHitTdc);
    for (int i =0; i<4;++i){
      // MultiHit2D
      c4->cd(1+i);
      auto h4 = dynamic_cast<TH2*>(GHist::get(depth_hid+i+42));
      h4->Draw();
    }
    c4->cd(5);
    auto h41 = dynamic_cast<TH2*>(GHist::get(depth_hid+46));
    h41->Draw();
    c4->Update();
  }

  Updater::setUpdating(false);
}
