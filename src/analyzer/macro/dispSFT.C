// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispSFT()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  ////////// SFT
  {
    const int n_hist_c1 = 8;
    const int n_hist_c2 = 8;
    const int n_hist_c3 = 8;
    const int n_hist_c4 = 6;
    const int n_hist_c5 = 8;

    int sft_id_c1[n_hist_c1] = {
      HistMaker::getUniqueID(kSFT, 0, kTDC,     1),
      HistMaker::getUniqueID(kSFT, 0, kTDC,     2),
      HistMaker::getUniqueID(kSFT, 0, kTDC,     3),
      HistMaker::getUniqueID(kSFT, 0, kTDC,     4),
      HistMaker::getUniqueID(kSFT, 0, kTDC,    11),
      HistMaker::getUniqueID(kSFT, 0, kTDC,    12),
      HistMaker::getUniqueID(kSFT, 0, kTDC,    13),
      HistMaker::getUniqueID(kSFT, 0, kTDC,    14),
    };

    int sft_id_c2[n_hist_c2] = {
      HistMaker::getUniqueID(kSFT, 0, kADC,     1),
      HistMaker::getUniqueID(kSFT, 0, kADC,     2),
      HistMaker::getUniqueID(kSFT, 0, kADC,     3),
      HistMaker::getUniqueID(kSFT, 0, kADC,     4),
      HistMaker::getUniqueID(kSFT, 0, kADC,    11),
      HistMaker::getUniqueID(kSFT, 0, kADC,    12),
      HistMaker::getUniqueID(kSFT, 0, kADC,    13),
      HistMaker::getUniqueID(kSFT, 0, kADC,    14),
    };

    int sft_id_c3[n_hist_c3] = {
      HistMaker::getUniqueID(kSFT, 0, kHitPat,  1),
      HistMaker::getUniqueID(kSFT, 0, kHitPat,  2),
      HistMaker::getUniqueID(kSFT, 0, kHitPat,  3),
      HistMaker::getUniqueID(kSFT, 0, kHitPat,  4),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 11),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 12),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 13),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 14),
    };

    int sft_id_c4[n_hist_c4] = {
      HistMaker::getUniqueID(kSFT, 0, kMulti,   1),
      HistMaker::getUniqueID(kSFT, 0, kMulti,   2),
      HistMaker::getUniqueID(kSFT, 0, kMulti,   3),
      HistMaker::getUniqueID(kSFT, 0, kMulti,  11),
      HistMaker::getUniqueID(kSFT, 0, kMulti,  12),
      HistMaker::getUniqueID(kSFT, 0, kMulti,  13),
    };

    int sft_id_c5[n_hist_c5] = {
      HistMaker::getUniqueID(kSFT, 0, kTDC2D,   1),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D,   2),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D,   3),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D,   4),
      HistMaker::getUniqueID(kSFT, 0, kADC2D,   1),
      HistMaker::getUniqueID(kSFT, 0, kADC2D,   2),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D,   3),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D,   4),
    };

    // draw TDC/TOT
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
      c->Clear();
      c->Divide(4,2);
      for(int i=0; i<n_hist_c1; i++){
	c->cd(i+1)->SetGrid();
	TH1 *h = (TH1*)GHist::get(sft_id_c1[i]);
	h->Draw();
      }
      c->Update();
    }

    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
      c->Clear();
      c->Divide(4,2);
      for(int i=0; i<n_hist_c2; i++){
	c->cd(i+1)->SetGrid();
	TH1 *h = (TH1*)GHist::get(sft_id_c2[i]);
	h->Draw();
      }
      c->Update();
    }

    // draw HitPat
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
      c->Clear();
      c->Divide(4,2);
      for(int i=0; i<n_hist_c3; i++){
	c->cd(i+1)->SetGrid();
	TH1 *h = (TH1*)GHist::get(sft_id_c3[i]);
	h->Draw();
      }
      c->Update();
    }

    // draw Multi
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
      c->Clear();
      c->Divide(3,2);
      for(int i=0; i<n_hist_c4; i++){
	c->cd(i+1)->SetGrid();
	TH1 *h = (TH1*)GHist::get(sft_id_c4[i]);
	h->Draw();
      }
      c->Update();
    }
    // draw TDC/TOT 2D
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
      c->Clear();
      c->Divide(4,2);
      for(int i=0; i<n_hist_c5; i++){
	c->cd(i+1)->SetGrid();
	TH1 *h = (TH1*)GHist::get(sft_id_c5[i]);
	h->Draw("colz");
      }
      c->Update();
    }
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
