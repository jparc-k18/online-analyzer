// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispAFT_TOTxTDC( void )
{
  //  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw TOT-2D UorD
  {
    int aft_tot_t_id     = HistMaker::getUniqueID( kAFT, 0, kTOTXTDC, 0 );
    //for( int ud=0; ud<2; ud++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 1));
      c->Clear();
      c->Divide(2, 2);
      for( int i=0; i<4; ++i ){
	c->cd(i+1);
	TH2 *h = (TH2*)GHist::get( aft_tot_t_id+NumOfPlaneAFT+NumOfPlaneAFT+i+1 );
	if( !h ) continue;
	h->Draw("colz");
      }
      c->Update();
  }


  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
