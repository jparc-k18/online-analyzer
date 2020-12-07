// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispGeAdc()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int NumOfSegGe = 16;
  ////////// Ge ADC
  {
    // draw ADC

    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,4);
    int base_id = HistMaker::getUniqueID(kGe, 0, kADC, 1);
    for(int i=0; i<NumOfSegGe; i++){
      c->cd(i+1);
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(base_id +i);
      h->Draw();
    }
    c->Update();
  }

  // draw ADC-2D
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(2,2);
    int hist_id[] = {
		     HistMaker::getUniqueID(kGe, 0, kADC, 1) +NumOfSegGe,
		     HistMaker::getUniqueID(kGe, 0, kADC, 1) +NumOfSegGe+1,
		     HistMaker::getUniqueID(kGe, 0, kADC2D,  1),
		     HistMaker::getUniqueID(kGe, 0, kHitPat, 1)
    };
    for(int i=0; i<4; i++){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get(hist_id[i]);
      h->Draw("colz");
      }
    c->Update();
  }


  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
