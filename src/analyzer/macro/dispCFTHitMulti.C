#include "HistMaker.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

//_____________________________________________________________________
void
dispCFTHitMulti( void )
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  std::vector<Int_t> id = { kHitPat, kMulti };
  for( Int_t i=0, n=id.size(); i<n; ++i ){
    TCanvas *c = (TCanvas*)gROOT->FindObject( Form("c%d", i+1) );
    c->Clear();
    c->Divide( 4, 2 );
    for( Int_t l=0; l<NumOfLayersCFT; ++l ){
      c->cd(l+1);
      TH1 *h1 = GHist::get(HistMaker::getUniqueID(kCFT, 0, id[i], l+1));
      if(!h1) continue;
      h1->Draw();
      TH1 *h2 = GHist::get(HistMaker::getUniqueID(kCFT, 0, id[i], l+11));
      if(!h2) continue;
      h2->SetLineColor(kRed);
      h2->Draw("same");
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
