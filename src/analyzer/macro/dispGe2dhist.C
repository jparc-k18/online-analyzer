// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispGe2dhist()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // const int NumOfSegGe = 16;
  const int NumOfSegGe = 23;
  ////////// Ge TDC

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kGe, 0, kTFA_ADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH2 *h = (TH2*)GHist::get(base_id + i);
      h->GetXaxis()->SetRangeUser(2000,5000);
      h->GetYaxis()->SetRangeUser(0,4000);
      h->Draw("colz");
    }
    c->Update();
  }
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kGe, 0, kTFA_ADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH2 *h = (TH2*)GHist::get(base_id + i);
      h->GetXaxis()->SetRangeUser(2000,5000);
      h->GetYaxis()->SetRangeUser(0,4000);
      h->Draw("colz");
    }
    c->Update();
  }

  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
  //   c->Clear();
  //   c->Divide(4,4);
  //   int base_id = HistMaker::getUniqueID(kGe, 0, kCRM_ADC, 1);
  //   for(int i=0; i<NumOfSegGe; i++){
  //     c->cd(i+1);
  //     TH2 *h = (TH2*)GHist::get(base_id + i);
  //     h->Draw("colz");
  //   }
  //   c->Update();
  // }

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kGe, 0, kRST_ADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH2 *h = (TH2*)GHist::get(base_id + i);
      h->Draw("colz");
    }
    c->Update();
  }

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kGe, 0, kRST_ADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH2 *h = (TH2*)GHist::get(base_id + i);
      h->Draw("colz");
    }
    c->Update();
  }

  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
  //   c->Clear();
  //   c->Divide(4,4);
  //   int base_id = HistMaker::getUniqueID(kGe, 0, kTFA_CRM, 1);
  //   for(int i=0; i<NumOfSegGe; i++){
  //     c->cd(i+1);
  //     TH2 *h = (TH2*)GHist::get(base_id + i);
  //     h->Draw("colz");
  //   }
  //   c->Update();
  // }

      // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
