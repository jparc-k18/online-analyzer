// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispAFTHitPat( void )
{
  //  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw Hitpattern
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    int aft_chit_id = HistMaker::getUniqueID( kAFT, 0, kHitPat, 101 );
    c->Clear();
    c->Divide(6, 3);
    for( int l = 0; l < NumOfPlaneAFT; l++ ){
      if( l%4 != 0 && l%4 != 1 ) continue;
      c->cd(l/2+1+l%2);
      TH2 *h = (TH2*)GHist::get( aft_chit_id+kUorD*NumOfPlaneAFT+l );
      if( h ) h->Draw("colz");
    }
    c->Update();
  }

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    int aft_chit_id = HistMaker::getUniqueID( kAFT, 0, kHitPat, 101 );
    c->Clear();
    c->Divide(6, 3);
    for( int l = 0; l < NumOfPlaneAFT; l++ ){
      if( l%4 != 2 && l%4 != 3 ) continue;
      c->cd(l/2+l%2);
      TH2 *h = (TH2*)GHist::get( aft_chit_id+kUorD*NumOfPlaneAFT+l );
      if( h ) h->Draw("colz");
    }
    c->Update();
  }

  {
    int aft_chit_id = HistMaker::getUniqueID( kAFT, 0, kHitPat, 100 );
    for( int i = 0; i < 2; i++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", i+3));
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
