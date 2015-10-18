// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispSSDHitPat()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int n_hist  = 4;
  
  int hist_id[n_hist] = {
    HistMaker::getUniqueID(kSSD0, 0, kHitPat2D, 1),
    HistMaker::getUniqueID(kSSD1, 0, kHitPat2D, 1),
    HistMaker::getUniqueID(kSSD1, 0, kHitPat2D, 2),
    HistMaker::getUniqueID(kSSD2, 0, kHitPat2D, 1)
  };
  
  // draw SSD
  TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  c->Clear();
  c->Divide(2,2);
  for(int i=0; i<n_hist; ++i){
    c->cd(i+1)->SetGrid();
    TH1 *h = (TH1*)GHist::get(hist_id[i]);
    h->Draw("colz");
    c->Update();
  }

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
