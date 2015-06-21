// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBFT()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  ////////// BFT
  {
    const int n_hist_c1 = 8;
    const int n_hist_c2 = 6;

    int bft_id_c1[n_hist_c1] = {
      HistMaker::getUniqueID(kBFT, 0, kTDC,      1),
      HistMaker::getUniqueID(kBFT, 0, kTDC,      2),
      HistMaker::getUniqueID(kBFT, 0, kADC,      1),
      HistMaker::getUniqueID(kBFT, 0, kADC,      2),
      HistMaker::getUniqueID(kBFT, 0, kTDC2D,    1),
      HistMaker::getUniqueID(kBFT, 0, kTDC2D,    2),
      HistMaker::getUniqueID(kBFT, 0, kADC2D,    1),
      HistMaker::getUniqueID(kBFT, 0, kADC2D,    2),
    };
    int bft_id_c2[n_hist_c2] = {
      HistMaker::getUniqueID(kBFT, 0, kHitPat,   1),
      HistMaker::getUniqueID(kBFT, 0, kHitPat,   2),
      HistMaker::getUniqueID(kBFT, 0, kMulti,    1),
      HistMaker::getUniqueID(kBFT, 0, kHitPat2D, 1),
      HistMaker::getUniqueID(kBFT, 0, kHitPat2D, 2),
      HistMaker::getUniqueID(kBFT, 0, kMulti2D,  1),
    };

    // draw TDC/TOT
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
      c->Clear();
      c->Divide(4,2);
      for(int i=0; i<n_hist_c1; i++){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get(bft_id_c1[i]);
	h->Draw();
      }
    }
    c->Update();

    // draw HitPat/Multi
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
      c->Clear();
      c->Divide(3,2);
      for(int i=0; i<n_hist_c2; i++){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get(bft_id_c2[i]);
	h->Draw();
      }
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
