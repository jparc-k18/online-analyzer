// // Updater belongs to the namespace hddaq::gui
// using namespace hddaq::gui;

#include "UserParamMan.hh"
#include "DetectorID.hh"

using hddaq::gui::Updater;

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
    for (int i =0; i<1;++i){
      // ADC
      c1->cd(3*i+1)->SetLogy();
      auto h1 = dynamic_cast<TH1*>(GHist::get(adc_hid+i));
      // std::cout << Form("accessing hist: %d", adc_hid+i) << std::endl;
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
      c1->cd(3*i+3);
      auto h4 = dynamic_cast<TH1*>(GHist::get(multi_hid+i));
      h4->Draw();
    }
    c1->Update();
  }

  Updater::setUpdating(false);
}
