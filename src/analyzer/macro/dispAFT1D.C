// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispAFT1D( void )
{
  //  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw TDC UandD
  {
    int aft_t_id     = HistMaker::getUniqueID( kAFT, 0, kTDC, 0 );
    for( int ud=0; ud<2; ud++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", ud+1));
      c->Clear();
      c->Divide(8, 5);
      for( int i=0; i<NumOfPlaneAFT; ++i ){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get( ++aft_t_id );
	if( !h ) continue;
	h->Draw();
      }
      c->Update();
    }
  }

  // draw TOT UandD
  {
    int aft_tot_id     = HistMaker::getUniqueID( kAFT, 0, kTOT, 0 );
    int aft_ctot_id    = HistMaker::getUniqueID( kAFT, 0, kTOT, 100 );
    for( int ud=0; ud<2; ud++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", ud+3));
      c->Clear();
      c->Divide(8, 5);
      for( int i=0; i<NumOfPlaneAFT; ++i ){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get( ++aft_tot_id );
	if( !h ) continue;
	h->Draw();
	TH1 *hh = (TH1*)GHist::get( ++aft_ctot_id );
	if( !hh ) continue;
	// hh->GetXaxis()->SetRangeUser( 0, 4096 );
	hh->SetLineColor( kRed );
	hh->Draw("same");
      }
      c->Update();
    }
  }

  // draw HighGain UandD
  {
    int aft_hg_id     = HistMaker::getUniqueID( kAFT, 0, kHighGain, 0 );
    int aft_chg_id    = HistMaker::getUniqueID( kAFT, 0, kHighGain, 100 );
    for( int ud=0; ud<2; ud++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", ud+5));
      c->Clear();
      c->Divide(8, 5);
      for( int i=0; i<NumOfPlaneAFT; ++i ){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get( ++aft_hg_id );
	if( !h ) continue;
	h->Draw();
	TH1 *hh = (TH1*)GHist::get( ++aft_chg_id );
	if( !hh ) continue;
	// hh->GetXaxis()->SetRangeUser( 0, 4096 );
	hh->SetLineColor( kRed );
	hh->Draw("same");
      }
      c->Update();
    }
  }

  // draw LowGain UandD
  {
    int aft_lg_id     = HistMaker::getUniqueID( kAFT, 0, kLowGain, 0 );
    int aft_clg_id    = HistMaker::getUniqueID( kAFT, 0, kLowGain, 100 );
    for( int ud=0; ud<2; ud++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", ud+7));
      c->Clear();
      c->Divide(8, 5);
      for( int i=0; i<NumOfPlaneAFT; ++i ){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get( ++aft_lg_id );
	if( !h ) continue;
	h->Draw();
	TH1 *hh = (TH1*)GHist::get( ++aft_clg_id );
	if( !hh ) continue;
	// hh->GetXaxis()->SetRangeUser( 0, 4096 );
	hh->SetLineColor( kRed );
	hh->Draw("same");
      }
      c->Update();
    }
  }

  // draw pe UandD
  {
    int aft_pe_id     = HistMaker::getUniqueID( kAFT, 0, kPede, 0 );
    for( int ud=0; ud<2; ud++ ){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", ud+9));
      c->Clear();
      c->Divide(8, 5);
      for( int i=0; i<NumOfPlaneAFT; ++i ){
	c->cd(i+1);
	TH1 *h = (TH1*)GHist::get( ++aft_pe_id );
	if( !h ) continue;
	h->Draw();
      }
      c->Update();
    }
  }




  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
