// -*- C++ -*-

#include"DetectorID.hh"

using namespace hddaq::gui;

//_____________________________________________________________________________
void
dispBAC( void )
{
  if( Updater::isUpdating() )
    return;
  Updater::setUpdating( true );

  auto c1 = dynamic_cast<TCanvas*>( gROOT->FindObject( "c1" ) );
  c1->Clear();
  c1->Divide( 2, 2 );
  Int_t id[NumOfSegBAC*3] = {
    HistMaker::getUniqueID( kBAC,  0, kADC,     1 ),
    HistMaker::getUniqueID( kBAC,  0, kADC,     2 ),
    HistMaker::getUniqueID( kBAC,  0, kADCwTDC, 1 ),
    HistMaker::getUniqueID( kBAC,  0, kADCwTDC, 2 ),
    HistMaker::getUniqueID( kBAC,  0, kTDC,     1 ),
    HistMaker::getUniqueID( kBAC,  0, kTDC,     2 )
  };
  TH1 *h = nullptr;

  for( Int_t i=0; i<NumOfSegBAC; ++i ){
    // ADC
    c1->cd( i+1 )->SetLogy();
    h = dynamic_cast<TH1*>( GHist::get( id[i] ) );
    h->GetXaxis()->SetRangeUser( 0, 0x1000 );
    h->Draw();
    // ADCwTDC
    h = dynamic_cast<TH1*>( GHist::get( id[i+NumOfSegBAC] ) );
    h->SetLineColor( kRed );
    h->Draw( "same" );
    // TDC
    c1->cd( i+NumOfSegBAC+1 )->SetLogy();
    h = dynamic_cast<TH1*>( GHist::get( id[i+NumOfSegBAC*2] ) );
    h->GetXaxis()->SetRangeUser( 0, 2000000 );
    h->Draw();
  }

  c1->Update();
  c1->cd(0);

  Updater::setUpdating( false );
}
