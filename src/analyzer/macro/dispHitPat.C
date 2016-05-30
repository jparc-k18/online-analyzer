// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispHitPat()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw ADC
  TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  c->Clear();
  c->Divide(4,2);

  TH1* h = NULL;

  // beam
  c->cd(1);
  int base_id = HistMaker::getUniqueID(kBH1, 0, kHitPat);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(2);
  base_id = HistMaker::getUniqueID(kBFT, 0, kHitPat);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(3);
  base_id = HistMaker::getUniqueID(kBC4, 0, kHitPat, 6);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(4);
  base_id = HistMaker::getUniqueID(kFBH, 0, kHitPat);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(5);
  base_id = HistMaker::getUniqueID(kSDC1, 0, kHitPat, 1);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(6);
  base_id = HistMaker::getUniqueID(kSCH, 0, kHitPat);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(7);
  base_id = HistMaker::getUniqueID(kSDC2, 0, kHitPat, 1);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->cd(8);
  base_id = HistMaker::getUniqueID(kTOF, 0, kHitPat);
  h = (TH1*)GHist::get(base_id);
  h->SetMinimum(0);
  h->Draw();

  c->Update();

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
