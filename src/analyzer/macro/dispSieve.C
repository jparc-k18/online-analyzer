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
	h->GetXaxis()->SetRangeUser(0,200000);
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
    }
    c1->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
