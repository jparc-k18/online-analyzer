// -*- C++ -*-

#include "DetectorID.hh"

#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispVMEEASIROC1D( void )
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw HighGain-2D
  { // aft01-53
    int vmeeasiroc_hg_2d_id  = HistMaker::getUniqueID(kVMEEASIROC, 0, kHighGain, 11);
    TH1D *h_py[NumOfSegVMEEASIROC];
    TF1 *fgaus = new TF1("fgaus", "gaus", 700, 1000);
    for( int iPlane=0; iPlane<NumOfPlaneVMEEASIROC; ++iPlane ){
      int planeId = PlaneIdOfVMEEASIROC[iPlane];
      if( planeId != 53 ) continue;
      TH2 *h = (TH2*)GHist::get( vmeeasiroc_hg_2d_id + iPlane );
      if( !h ) continue;
      for( int i = 0; i < 4; i++ ){
	TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", i+1));;
	c->Clear();
	c->Divide(4, 4);
	for( int iSeg = 0; iSeg < NumOfSegVMEEASIROC; ++iSeg ){
	  if( iSeg/16 != i ) continue;
	  c->cd(iSeg%16+1);
	  h_py[iSeg] = h->ProjectionY(Form("VMEEASIROC%d_Ch%d", planeId, iSeg), iSeg+1, iSeg+1);
	  h_py[iSeg]->SetTitle(Form("VMEEASIROC%d_Ch%d", planeId, iSeg));
	  h_py[iSeg]->Rebin(2);
	  h_py[iSeg]->GetXaxis()->SetRangeUser(700, 1100);
	  // h_py[iSeg]->Fit("fgaus");
	  h_py[iSeg]->Draw();

	  // int npeaks = 5;
	  // int maxpeaks = 2*npeaks;
	  // TSpectrum *s = new TSpectrum(maxpeaks);
	  // int nfound = s->Search(h_py[iSeg], 2, "", 0.10);
	  // std::cout << "npeaks[" << iSeg << "] = " << nfound << std::endl;

	  // // TH1F *h2 = (TH1F*)h->Clone("h2");
	  // // // Use TSpectrum to find the peak candidates
	  // // TSpectrum *s = new TSpectrum(2*npeaks);
	  // // Int_t nfound = s->Search(h,2,"",0.10);
	  // // printf("Found %d candidate peaks to fit\n",nfound);
	  // // // Estimate background using TSpectrum::Background
	  // // TH1 *hb = s->Background(h,20,"same");
	  // // if (hb) c1->Update();
	  // // if (np <0) return;

	  // //estimate linear background using a fitting method
	  // // c1->cd(2);
	  // // TF1 *fline = new TF1("fline","pol1",0,1000);
	  // // h->Fit("fline","qn");
	  // // Loop on all found peaks. Eliminate peaks at the background level
	  // // par[0] = fline->GetParameter(0);
	  // // par[1] = fline->GetParameter(1);
	  // npeaks = 0;
	  // Double_t *xpeaks;
	  // xpeaks = s->GetPositionX();
	  // TF1 *fgaus[maxpeaks];
	  // for (int p=0;p<nfound;p++) {
	  //   Double_t xp = xpeaks[p];
	  //   Int_t bin = h_py[iSeg]->GetXaxis()->FindBin(xp);
	  //   Double_t yp = h_py[iSeg]->GetBinContent(bin);
	  //   fgaus[p] = new TF1(Form("fgaus%d", p+1), "gaus", xp-10, xp+10);
	  //   fgaus[p]->SetLineColor(kRed);
	  //   fgaus[p]->SetParameter(0, yp);
	  //   fgaus[p]->SetParameter(1, xp);
	  //   fgaus[p]->SetParameter(2, 3.);
	  //   h_py[iSeg]->Fit(Form("fgaus%d", p+1), "N", "", xp-10, xp+10);
	  //   fgaus[p]->Draw("same");
	  //   // // if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
	  //   // par[3*npeaks+2] = yp; // "height"
	  //   // par[3*npeaks+3] = xp; // "mean"
	  //   // par[3*npeaks+4] = 3; // "sigma"
	  // }
	  // // printf("Found %d useful peaks to fit\n",npeaks);
	  // // printf("Now fitting: Be patient\n");
	  // // TF1 *fit = new TF1("fit",fpeaks,0,1000,2+3*npeaks);
	  // // // We may have more than the default 25 parameters
	  // // TVirtualFitter::Fitter(h2,10+3*npeaks);
	  // // fit->SetParameters(par);
	  // // fit->SetNpx(1000);
	  // // h2->Fit("fit");


	  // // double stddev = fgaus->GetParameter(2);
    	  // // // double xpos  = h->GetXaxis()->GetBinCenter(h->GetNbinsX())*0.1;
    	  // // // double ypos  = h->GetYaxis()->GetBinCenter(h->GetNbinsY())*0.5;
    	  // // TLatex *text = new TLatex(0.6, 0.6, Form("stddev %.2f", stddev));
    	  // // text->SetTextSize(0.2);
    	  // // text->Draw("same");
	}
	c->Update();
      }
    }
  }

  { // aft02-48
    int vmeeasiroc_hg_2d_id  = HistMaker::getUniqueID(kVMEEASIROC, 0, kHighGain, 11);
    TH1D *h_py[NumOfSegVMEEASIROC];
    TF1 *fgaus = new TF1("fgaus", "gaus", 700, 1000);
    for( int iPlane=0; iPlane<NumOfPlaneVMEEASIROC; ++iPlane ){
      int planeId = PlaneIdOfVMEEASIROC[iPlane];
      if( planeId != 48 ) continue;
      TH2 *h = (TH2*)GHist::get( vmeeasiroc_hg_2d_id + iPlane );
      if( !h ) continue;
      for( int i = 0; i < 4; i++ ){
	TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", i+5));;
	c->Clear();
	c->Divide(4, 4);
	for( int iSeg = 0; iSeg < NumOfSegVMEEASIROC; ++iSeg ){
	  if( iSeg/16 != i ) continue;
	  c->cd(iSeg%16+1);
	  h_py[iSeg] = h->ProjectionY(Form("VMEEASIROC%d_Ch%d", planeId, iSeg), iSeg+1, iSeg+1);
	  h_py[iSeg]->SetTitle(Form("VMEEASIROC%d_Ch%d", planeId, iSeg));
	  h_py[iSeg]->Rebin(2);
	  h_py[iSeg]->GetXaxis()->SetRangeUser(700, 1100);
	  // h_py[iSeg]->Fit("fgaus");
	  h_py[iSeg]->Draw();

	  // int npeaks = 5;
	  // int maxpeaks = 2*npeaks;
	  // TSpectrum *s = new TSpectrum(maxpeaks);
	  // int nfound = s->Search(h_py[iSeg], 2, "", 0.10);
	  // std::cout << "npeaks[" << iSeg << "] = " << nfound << std::endl;

	  // // TH1F *h2 = (TH1F*)h->Clone("h2");
	  // // // Use TSpectrum to find the peak candidates
	  // // TSpectrum *s = new TSpectrum(2*npeaks);
	  // // Int_t nfound = s->Search(h,2,"",0.10);
	  // // printf("Found %d candidate peaks to fit\n",nfound);
	  // // // Estimate background using TSpectrum::Background
	  // // TH1 *hb = s->Background(h,20,"same");
	  // // if (hb) c1->Update();
	  // // if (np <0) return;

	  // //estimate linear background using a fitting method
	  // // c1->cd(2);
	  // // TF1 *fline = new TF1("fline","pol1",0,1000);
	  // // h->Fit("fline","qn");
	  // // Loop on all found peaks. Eliminate peaks at the background level
	  // // par[0] = fline->GetParameter(0);
	  // // par[1] = fline->GetParameter(1);
	  // npeaks = 0;
	  // Double_t *xpeaks;
	  // xpeaks = s->GetPositionX();
	  // TF1 *fgaus[maxpeaks];
	  // for (int p=0;p<nfound;p++) {
	  //   Double_t xp = xpeaks[p];
	  //   Int_t bin = h_py[iSeg]->GetXaxis()->FindBin(xp);
	  //   Double_t yp = h_py[iSeg]->GetBinContent(bin);
	  //   fgaus[p] = new TF1(Form("fgaus%d", p+1), "gaus", xp-10, xp+10);
	  //   fgaus[p]->SetLineColor(kRed);
	  //   fgaus[p]->SetParameter(0, yp);
	  //   fgaus[p]->SetParameter(1, xp);
	  //   fgaus[p]->SetParameter(2, 3.);
	  //   h_py[iSeg]->Fit(Form("fgaus%d", p+1), "N", "", xp-10, xp+10);
	  //   fgaus[p]->Draw("same");
	  //   // // if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
	  //   // par[3*npeaks+2] = yp; // "height"
	  //   // par[3*npeaks+3] = xp; // "mean"
	  //   // par[3*npeaks+4] = 3; // "sigma"
	  // }
	  // // printf("Found %d useful peaks to fit\n",npeaks);
	  // // printf("Now fitting: Be patient\n");
	  // // TF1 *fit = new TF1("fit",fpeaks,0,1000,2+3*npeaks);
	  // // // We may have more than the default 25 parameters
	  // // TVirtualFitter::Fitter(h2,10+3*npeaks);
	  // // fit->SetParameters(par);
	  // // fit->SetNpx(1000);
	  // // h2->Fit("fit");


	  // // double stddev = fgaus->GetParameter(2);
    	  // // // double xpos  = h->GetXaxis()->GetBinCenter(h->GetNbinsX())*0.1;
    	  // // // double ypos  = h->GetYaxis()->GetBinCenter(h->GetNbinsY())*0.5;
    	  // // TLatex *text = new TLatex(0.6, 0.6, Form("stddev %.2f", stddev));
    	  // // text->SetTextSize(0.2);
    	  // // text->Draw("same");
	}
	c->Update();
      }
    }
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------

}
