// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBH2()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(2,2);
    int icanvas = 0;
    for( int ud=0; ud<2; ++ud ){
      TH1 *h = NULL;
      // draw ADC
      c->cd( ++icanvas );
      gPad->SetLogy();
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kBH2, 0, kADC, ud+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,2000);
	h->Draw();
      }
      // draw TDC
      c->cd( ++icanvas );
      gPad->SetLogy();
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kBH2, 0, kTDC, ud+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,2000);
	h->Draw();
      }
      c->Update();
      c->cd(0);
    }
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
