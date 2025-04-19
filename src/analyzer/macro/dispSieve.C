#include "DetectorID.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispSieve()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------
  //
  static const unsigned int tdc_min = gUser.GetParameter("TdcSieve", 0);
  static const unsigned int tdc_max = gUser.GetParameter("TdcSieve", 1);

  {
    TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
    c1->Clear();
    c1->Divide(2,2);

    int icanvas = 0;

    for( int LR=0; LR<NumOfSegParaTOFC; LR++){
      TH1 *h  = NULL;
      // draw TDC
      c1->cd( ++icanvas );
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kTOFC, 0, kTDC, LR+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
	h->Draw();
      }
    }

    for( int LR=0; LR<NumOfSegParaTOFC; LR++ ){
      TH1 *h  = NULL;
      // draw TOT
      c1->cd( ++icanvas );
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kTOFC, 0, kTOT, LR+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,100000);
	h->Draw();
      }
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kTOFC, 0, kADCwTDC, LR+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,100000);
	h->SetLineColor(kRed);
	h->Draw("same");
      }
    }
    c1->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
