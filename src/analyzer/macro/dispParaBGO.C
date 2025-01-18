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

  Int_t SegOfVC[10] = {36, 42, 58, 62, 46, 48, 60, 50, 32, 34};

  // BGO&TMC ADC w/ TDC, TDC
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3,2);
    int adc_id  = HistMaker::getUniqueID(kParaBGO, 0, kADC);
    int tdc_id  = HistMaker::getUniqueID(kParaBGO, 0, kTDC);
    int awt_id  = HistMaker::getUniqueID(kParaBGO, 0, kADCwTDC);
    int tadc_id = HistMaker::getUniqueID(kParaTMC, 0, kADC);
    int ttdc_id  = HistMaker::getUniqueID(kParaTMC, 0, kTDC);
    int tawt_id  = HistMaker::getUniqueID(kParaTMC, 0, kADCwTDC);

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

    c->cd(3);
    gPad->SetLogy();
    TH1 *th = (TH1*)GHist::get(tadc_id);
    th->GetXaxis()->SetRangeUser( 0 , 4096 );
    th->Draw();
    TH1 *thh = (TH1*)GHist::get(tawt_id);
    thh->GetXaxis()->SetRangeUser( 0 , 4096 );
    thh->SetLineColor( kRed );
    thh->Draw("same");

    for (int i=0; i<2; ++i){
      c->cd(i+4);
      TH1 *hhh = (TH1*)GHist::get(tdc_id+i);
      if( !hhh ) continue;
      hhh->GetXaxis()->SetRangeUser( 0 , 4096 );
      hhh->Draw();
    }

    c->cd(6);
    TH1 *thhh = (TH1*)GHist::get(ttdc_id);
    thhh->Draw();

    c->Update();
  }

  // TC1,2 TDC/TOT 1D
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 2));
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
      TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", plane+3));
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
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 5));
    c->Clear();
    c->Divide(4,3);

    int axt_id = HistMaker::getUniqueID(kParaVC, 0, kHighGainXTDC, 30);
    for (int i=0; i<10; ++i){
      c->cd(i+1);
      TH2* h_axt = (TH2*)GHist::get(axt_id+1 + SegOfVC[i]);
      if ( !h_axt ) continue;
      h_axt->Draw("colz");
    }
    c->Update();
  }

  // VC TDC/TOT
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", 6));
    c->Clear();
    c->Divide(1,2);

    int tdc_id = HistMaker::getUniqueID(kParaVC, 0, kTDC, 0);
    int tot_id = HistMaker::getUniqueID(kParaVC, 0, kTOT, 0);

    c->cd(1);
    TH1* h_tdc = (TH1*)GHist::get(++tdc_id);
    h_tdc->Draw();

    c->cd(2);
    TH1* h_tot = (TH1*)GHist::get(++tot_id);
    h_tot->Draw();
    c->Update();
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
