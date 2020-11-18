#include "DetectorID.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispBH2()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------
  //
  // TDC gate range
  static const unsigned int tdc_min = gUser.GetParameter("BH2_TDC_FPGA", 0);
  static const unsigned int tdc_max = gUser.GetParameter("BH2_TDC_FPGA", 1);

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    TCanvas *c2 = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(3,2);
    c2->Clear();
    c2->Divide(3,2);
    int icanvas = 0;
    int icanvas3 = 0;
    for( int ud=0; ud<NumOfSegBH2; ++ud ){
      TH1 *h  = NULL;
      TH1 *hh = NULL;
      // draw ADC
      c->cd( ++icanvas );
      gPad->SetLogy();
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kBH2, 0, kADC, ud+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,2000);
	h->Draw();
      }
      hh = (TH1*)GHist::get( HistMaker::getUniqueID(kBH2, 0, kADCwTDC, ud+1) );
      if( hh ){
	hh->GetXaxis()->SetRangeUser(0,2000);
	hh->SetLineColor( kRed );
	hh->Draw("same");
      }
    }
    for( int ud=0; ud<NumOfSegBH2; ++ud ){
      TH1 *h  = NULL;
      // draw TDC_FPGA
      c2->cd( ++icanvas3 );
      //      gPad->SetLogy();
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kBH2, 0, kTDC, ud+1) );
      if( h ){
	//h->GetXaxis()->SetRangeUser(0,2000);
	h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
	h->Draw();
      }
    }
    c->Update();
    c->cd(0);
    c2->Update();
    c2->cd(0);
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
