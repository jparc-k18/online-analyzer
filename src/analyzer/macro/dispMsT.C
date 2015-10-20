// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispMsT()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  int n_seg = 16;

  // draw TDC 1st harf 
  TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  c->Clear();
  c->Divide(4,4);
  int base_id = HistMaker::getUniqueID(kMsT, 0, kTDC, 1);
  for(int i = 0; i<n_seg; ++i){
    c->cd(i+1);
    gPad->SetLogy();
    TH1 *h = (TH1*)GHist::get(base_id + i);
    h->GetXaxis()->SetRangeUser(0,2048);
    h->Draw();
  }

  c->Update();

  // draw TDC 2nd harf
  c = (TCanvas*)gROOT->FindObject("c2");
  c->Clear();
  c->Divide(4,4);
  int base_id = HistMaker::getUniqueID(kMsT, 0, kTDC, 1+n_seg);
  for(int i = 0; i<n_seg; ++i){
    c->cd(i+1);
    gPad->SetLogy();
    TH1 *h = (TH1*)GHist::get(base_id + i);
    h->GetXaxis()->SetRangeUser(0,3000);
    h->Draw();
  }

  c->Update();

  // draw HitPat with TOF & LC
  c = (TCanvas*)gROOT->FindObject("c3");
  c->Clear();
  c->Divide(2,2);
  int base_id = HistMaker::getUniqueID(kMsT, 0, kHitPat, 0);
  c->cd(1);
  GHist::get(base_id)->Draw();
  c->cd(2);
  GHist::get(base_id+1)->Draw();
  c->cd(3);// TOF HitPat
  base_id = HistMaker::getUniqueID(kTOF, 0, kHitPat);
  GHist::get(base_id)->Draw();
  c->cd(4);// LC HitPat
  base_id = HistMaker::getUniqueID(kLC, 0, kHitPat);
  GHist::get(base_id)->Draw();

  c->Update();
  
  // draw HitPat & Flag
  c = (TCanvas*)gROOT->FindObject("c4");
  c->Clear();
  c->Divide(2,2);
  c->cd(1);
  int base_id = HistMaker::getUniqueID(kMsT, 0, kHitPat, 0);
  GHist::get(base_id)->Draw();
  c->cd(2);
  GHist::get(base_id+1)->Draw();
  c->cd(3);
  base_id = HistMaker::getUniqueID(kMsT, 0, kTDC2D);
  GHist::get(base_id)->Draw("col");
  c->cd(4);
  base_id = HistMaker::getUniqueID(kMsT, 0, kHitPat2D,0);
  GHist::get(base_id)->Draw("text");

  c->Update();

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
