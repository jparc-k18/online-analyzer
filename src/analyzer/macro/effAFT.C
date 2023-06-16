// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "DetectorID.hh"
void effAFT()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  int n_layer = 6;

  // draw Multi with plane efficiency AFT
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3,3);
    int aft_cmul_id = HistMaker::getUniqueID(kAFT, 0, kMulti,  101);
    for(int i = 0; i<NumOfPlaneAFT; ++i){
      if( i%4==0 ){
	c->cd(i/4+1);
	TH1 *h_wt  = (TH1*)GHist::get(aft_cmul_id+(kUorD+1)*NumOfPlaneAFT+(i+1)/2);
	h_wt->Draw();

	double Nof0     = h_wt->GetBinContent(1);
	double NofTotal = h_wt->GetEntries();
	double eff      = 1. - (double)Nof0/NofTotal;

	double xpos  = h_wt->GetXaxis()->GetBinCenter(h_wt->GetNbinsX())*0.3;
	double ypos  = h_wt->GetMaximum()*0.8;
	TLatex *text = new TLatex(xpos, ypos, Form("plane eff. %.2f", eff));
	text->SetTextSize(0.08);
	text->Draw();
      }else{
	continue;
      }
      c->Modified();
      c->Update();
    }
  }

  {TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(3,3);
    int aft_cmul_id = HistMaker::getUniqueID(kAFT, 0, kMulti,  101);
    for(int i = 0; i<NumOfPlaneAFT; ++i){
      if( i%4==1 ){
	c->cd(i/4+1);
	TH1 *h_wt  = (TH1*)GHist::get(aft_cmul_id+(kUorD+1)*NumOfPlaneAFT+(i+1)/2);
	h_wt->Draw();

	double Nof0     = h_wt->GetBinContent(1);
	double NofTotal = h_wt->GetEntries();
	double eff      = 1. - (double)Nof0/NofTotal;

	double xpos  = h_wt->GetXaxis()->GetBinCenter(h_wt->GetNbinsX())*0.3;
	double ypos  = h_wt->GetMaximum()*0.8;
	TLatex *text = new TLatex(xpos, ypos, Form("plane eff. %.2f", eff));
	text->SetTextSize(0.08);
	text->Draw();
      }else{
	continue;
      }
      c->Modified();
      c->Update();
    }
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
