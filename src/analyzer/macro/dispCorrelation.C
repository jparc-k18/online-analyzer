// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispCorrelation( void )
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3,2);
    for( Int_t i=0; i<6; ++i ){
      c->cd(i+1);//->SetGrid();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kCorrelation, 0, 0, i+1) );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(2,2);
    for( Int_t i=0; i<4; ++i ){
      c->cd(i+1);//->SetGrid();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kCorrelation, 0, 0, i+7) );
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
