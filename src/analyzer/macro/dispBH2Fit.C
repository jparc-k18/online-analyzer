#include "DetectorID.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispBH2Fit()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------
  //
  // TDC gate range
  static const unsigned int tdc_min = gUser.GetParameter("BH2_TDC_FPGA", 0);
  static const unsigned int tdc_max = gUser.GetParameter("BH2_TDC_FPGA", 1);

  {
    TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
    c1->Clear();
    c1->Divide(4,2);
    int icanvas1 = 0;
    for( int ud=NumOfSegBH2; ud<NumOfSegBH2*2; ++ud ){
      TH1 *h  = NULL;
      TH1 *h1  = NULL;
      TF1 fit = TF1("fit","gaus");
      fit.SetLineColor(kRed);
      // draw TDC_FPGA
      c1->cd( ++icanvas1 );
      //      gPad->SetLogy();
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kBH2, 0, kTDC, ud+1) );
      h1 = h;
      if( h1 ){
	//h1->GetXaxis()->SetRangeUser(0,2000);
	h1->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
//        double max  = h1->GetMaximumBin();
        double max  =  h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin());
	double range = 100.;
//	h1->Fit("fit","IQ","",tdc_min,tdc_max);
	h1->Fit("fit","IQ","",max-range,max+range);
	h1->Draw();
        double mean = fit.GetParameter(1);
//        double xpos  = h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin());
        double xpos  = tdc_min+1000;
        double ypos  = h1->GetMaximum()*0.7;
        TLatex *text = new TLatex(xpos, ypos, Form("Mean. %.2f", mean));
        text->SetTextSize(0.08);
        text->Draw("same");
      }
    }
    c1->Update();
    c1->cd(0);
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
