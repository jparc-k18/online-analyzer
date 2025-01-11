#include "DetectorID.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispBH2_MTHR_Fit()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  { // Draw BH2MTHR(BH2 mean timer calc. from UD high reso.)
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3,3);
    int mthr_hid = HistMaker::getUniqueID( kBH2, 0, kTDC, 40 );
    for( int i=0; i<NumOfSegBH2; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( mthr_hid + i );
      if( !h ) continue;
      Int_t max = h->GetBinCenter(h->GetMaximumBin());
      h->GetXaxis()->SetRangeUser( max-1000, max+1000);
      h->Draw();
      h->Fit("gaus", "", "", max-200, max+200);
      auto f = (TF1*)h->FindObject("gaus");
      f->SetNpx(10000);
      f->SetLineColor(kRed);
      f->SetLineWidth(2);
      f->Draw("same");

      auto sigma    = f->GetParameter(2);
      auto sigma_ps = sigma*0.93;
      TLatex *text = new TLatex();
      text->SetNDC();
      text->SetTextSize(0.07);
      text->DrawLatex(0.200, 0.700, "#sigma:");
      text->DrawLatex(0.200, 0.630, Form("%.1f[ch]", sigma));
      text->DrawLatex(0.200, 0.560, Form("(%.1f[ps])", sigma_ps));
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
