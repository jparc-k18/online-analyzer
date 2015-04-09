// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBH2()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  int n_seg = 5;

  // draw ADC
  TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  c->Clear();
  c->Divide(3,2);
  int base_id = HistMaker::getUniqueID(kBH2, 0, kADC, 1);
  for(int i = 0; i<n_seg; ++i){
    c->cd(i+1);
    gPad->SetLogy();
    TH1 *h = (TH1*)GHist::get(base_id + i)->Clone();
    h->GetXaxis()->SetRangeUser(0,2000);
    h->Draw();
  }

  c->Update();

  // draw TDC
  c = (TCanvas*)gROOT->FindObject("c2");
  c->Clear();
  c->Divide(3,2);
  int base_id = HistMaker::getUniqueID(kBH2, 0, kTDC, 1);
  for(int i = 0; i<n_seg; ++i){
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
