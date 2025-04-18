// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispBcOutSdcInTracking( void )
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  gStyle->SetOptStat(1111110);
  int n_layer = 22;
  static const std::vector<TString> VPs =
    {
      "VP1", "VP2", "VP3", "VP4"
    };

  Double_t SS_size[2] = {800./2, 240./2}; // Sieve-Slit size {X, Y}

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kMisc, 0, kHitPat, 250);
    for( int i=0; i<12; ++i ){ // Residual BcOut
      c->cd(i+1);
      TH1 *h = GHist::get(base_id + i);
      if( h ) h->Draw();
    }
    c->Update();
  }

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(4,3);
    int base_id = HistMaker::getUniqueID(kMisc, 0, kHitPat, 250);
    for( int i=0; i<10; ++i ){ // Residual SdcIn
      c->cd(i+1);
      TH1 *h = GHist::get(base_id + i + 12);
      if( h ) h->Draw();
    }
    c->Update();
  }

  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(4,2);
    int base_id = HistMaker::getUniqueID(kMisc, 0, kHitPat, 200);
    for( int i=0; i<2; ++i ){ // X, Y profile
      for( int j=0; j<VPs.size(); j++ ){
	Int_t hid = i*VPs.size()+j;
	c->cd(hid+1);
	TH1 *h = GHist::get(base_id + hid);
	if( !h ) continue;
	h->Draw();
      }
    }
    c->Update();
  }

  { // XY profile
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide(2,2);
    int base_id = HistMaker::getUniqueID(kMisc, 0, kHitPat, 200)+2*VPs.size();
    for( int i=0; i<VPs.size(); i++ ){
      c->cd(i+1);
      TH1 *h = GHist::get(base_id + i);
      if( !h ) continue;
      h->SetStats(0);
      h->Draw();

      if( VPs[i]=="VP4" ){
	TBox *box = new TBox(-SS_size[0], -SS_size[1], SS_size[0], SS_size[1]);
	box->SetLineColor(kRed);
	box->SetLineStyle(1);
	box->SetLineWidth(3);
	box->SetFillStyle(0);
	box->Draw("same");
      }
    }
    c->Update();
  }


  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
