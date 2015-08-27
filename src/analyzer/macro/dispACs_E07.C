// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispACs_E07()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int n_seg_bac  = 4+2;
  const int n_seg_pvac = 6+1;
  const int n_seg_fac  = 6+1;
  
  int bac_id[n_seg_bac*2] = {
    HistMaker::getUniqueID(kBAC_E07,  0, kADC, 1),
    HistMaker::getUniqueID(kBAC_E07,  0, kADC, 2),
    HistMaker::getUniqueID(kBAC_E07,  0, kADC, 3),
    HistMaker::getUniqueID(kBAC_E07,  0, kADC, 4),
    HistMaker::getUniqueID(kBAC_E07,  0, kADC, 5),
    HistMaker::getUniqueID(kBAC_E07,  0, kADC, 6),

    HistMaker::getUniqueID(kBAC_E07,  0, kTDC, 1),
    HistMaker::getUniqueID(kBAC_E07,  0, kTDC, 2),
    HistMaker::getUniqueID(kBAC_E07,  0, kTDC, 3),
    HistMaker::getUniqueID(kBAC_E07,  0, kTDC, 4),
    HistMaker::getUniqueID(kBAC_E07,  0, kTDC, 5),
    HistMaker::getUniqueID(kBAC_E07,  0, kTDC, 6)
  };

  int pvac_id[n_seg_pvac*2] = {
    HistMaker::getUniqueID(kPVAC,  0, kADC, 1),
    HistMaker::getUniqueID(kPVAC,  0, kADC, 2),
    HistMaker::getUniqueID(kPVAC,  0, kADC, 3),
    HistMaker::getUniqueID(kPVAC,  0, kADC, 4),
    HistMaker::getUniqueID(kPVAC,  0, kADC, 5),
    HistMaker::getUniqueID(kPVAC,  0, kADC, 6),
    HistMaker::getUniqueID(kPVAC,  0, kADC, 7),

    HistMaker::getUniqueID(kPVAC,  0, kTDC, 1),
    HistMaker::getUniqueID(kPVAC,  0, kTDC, 2),
    HistMaker::getUniqueID(kPVAC,  0, kTDC, 3),
    HistMaker::getUniqueID(kPVAC,  0, kTDC, 4),
    HistMaker::getUniqueID(kPVAC,  0, kTDC, 5),
    HistMaker::getUniqueID(kPVAC,  0, kTDC, 6),
    HistMaker::getUniqueID(kPVAC,  0, kTDC, 7)
  };

  int fac_id[n_seg_fac*2] = {
    HistMaker::getUniqueID(kFAC,  0, kADC, 1),
    HistMaker::getUniqueID(kFAC,  0, kADC, 2),
    HistMaker::getUniqueID(kFAC,  0, kADC, 3),
    HistMaker::getUniqueID(kFAC,  0, kADC, 4),
    HistMaker::getUniqueID(kFAC,  0, kADC, 5),
    HistMaker::getUniqueID(kFAC,  0, kADC, 6),
    HistMaker::getUniqueID(kFAC,  0, kADC, 7),

    HistMaker::getUniqueID(kFAC,  0, kTDC, 1),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 2),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 3),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 4),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 5),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 6),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 7)
  };

  { // draw BAC ADC & TDC
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3,4);
    for(int i = 0; i<n_seg_bac*2; ++i){
      c->cd(i+1);
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(bac_id[i]);
      h->GetXaxis()->SetRangeUser(0,4000);
      h->Draw();
    }
  }
  c->Update();

  { // draw PVAC ADC & TDC
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(4,4);
    for(int i = 0; i<n_seg_pvac*2; ++i){
      if(i<n_seg_pvac) c->cd(i+1);
      else             c->cd(i+2);
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(pvac_id[i]);
      h->GetXaxis()->SetRangeUser(0,4000);
      h->Draw();
    }
  }
  c->Update();

  { // draw FAC ADC & TDC
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(4,4);
    for(int i = 0; i<n_seg_fac*2; ++i){
      if(i<n_seg_pvac) c->cd(i+1);
      else             c->cd(i+2);
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(fac_id[i]);
      h->GetXaxis()->SetRangeUser(0,4000);
      h->Draw();
    }
  }
  c->Update();

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
