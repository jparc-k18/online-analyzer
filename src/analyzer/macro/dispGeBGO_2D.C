// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispGeBGO_2D()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  ////////// Ge BGO 2D hitpat
  {

    gStyle->SetOptStat(0);
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    int base_id = HistMaker::getUniqueID(kGe, 0, kHitPat, 2);
    c->cd(1);
      TH1 *h = (TH1*)GHist::get(base_id);
      h->Draw("colz");
      c->Update();
  }
    // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
