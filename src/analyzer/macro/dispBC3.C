// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBC3()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  int n_layer = 6;

  // draw TDC
  TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  c->Clear();
  c->Divide(3,2);
  int base_id = HistMaker::getUniqueID(kBC3, 0, kTDC);
  for(int i = 0; i<n_layer; ++i){
    c->cd(i+1);
    TH1 *h = GHist::get(base_id + i);
    h->GetXaxis()->SetRangeUser(256,1000);
    h->Draw();
  }

  c->Update();

  // draw HitPat
  c = (TCanvas*)gROOT->FindObject("c2");
  c->Clear();
  c->Divide(3,2);
  int base_id = HistMaker::getUniqueID(kBC3, 0, kHitPat);
  for(int i = 0; i<n_layer; ++i){
    c->cd(i+1);
    GHist::get(base_id + i)->Draw();
  }

  c->Update();

  // draw Multi
  c = (TCanvas*)gROOT->FindObject("c3");
  c->Clear();
  c->Divide(3,2);
  int base_id = HistMaker::getUniqueID(kBC3, 0, kMulti);
  for(int i = 0; i<n_layer; ++i){
    c->cd(i+1);
    GHist::get(base_id + i)->Draw();
  }

  c->Update();

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
