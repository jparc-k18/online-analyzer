// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispMtx2D()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int n_seg_tof = 24;
  const int n_seg_sch = 64;

  // Draw Mtx2D pattern and TOFxSCH
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->cd(0);
    int mtx2d_id  = HistMaker::getUniqueID(kMisc,  kHul2D,      kHitPat2D);
    int tofsch_id = HistMaker::getUniqueID(kMtx3D, kHulTOFxSCH, kHitPat2D);
    TH2 *hmtx2d  = (TH1*)GHist::get(mtx2d_id);
    TH2 *htofsch = (TH1*)GHist::get(tofsch_id);
    
    hmtx2d->GetXaxis()->SetRangeUser(0, n_seg_sch);
    hmtx2d->GetYaxis()->SetRangeUser(0, n_seg_tof);
    hmtx2d->Draw("box");

    htofsch->GetXaxis()->SetRangeUser(0, n_seg_sch);
    htofsch->GetYaxis()->SetRangeUser(0, n_seg_tof);
    htofsch->Draw("same");

    c->Update();
  }

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
