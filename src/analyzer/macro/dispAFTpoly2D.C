// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispAFTpoly2D( void )
{
  //  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw Hitpattern
  {
    int aft_chit_id     = HistMaker::getUniqueID( kAFT, 0, kHitPat, 100 );
    for( int i = 0; i < 2; i++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", i+1));
      c->Clear();
      TH2 *h = (TH2*)GHist::get( aft_chit_id+(kUorD+1)*NumOfPlaneAFT+i+1 );
      if( h ) h->Draw("colz");
      c->Update();
    }
  }


  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
