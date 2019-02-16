// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispSCH()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int n_seg     = 64;

  int sch_id_c1[] = {
    HistMaker::getUniqueID(kSCH, 0, kTDC,    kSCH_1to16_Offset),
    HistMaker::getUniqueID(kSCH, 0, kTDC,    kSCH_17to64_Offset),
    HistMaker::getUniqueID(kSCH, 0, kADC,    kSCH_1to16_Offset),
    HistMaker::getUniqueID(kSCH, 0, kADC,    kSCH_17to64_Offset),
    HistMaker::getUniqueID(kSCH, 0, kHitPat, 1),
    HistMaker::getUniqueID(kSCH, 0, kTDC2D,  1),
    HistMaker::getUniqueID(kSCH, 0, kADC2D,  1),
    HistMaker::getUniqueID(kSCH, 0, kMulti,  1)
  };

  // draw TDC/TOT
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,2);
    for(int i=0; i<8; i++){
      c->cd(i+1);//->SetGrid();
      TH1 *h = (TH1*)GHist::get( sch_id_c1[i] );
      h->Draw("colz");
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
