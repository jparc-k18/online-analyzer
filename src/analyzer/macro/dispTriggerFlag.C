// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispTriggerFlag( void )
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  {
    const Int_t n = 24;
    TCanvas *c = dynamic_cast<TCanvas*>( gROOT->FindObject("c1") );
    c->Clear();
    c->Divide(6,4);
    Int_t id = HistMaker::getUniqueID( kTriggerFlag, 0, kTDC );
    for( int i=0; i<n; ++i ){
      c->cd(i+1)->SetLogy();
      TH1 *h = dynamic_cast<TH1*>( GHist::get( id+i  ) );
      if( !h ) continue;
      h->Draw();
    }
    c->Update();
  }

  {
    TCanvas *c = dynamic_cast<TCanvas*>( gROOT->FindObject("c2") );
    c->Clear();
    c->Divide(6,4);
    Int_t id = HistMaker::getUniqueID( kTriggerFlag, 0, kHitPat );
    c->cd();
    TH1 *h = dynamic_cast<TH1*>( GHist::get( id ));
    if( !h ) continue;
    h->Draw();
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
