// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispCFT()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  ////////// CFT
  {
    const int n_hist_c1 = 8;
    const int n_hist_c2 = 8;
    const int n_hist_c3 = 8;
    const int n_hist_c4 = 8;
    const int n_hist_c5 = 8;


    int sft_id_c1[n_hist_c1] = {
      HistMaker::getUniqueID(kCFT, 0, kTDC,    0), // V 
      HistMaker::getUniqueID(kCFT, 0, kTDC,    1), // U 
      HistMaker::getUniqueID(kCFT, 0, kTDC,    2), // X
      HistMaker::getUniqueID(kCFT, 0, kTDC,    3), // X'
      HistMaker::getUniqueID(kCFT, 0, kTDC,    4), // V 
      HistMaker::getUniqueID(kCFT, 0, kTDC,    5), // U 
      HistMaker::getUniqueID(kCFT, 0, kTDC,    6), // X
      HistMaker::getUniqueID(kCFT, 0, kTDC,    7), // X'
    };

    int sft_id_c2[n_hist_c2] = {
      HistMaker::getUniqueID(kCFT, 0, kADC,     0), // V 
      HistMaker::getUniqueID(kCFT, 0, kADC,     1), // U 
      HistMaker::getUniqueID(kCFT, 0, kADC,     2), // X
      HistMaker::getUniqueID(kCFT, 0, kADC,     3), // X'
      HistMaker::getUniqueID(kCFT, 0, kADC,     4), // V 
      HistMaker::getUniqueID(kCFT, 0, kADC,     5), // U 
      HistMaker::getUniqueID(kCFT, 0, kADC,     6), // X
      HistMaker::getUniqueID(kCFT, 0, kADC,     7), // X'
    };

    int sft_id_c3[n_hist_c3] = {
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  0), // V 
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  1), // U 
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  2), // X
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  3), // X'
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  4), // V 
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  5), // U 
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  6), // X
      HistMaker::getUniqueID(kCFT, 0, kHitPat,  7), // X'
    };

    int sft_id_c4[n_hist_c4] = {
      HistMaker::getUniqueID(kCFT, 0, kMulti,   0), // V
      HistMaker::getUniqueID(kCFT, 0, kMulti,   1), // U 
      HistMaker::getUniqueID(kCFT, 0, kMulti,   2), // X
      HistMaker::getUniqueID(kCFT, 0, kMulti,   3), // V
      HistMaker::getUniqueID(kCFT, 0, kMulti,   4), // U
      HistMaker::getUniqueID(kCFT, 0, kMulti,   5), // X
      HistMaker::getUniqueID(kCFT, 0, kMulti,   6), // U
      HistMaker::getUniqueID(kCFT, 0, kMulti,   7), // X
    };                                             
                                                  
    int sft_id_c5[n_hist_c5] = {
      HistMaker::getUniqueID(kCFT, 0, kTDC2D,   0), // V 
      HistMaker::getUniqueID(kCFT, 0, kTDC2D,   1), // U 
      HistMaker::getUniqueID(kCFT, 0, kTDC2D,   2), // X
      HistMaker::getUniqueID(kCFT, 0, kTDC2D,   3), // X'
      HistMaker::getUniqueID(kCFT, 0, kADC2D,   4), // V 
      HistMaker::getUniqueID(kCFT, 0, kADC2D,   5), // U 
      HistMaker::getUniqueID(kCFT, 0, kADC2D,   6), // X
      HistMaker::getUniqueID(kCFT, 0, kADC2D,   7), // X'
    };

    // draw TDC/TOT
    {
      TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
      c->Clear();
      c->Divide(4,2);
      for(int i=0; i<n_hist_c1; i++){
	c->cd(i+1);//->SetGrid();
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
	c->cd(i+1);//->SetGrid();
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
	c->cd(i+1);//->SetGrid();
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
	c->cd(i+1);//->SetGrid();
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
	c->cd(i+1);//->SetGrid();
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
