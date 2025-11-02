
using namespace hddaq::gui;

//_____________________________________________________________________________
void dispGeTdc()
{
  if( Updater::isUpdating() )
    return;
  Updater::setUpdating( true );

  std::vector<Int_t> base_id = {
    HistMaker::getUniqueID(kGe, 0, kCRM),
    HistMaker::getUniqueID(kGe, 0, kCRM),
    HistMaker::getUniqueID(kGe, 0, kTFA),
    HistMaker::getUniqueID(kGe, 0, kTFA),
    HistMaker::getUniqueID(kGe, 0, kRST),
    HistMaker::getUniqueID(kGe, 0, kRST),
  };

  // draw TDC
  for( Int_t t=0, n=base_id.size(); t<n; ++t ){
    auto c = dynamic_cast<TCanvas*>( gROOT->FindObject( Form("c%d", t+1) ) );
    c->Clear();
    c->Divide( 4, 3 );
    for( Int_t i=0; i<NumOfSegGe; ++i ){
      if(t%2==0) {
	if( i >= 12 ) break;
	c->cd( i+1 );
      }
      else {
	if( i < 12 ) continue;
	c->cd( (i-12)+1 );
      }
      gPad->SetLogy();
      auto h = GHist::get( base_id[t]+i );
      h->Draw();
    }
    c->Update();
  }

  Updater::setUpdating( false );
}
