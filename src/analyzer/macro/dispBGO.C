// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBGO()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  int NumOfSegClBGO = 60;
  int NumOfSlotCl = 5;
  int NumOfSegSingleBGO = 18;
  int NumOfSlotSingle   = 3;

  ////////// BGO TDC
  {
    // Clover
    for(int slot=0; slot<NumOfSlotCl; slot++){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", slot+1));
      c->Clear();
      c->Divide(3,4);
      int base_id = HistMaker::getUniqueID(kBGO, 0, kTDC, 1);
      for(int i=0; i<NumOfSegClBGO/NumOfSlotCl; i++){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get(base_id + i + slot*NumOfSegClBGO/NumOfSlotCl);
	h->Draw();
      }
      c->Update();
    }

    // Single
    for(int slot=0; slot<NumOfSlotSingle; slot++){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", slot+6));
      c->Clear();
      c->Divide(3,2);
      int base_id = HistMaker::getUniqueID(kBGO, 0, kTDC, 1);
      for(int i=0; i<NumOfSegSingleBGO/NumOfSlotSingle; i++){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get(base_id + i + 60 + slot*NumOfSegSingleBGO/NumOfSlotSingle);
	h->Draw();
      }
      c->Update();
    }

  }

  ////////// BGO TDC 2D
  {

      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 9));
      c->Clear();
      int base_id = HistMaker::getUniqueID(kBGO, 0, kTDC2D, 1);
      TH2 *h2 = (TH2*)GHist::get(base_id);
      h2->Draw("colz");
      c->Update();
  }


  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
