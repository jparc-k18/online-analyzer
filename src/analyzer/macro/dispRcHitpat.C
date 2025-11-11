// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui;

//_____________________________________________________________________________
void
dispRcHitpat( void )
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  if(Updater::isUpdating())
    return;
  Updater::setUpdating(true);

  { // PD hitpat
    auto c1 = dynamic_cast<TCanvas*>(gROOT->FindObject("c1"));
    c1->Clear();
    c1->Divide(2, 2);
    auto hitpat_hid       = HistMaker::getUniqueID(kRC, 0, kHitPat, 1);

    for (int i = 0; i<2; ++i){
      // RC-X
      c1->cd(1+i);
      auto hm   = dynamic_cast<TH1*>(GHist::get(hitpat_hid+i));

      hm->Draw();

      c1->cd(3+i);
      auto hp   = dynamic_cast<TH1*>(GHist::get(hitpat_hid+i+5));

      hp->Draw();
    }
    c1->Update();
  }

  { // PD hitpat (after cut)
    auto c3 = dynamic_cast<TCanvas*>(gROOT->FindObject("c3"));
    c3->Clear();
    c3->Divide(2, 2);
    auto chitpat_hid      = HistMaker::getUniqueID(kRC, 0, kHitPat, 101);
    auto chitpat_wa_hid   = HistMaker::getUniqueID(kRC, 0, kHitPat, 201);

    for (int i = 0; i<2; ++i){
      // RC-X
      c3->cd(1+i);
      auto hwtm = dynamic_cast<TH1*>(GHist::get(chitpat_hid+i));
      auto hwam = dynamic_cast<TH1*>(GHist::get(chitpat_wa_hid+i));

      hwtm->SetLineColor(kRed);
      hwam->SetLineColor(kBlue);

      hwtm->Draw();
      hwam->Draw("same");

      c3->cd(3+i);
      auto hwtp = dynamic_cast<TH1*>(GHist::get(chitpat_hid+i+5));
      auto hwap = dynamic_cast<TH1*>(GHist::get(chitpat_wa_hid+i+5));

      hwtp->SetLineColor(kRed);
      hwap->SetLineColor(kBlue);

      hwtp->Draw();
      hwap->Draw("same");
    }
    c3->Update();
  }

  { // RC hitpat
    auto c2 = dynamic_cast<TCanvas*>(gROOT->FindObject("c2"));
    c2->Clear();
    c2->Divide(3, 2);
    auto hitpat_hid       = HistMaker::getUniqueID(kRC, 0, kHitPat, 1);

    for (int i = 0; i<3; ++i){
      // RC-X
      c2->cd(1+i);
      auto hm   = dynamic_cast<TH1*>(GHist::get(hitpat_hid+i+2));

      hm->Draw();

      c2->cd(4+i);
      auto hp   = dynamic_cast<TH1*>(GHist::get(hitpat_hid+i+7));

      hp->Draw();
    }
    c2->Update();
  }

  { // RC hitpat (after cut)
    auto c4 = dynamic_cast<TCanvas*>(gROOT->FindObject("c4"));
    c4->Clear();
    c4->Divide(3, 2);
    auto chitpat_hid      = HistMaker::getUniqueID(kRC, 0, kHitPat, 101);
    auto chitpat_wa_hid   = HistMaker::getUniqueID(kRC, 0, kHitPat, 201);

    for (int i = 0; i<3; ++i){
      // RC-X
      c4->cd(1+i);
      auto hwtm = dynamic_cast<TH1*>(GHist::get(chitpat_hid+i+2));
      auto hwam = dynamic_cast<TH1*>(GHist::get(chitpat_wa_hid+i+2));

      hwtm->SetLineColor(kRed);
      hwam->SetLineColor(kBlue);

      hwtm->Draw();
      hwam->Draw("same");

      c4->cd(4+i);
      auto hwtp = dynamic_cast<TH1*>(GHist::get(chitpat_hid+i+7));
      auto hwap = dynamic_cast<TH1*>(GHist::get(chitpat_wa_hid+i+7));

      hwtp->SetLineColor(kRed);
      hwap->SetLineColor(kBlue);

      hwtp->Draw();
      hwap->Draw("same");
    }
    c4->Update();
  }

  { // RC Multiplicity
    auto c5 = dynamic_cast<TCanvas*>(gROOT->FindObject("c5"));
    c5->Clear();
    c5->Divide(1, 2);
    auto multi_hid = HistMaker::getUniqueID(kRC, 0, kMulti, 1);
    for (int i =0; i<2;++i){
      c5->cd(1+i);
      auto hmul = dynamic_cast<TH1*>(GHist::get(multi_hid+i));
      hmul->Draw();
      gPad->SetLogy();
    }
    c5->Update();
  }

  Updater::setUpdating(false);
}
