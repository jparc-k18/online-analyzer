// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispParaBGO()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int NumOfSegParaBGO  = 2;
  const int NumOfPlaneParaTC = 2;
  const int NumOfSegVMEEASIROC = 64;

  ////////// BGO ADC w/ TDC, TDC
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(2,2);
    int adc_id = HistMaker::getUniqueID(kParaBGO, 0, kADC);
    int tdc_id = HistMaker::getUniqueID(kParaBGO, 0, kTDC);
    int awt_id = HistMaker::getUniqueID(kParaBGO, 0, kADCwTDC);

    for (int i=0; i<2; ++i){
      c->cd(i+1);
      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get(adc_id+i);
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser( 0 , 4096 );
      h->Draw();
      TH1 *hh =(TH1*)GHist::get(awt_id + i);
      if( !hh ) continue;
      hh->GetXaxis()->SetRangeUser( 0 , 4096 );
      hh->SetLineColor( kRed );
      hh->Draw("same");
    }
    for (int i=0; i<2; ++i){
      c->cd(i+3);
      TH1 *hhh = (TH1*)GHist::get(tdc_id+i);
      if( !hhh ) continue;
      hhh->GetXaxis()->SetRangeUser( 0 , 4096 );
      hhh->Draw();
    }
    c->Update();
  }

  // TC TOT vs TDC
  {
    // for (int plane=0; plane<NumOfPlaneParaTC; ++plane){
    //   TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", plane+2));
    //   c->Clear();
    //   c->Divide(8,8);

    //   int base_id = HistMaker::getUniqueID(kParaTC, 0, kTOTXTDC, 30);
    //   for (int seg = 0; seg<NumOfSegVMEEASIROC; ++seg){
    // 	c->cd(seg+1);
    // 	TH2* h2 = (TH2*)GHist::get(++base_id);
    // 	if ( !h2 ) continue;
    // 	h2->Draw("colz");
    //   }
    //   c->Update();
    // }
  }

  // TC1,2 TDC/TOT
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 4));
    c->Clear();
    c->Divide(2,2);

    int tdc_id = HistMaker::getUniqueID(kParaTC, 0, kTDC, 0);
    int tot_id = HistMaker::getUniqueID(kParaTC, 0, kTOT, 0);
    for (int plane = 0; plane<NumOfPlaneParaTC; ++plane){
      c->cd(plane+1);
      TH1* h_tdc = (TH1*)GHist::get(++tdc_id);
      if ( !h_tdc ) continue;
      h_tdc->Draw();

      c->cd(plane+3);
      TH1* h_tot = (TH1*)GHist::get(++tot_id);
      if ( !h_tot ) continue;
      h_tot->Draw();
    }
    c->Update();
  }

  // TC1,2 Hitpat/Multiplicity
  {
    int hitpat_id = HistMaker::getUniqueID(kParaTC, 0, kHitPat, 0);
    int Multiplicity_id = HistMaker::getUniqueID(kParaTC, 0, kMulti, 0);
    for (int plane = 0; plane<NumOfPlaneParaTC; ++plane){
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", plane+5));
      c->Clear();
      c->Divide(2,2);

      for (int xy = 0; xy<2; ++xy){
	c->cd(xy+1);
	TH1* h_hit = (TH1*)GHist::get(++hitpat_id);
	if ( !h_hit ) continue;
	h_hit->Draw();

	c->cd(xy+3);
	TH1* h_multi = (TH1*)GHist::get(++Multiplicity_id);
	if ( !h_multi ) continue;
	h_multi->Draw();
      }
      c->Update();
    }
  }

  // VC ADC vs TDC
  {

  }

  // VC Hitpat/Multiplicity
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 7));
    c->Clear();
    c->Divide(2,3);

    int hitpat_id = HistMaker::getUniqueID(kParaVC, 0, kHitPat, 0);
    int Multiplicity_id = HistMaker::getUniqueID(kParaVC, 0, kMulti, 0); // need to change (multiplicity has 5 hists)
    c->cd(1);
    TH1* h_hit = (TH1*)GHist::get(++hitpat_id);
    h_hit->Draw();

    for (int i=0; i<5; ++i){
      c->cd(i+2);
      TH1* h_multi = (TH1*)GHist::get(++Multiplicity_id);
      h_multi->Draw();
    }

    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
