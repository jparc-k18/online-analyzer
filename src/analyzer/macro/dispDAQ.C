// -*- C++ -*-

using hddaq::gui::Updater;

//_____________________________________________________________________________
void
dispDAQ( void )
{
  if( Updater::isUpdating() )
    return;
  Updater::setUpdating( true );

  //gStyle->SetOptStat(1111110);

  std::vector<Int_t> hist_id = {
    HistMaker::getUniqueID( kDAQ, kEB, kHitPat ),
    HistMaker::getUniqueID( kDAQ, kVME, kHitPat2D ),
    HistMaker::getUniqueID( kDAQ, kEASIROC, kHitPat2D ),
    HistMaker::getUniqueID( kDAQ, kHUL, kHitPat2D ),
  };

  {
    auto c = dynamic_cast<TCanvas*>( gROOT->FindObject("c1") );
    c->Clear();
    c->Divide( 1, 2 );
    c->cd(1)->Divide( 3, 1 );
    for( Int_t i=0, n=hist_id.size(); i<n; ++i ){
      if( i < n -1 )
        c->cd( 1 )->cd( i+1 );
      else
        c->cd( 2 );
      gPad->SetGrid();
      TH1 *h = GHist::get( hist_id[i] );
      if( h ) h->Draw("colz");
    }
    c->Update();
  }

  Updater::setUpdating( false );
}
