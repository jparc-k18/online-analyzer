// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBGO()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

//  int n_seg = 12;

  // draw ADC  1-12
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,3);
    int fadc_id     = HistMaker::getUniqueID(kBGO, 0, kFADC);
    for( int i=0; i<NumOfSegBGO/2; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( fadc_id + i );
      if( !h ) continue;
      h->Draw();
    }
    c->Update();
  }

  // draw ADC 13-24
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(4,3);
    int fadc_id     = HistMaker::getUniqueID(kBGO, 0, kFADC, 13);
    for( int i=0; i<NumOfSegBGO/2; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( fadc_id + i );
      if( !h ) continue;
      h->Draw();
    }
    c->Update();
  }

  // draw TDC  1-12
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(4,3);
    int tdc_id     = HistMaker::getUniqueID(kBGO, 0, kTDC);
    for( int i=0; i<NumOfSegBGO/2; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_id + i );
      if( !h ) continue;
      h->Draw();
    }
    c->Update();
  }

  // draw TDC 13-24
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide(4,3);
    int tdc_id     = HistMaker::getUniqueID(kBGO, 0, kTDC, 13);
    for( int i=0; i<NumOfSegBGO/2; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_id + i );
      if( !h ) continue;
      h->Draw();
    }
    c->Update();
  }

  // draw clk
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
    c->Clear();
    c->Divide(4,2);
    int clk_id     = HistMaker::getUniqueID(kBGO, 0, kTDC, 25);
    for( int i=0; i<NumOfSegBGO_T; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( clk_id + i );
      if( !h ) continue;
      h->Draw();
    }
    int h_id     = HistMaker::getUniqueID(kBGO, 0, kHitPat, 1);
    TH1 *h1 = (TH1*)GHist::get( h_id );
    c->cd(5);
    h1->Draw();

    h_id     = HistMaker::getUniqueID(kBGO, 0, kHitPat, 2);
    TH1 *h2 = (TH1*)GHist::get( h_id );
    c->cd(6);
    h2->Draw();

    h_id     = HistMaker::getUniqueID(kBGO, 0, kMulti, 1);
    TH1 *h3 = (TH1*)GHist::get( h_id );
    c->cd(7);
    h3->Draw();

    h_id     = HistMaker::getUniqueID(kBGO, 0, kMulti, 2);
    TH1 *h4 = (TH1*)GHist::get( h_id );
    c->cd(8);
    h4->Draw();

    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
