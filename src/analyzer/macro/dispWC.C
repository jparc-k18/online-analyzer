// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispWC( void )
{
  if( Updater::isUpdating() )
    return;
  Updater::setUpdating( true );

  const auto& gUser = UserParamMan::GetInstance();
  const UInt_t tdc_min = gUser.GetParameter( "WC_TDC_FPGA", 0 );
  const UInt_t tdc_max = gUser.GetParameter( "WC_TDC_FPGA", 1 );

  auto c1 = dynamic_cast<TCanvas*>( gROOT->FindObject("c1") );
  c1->Clear();
  c1->Divide( 3, 2 );
  Int_t icanvas = 0;
  Int_t id = 0;
  TH1* h1 = nullptr;
  for( Int_t i=0; i<NumOfSegWC; ++i ){
    for( Int_t ud=0; ud<2; ++ud ){
      if( i == NumOfSegWC-1 && ud == 1 )
	continue;
      // ADC
      c1->cd( ++icanvas )->SetLogy();
      id = HistMaker::getUniqueID( kWC, 0, kADC, i+ud*NumOfSegWC+1 );
      h1 = dynamic_cast<TH1*>( GHist::get( id ) );
      if( h1 ){
	h1->GetXaxis()->SetRangeUser( 0, 0x1000 );
	h1->Draw();
      }
      // ADCw/TDC
      id = HistMaker::getUniqueID( kWC, 0, kADCwTDC, i+ud*NumOfSegWC+1 );
      h1 = dynamic_cast<TH1*>( GHist::get( id ) );
      if( h1 ){
	h1->GetXaxis()->SetRangeUser( 0, 0x1000 );
	h1->SetLineColor( kRed );
	h1->Draw( "same" );
      }
      // TDC
      c1->cd( icanvas + NumOfSegWC*2 - 1 )->SetLogy();
      id = HistMaker::getUniqueID( kWC, 0, kTDC, i+ud*NumOfSegWC+1 );
      h1 = dynamic_cast<TH1*>( GHist::get( id ) );
      if( h1 ){
	h1->GetXaxis()->SetRangeUser( tdc_min, tdc_max );
	h1->Draw();
      }
    }
  }

  c1->Update();
  c1->cd(0);

  Updater::setUpdating( false );
}
