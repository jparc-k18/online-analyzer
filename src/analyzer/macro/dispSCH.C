// -*- C++ -*-

using hddaq::gui::Updater;

//_____________________________________________________________________________
void
dispSCH( void )
{
  if( Updater::isUpdating() )
    return;
  Updater::setUpdating( true );

  const auto& gUser = UserParamMan::GetInstance();
  const Double_t RefTot = gUser.GetParameter( "SCH_REFTOT" );

  std::vector<Int_t> hid_c1 = {
    HistMaker::getUniqueID( kSCH, 0, kTDC, NumOfSegSCH+1 ),
    HistMaker::getUniqueID( kSCH, 0, kADC, NumOfSegSCH+1 ),
    HistMaker::getUniqueID( kSCH, 0, kHitPat, 1 ),
    HistMaker::getUniqueID( kSCH, 0, kTDC2D, 1 ),
    HistMaker::getUniqueID( kSCH, 0, kADC2D, 1 ),
    HistMaker::getUniqueID( kSCH, 0, kMulti, 1 )
  };

  // draw TDC/TOT
  {
    auto c = dynamic_cast<TCanvas*>( gROOT->FindObject("c1") );
    c->Clear();
    c->Divide( 3, 2 );
    for( Int_t i=0, n=hid_c1.size(); i<n; ++i ){
      c->cd( i+1 ); //->SetGrid();
      auto h = dynamic_cast<TH1*>( GHist::get( hid_c1.at(i) ) );
      if( TString( h->GetTitle() ).Contains("TOT") )
	h->GetXaxis()->SetRangeUser(0., 100.);
      h->Draw("colz");
      if( TString( h->GetTitle() ).Contains("TOT") &&
	  TString( h->GetTitle() ).Contains("ALL")) {
	Double_t peak = h->GetMaximum();
	TLine *l = new TLine( RefTot, 0, RefTot, peak );
	l->SetLineColor( kRed );
	l->Draw( "same" );
      }
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
