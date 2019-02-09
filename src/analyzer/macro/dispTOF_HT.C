// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispTOF_HT()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw TDC-HT TDC
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,4);
    int tdc_id = HistMaker::getUniqueID( kTOF_HT, 0, kTDC, 1 );
    for( int i=0; i<NumOfSegHtTOF; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_id + i );
      if( h ) h->Draw();
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
