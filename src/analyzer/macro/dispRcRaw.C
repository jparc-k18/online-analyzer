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
    }

    for (int i=2; i<5; ++i)
    { // RC-X RC
      c1->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-2)); // Up
      ha_u->Draw();

      c1->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-1)); // Down
      ha_d->Draw();
    }

    for (int i=10; i<12; ++i)
    { // RC+X PD
      c1->cd(i+1);
      auto ha = dynamic_cast<TH2*>(GHist::get(adc_hid+i-2));
      ha->Draw();
    }

    for (int i=12; i<15; ++i)
    { // RC+X RC
      c1->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 12)); // Up
      ha_u->Draw();

      c1->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 13)); // Down
      ha_d->Draw();
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
    }

    for (int i=2; i<5; ++i)
    { // RC-X RC
      c2->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-2)); // Up
      ha_u->Draw();

      c2->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid+2*i-1)); // Down
      ha_d->Draw();
    }

    for (int i=10; i<12; ++i)
    { // RC+X PD
      c2->cd(i+1);
      auto ha = dynamic_cast<TH2*>(GHist::get(adc_hid+i-2));
      ha->Draw();
    }

    for (int i=12; i<15; ++i)
    { // RC+X RC
      c2->cd(i+1);
      auto ha_u = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 12)); // Up
      ha_u->Draw();

      c2->cd(i+6);
      auto ha_d = dynamic_cast<TH2*>(GHist::get(adc_hid + 2*(i - 13) + 13)); // Down
      ha_d->Draw();
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

  {
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

  Updater::setUpdating(false);
}
