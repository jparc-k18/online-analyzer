// -*- C++ -*-

#include "DetectorID.hh"
#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispAFTTot( void )
{
  //  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------
  const auto& gUser = UserParamMan::GetInstance();
  const Double_t TotRef = gUser.GetParameter( "TotRefAFT" );

  // draw TOT for each MPPC vias
  {

    static std::vector<TString> name = {
      "AFT_TOT_X1-3", "AFT_TOT_X4-6", "AFT_TOT_X7-9",
      "AFT_TOT_Y1-3", "AFT_TOT_Y4-6", "AFT_TOT_Y7-9"
    };

    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3, 2);
    int aft_tot_id  = HistMaker::getUniqueID( kAFT, 0, kTOT, 1 );
    int aft_ctot_id = HistMaker::getUniqueID( kAFT, 0, kTOT, 101 );
    TH1D *h_X[3];
    TH1D *h_Y[3];
    for( int i = 0; i < 3; i++ ){
      h_X[i] = new TH1D( name[i], name[i], 150, 0, 150);
      h_Y[i] = new TH1D( name[i+3], name[i+3], 150, 0, 150);
    }
    for( int ud=0; ud<2; ud++ ){
      for( int i=0; i<NumOfPlaneAFT; ++i ){
	TH1 *h = (TH1*)GHist::get( aft_tot_id + i );
	if( !h ) continue;
	if( i%4 == 0 || i%4 == 1 ) h_X[i/12]->Add(h);
	if( i%4 == 2 || i%4 == 3 ) h_Y[i/12]->Add(h);
      }
    }

    for( int i = 0; i < 3; i++ ){
      {
	c->cd(i+1);
	h_X[i]->Draw();
	TF1 f("f", "gaus", 0., 100.);
	f.SetLineColor(kRed);
	Double_t p = h_X[i]->GetBinCenter(h_X[i]->GetMaximumBin());
	if(p < 30.) p = 60.;
	Double_t w = 10.;
	for(Int_t ifit=0; ifit<3; ++ifit){
	  Double_t fmin = p - w;
	  Double_t fmax = p + w;
	  h_X[i]->Fit("f", "Q", "", fmin, fmax);
	  p = f.GetParameter(1);
	  w = f.GetParameter(2) * 1.;
	}
	TLatex *text = new TLatex();
	text->SetNDC();
	text->SetTextSize(0.100);
	text->SetText(0.500, 0.700, Form("%.2f", p));
	text->Draw();
	auto l = new TLine( TotRef, 0, TotRef, h_X[i]->GetMaximum() );
	l->Draw();
      }
      {
	c->cd(i+4);
	h_Y[i]->Draw();
	TF1 f("f", "gaus", 0., 100.);
	f.SetLineColor(kRed);
	Double_t p = h_Y[i]->GetBinCenter(h_Y[i]->GetMaximumBin());
	if(p < 30.) p = 60.;
	Double_t w = 10.;
	for(Int_t ifit=0; ifit<3; ++ifit){
	  Double_t fmin = p - w;
	  Double_t fmax = p + w;
	  h_Y[i]->Fit("f", "Q", "", fmin, fmax);
	  p = f.GetParameter(1);
	  w = f.GetParameter(2) * 1.;
	}
	TLatex *text = new TLatex();
	text->SetNDC();
	text->SetTextSize(0.100);
	text->SetText(0.500, 0.700, Form("%.2f", p));
	text->Draw();
	auto l = new TLine( TotRef, 0, TotRef, h_Y[i]->GetMaximum() );
	l->Draw();
      }
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
