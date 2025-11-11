// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui;

//_____________________________________________________________________________
void
dispRcRaw( void )
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  if(Updater::isUpdating())
    return;
  Updater::setUpdating(true);

  { // Highgain2D
    auto c1 = dynamic_cast<TCanvas*>(gROOT->FindObject("c1"));
    c1->Clear();
    c1->Divide(5, 4);
    auto adc_hid = HistMaker::getUniqueID(kRC, 0, kHighGain2D, 1);

    for (int i=0; i<2; ++i)
    { // RC-X PD
      c1->cd(i+1);
      auto ha = dynamic_cast<TH2*>(GHist::get(adc_hid+i));
      ha->Draw();
      gPad->SetLogz();
    }

    for (int i=2; i<5; ++i)
    { // RC-X RC
      c1->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-2)); // Up
      ha_u->Draw();
      gPad->SetLogz();

      c1->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-1)); // Down
      ha_d->Draw();
      gPad->SetLogz();
    }

    for (int i=10; i<12; ++i)
    { // RC+X PD
      c1->cd(i+1);
      auto ha = dynamic_cast<TH2*>(GHist::get(adc_hid+i-2));
      ha->Draw();
      gPad->SetLogz();
    }

    for (int i=12; i<15; ++i)
    { // RC+X RC
      c1->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 12)); // Up
      ha_u->Draw();
      gPad->SetLogz();

      c1->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 13)); // Down
      ha_d->Draw();
      gPad->SetLogz();
    }
    c1->Update();
  }

  { // Lowgain2D
    auto c2 = dynamic_cast<TCanvas*>(gROOT->FindObject("c2"));
    c2->Clear();
    c2->Divide(5, 4);
    auto adc_hid = HistMaker::getUniqueID(kRC, 0, kLowGain2D, 1);

    for (int i=0; i<2; ++i)
    { // RC-X PD
      c2->cd(i+1);
      auto ha = dynamic_cast<TH2*>(GHist::get(adc_hid+i));
      ha->Draw();
      gPad->SetLogz();
    }

    for (int i=2; i<5; ++i)
    { // RC-X RC
      c2->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-2)); // Up
      ha_u->Draw();
      gPad->SetLogz();

      c2->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-1)); // Down
      ha_d->Draw();
      gPad->SetLogz();
    }

    for (int i=10; i<12; ++i)
    { // RC+X PD
      c2->cd(i+1);
      auto ha = dynamic_cast<TH2*>(GHist::get(adc_hid+i-2));
      ha->Draw();
      gPad->SetLogz();
    }

    for (int i=12; i<15; ++i)
    { // RC+X RC
      c2->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 12)); // Up
      ha_u->Draw();
      gPad->SetLogz();

      c2->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 13)); // Down
      ha_d->Draw();
      gPad->SetLogz();
    }
    c2->Update();
  }

  { // TDC2D
    auto c3 = dynamic_cast<TCanvas*>(gROOT->FindObject("c3"));
    c3->Clear();
    c3->Divide(5, 4);
    auto tdc_hid = HistMaker::getUniqueID(kRC, 0, kTDC2D, 1);

    for (int i=0; i<2; ++i)
    { // RC-X PD
      c3->cd(i+1);
      auto ht = dynamic_cast<TH2*>(GHist::get(tdc_hid+i));
      ht->Draw();
    }

    for (int i=2; i<5; ++i)
    { // RC-X RC
      c3->cd(i+1);
      auto ht_u = dynamic_cast<TH2*>(GHist::get(tdc_hid+2*i-2)); // Up
      ht_u->Draw();

      c3->cd(i+6);
      auto ht_d = dynamic_cast<TH2*>(GHist::get(tdc_hid+2*i-1)); // Down
      ht_d->Draw();
    }

    for (int i=10; i<12; ++i)
    { // RC+X PD
      c3->cd(i+1);
      auto ht = dynamic_cast<TH2*>(GHist::get(tdc_hid+i-2));
      ht->Draw();
    }

    for (int i=12; i<15; ++i)
    { // RC+X RC
      c3->cd(i+1);
      auto ht_u = dynamic_cast<TH2*>(GHist::get(tdc_hid + 2*(i - 13) + 12)); // Up
      ht_u->Draw();

      c3->cd(i+6);
      auto ht_d = dynamic_cast<TH2*>(GHist::get(tdc_hid + 2*(i - 13) + 13)); // Down
      ht_d->Draw();
    }
    c3->Update();
  }

  { // TOT2D
    auto c4 = dynamic_cast<TCanvas*>(gROOT->FindObject("c4"));
    c4->Clear();
    c4->Divide(5, 4);
    auto tot_hid = HistMaker::getUniqueID(kRC, 0, kTOT2D, 1);

    for (int i=0; i<2; ++i)
    { // RC-X PD
      c4->cd(i+1);
      auto ht = dynamic_cast<TH2*>(GHist::get(tot_hid+i));
      ht->Draw();
    }

    for (int i=2; i<5; ++i)
    { // RC-X RC
      c4->cd(i+1);
      auto ht_u = dynamic_cast<TH2*>(GHist::get(tot_hid+2*i-2)); // Up
      ht_u->Draw();

      c4->cd(i+6);
      auto ht_d = dynamic_cast<TH2*>(GHist::get(tot_hid+2*i-1)); // Down
      ht_d->Draw();
    }

    for (int i=10; i<12; ++i)
    { // RC+X PD
      c4->cd(i+1);
      auto ht = dynamic_cast<TH2*>(GHist::get(tot_hid+i-2));
      ht->Draw();
    }

    for (int i=12; i<15; ++i)
    { // RC+X RC
      c4->cd(i+1);
      auto ht_u = dynamic_cast<TH2*>(GHist::get(tot_hid + 2*(i - 13) + 12)); // Up
      ht_u->Draw();

      c4->cd(i+6);
      auto ht_d = dynamic_cast<TH2*>(GHist::get(tot_hid + 2*(i - 13) + 13)); // Down
      ht_d->Draw();
    }
    c4->Update();
  }

  { // TDC1D
    auto c5 = dynamic_cast<TCanvas*>(gROOT->FindObject("c5"));
    c5->Clear();
    c5->Divide(4, 3);
    auto tdc_hid = HistMaker::getUniqueID(kRC, 0, kTDC, 1);

    for (int i=0; i<5; ++i)
    {
      c5->cd(i+1);
      auto ht1m = dynamic_cast<TH1*>(GHist::get(tdc_hid + i));
      ht1m->Draw();

      c5->cd(i+7);
      auto h1tp = dynamic_cast<TH1*>(GHist::get(tdc_hid + i + 5));
      h1tp->Draw();
    }
    c5->Update();
  }

  { // TOT1D, CTOT1D
    auto c6 = dynamic_cast<TCanvas*>(gROOT->FindObject("c6"));
    c6->Clear();
    c6->Divide(4, 3);
    auto tot_hid = HistMaker::getUniqueID(kRC, 0, kTOT, 1);
    auto ctot_hid= HistMaker::getUniqueID(kRC, 0, kTOT, 201);

    for (int i=0; i<5; ++i)
    {
      c6->cd(i+1);
      auto ht1m = dynamic_cast<TH1*>(GHist::get(tot_hid + i));
      auto cht1m= dynamic_cast<TH1*>(GHist::get(ctot_hid + i));

      cht1m->SetLineColor(kRed);

      ht1m->Draw();
      cht1m->Draw("same");

      c6->cd(i+7);
      auto h1tp = dynamic_cast<TH1*>(GHist::get(tot_hid + i + 5));
      auto cht1p = dynamic_cast<TH1*>(GHist::get(ctot_hid + i + 5));

      cht1p->SetLineColor(kRed);

      h1tp->Draw();
      cht1p->Draw("same");
    }
    c6->Update();
  }

  { // HighGain1D, CHighGain1D
    auto c7 = dynamic_cast<TCanvas*>(gROOT->FindObject("c7"));
    c7->Clear();
    c7->Divide(4, 3);
    auto highgain_hid = HistMaker::getUniqueID(kRC, 0, kHighGain, 1);
    auto chighgain_hid= HistMaker::getUniqueID(kRC, 0, kHighGain, 201);

    for (int i=0; i<5; ++i)
    {
      c7->cd(i+1);
      auto ht1m = dynamic_cast<TH1*>(GHist::get(highgain_hid + i));
      auto cht1m= dynamic_cast<TH1*>(GHist::get(chighgain_hid + i));

      cht1m->SetLineColor(kRed);

      ht1m->Draw();
      cht1m->Draw("same");
      gPad->SetLogy();

      c7->cd(i+7);
      auto h1tp = dynamic_cast<TH1*>(GHist::get(highgain_hid + i + 5));
      auto cht1p = dynamic_cast<TH1*>(GHist::get(chighgain_hid + i + 5));

      cht1p->SetLineColor(kRed);

      h1tp->Draw();
      cht1p->Draw("same");
      gPad->SetLogy();
    }
    c7->Update();
  }

  { // LowGain1D, CLowGain1D
    auto c8 = dynamic_cast<TCanvas*>(gROOT->FindObject("c8"));
    c8->Clear();
    c8->Divide(4, 3);
    auto lowgain_hid = HistMaker::getUniqueID(kRC, 0, kLowGain, 1);
    auto clowgain_hid= HistMaker::getUniqueID(kRC, 0, kLowGain, 201);

    for (int i=0; i<5; ++i)
    {
      c8->cd(i+1);
      auto ht1m = dynamic_cast<TH1*>(GHist::get(lowgain_hid + i));
      auto cht1m= dynamic_cast<TH1*>(GHist::get(clowgain_hid + i));

      cht1m->SetLineColor(kRed);

      ht1m->Draw();
      cht1m->Draw("same");
      gPad->SetLogy();

      c8->cd(i+7);
      auto h1tp = dynamic_cast<TH1*>(GHist::get(lowgain_hid + i + 5));
      auto cht1p = dynamic_cast<TH1*>(GHist::get(clowgain_hid + i + 5));

      cht1p->SetLineColor(kRed);

      h1tp->Draw();
      cht1p->Draw("same");
      gPad->SetLogy();
    }
    c8->Update();
  }

  Updater::setUpdating(false);
}
