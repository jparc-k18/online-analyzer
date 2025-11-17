#include "DetectorID.hh"

// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispSieve()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------
  //
  {
    static const unsigned int tdc_min = gUser.GetParameter("TdcSieve", 0);
    static const unsigned int tdc_max = gUser.GetParameter("TdcSieve", 1);

    TCanvas *c1 = (TCanvas*)gROOT->FindObject("c1");
    c1->Clear();
    c1->Divide(2,2);

    int icanvas = 0;

    for( int LR=0; LR<NumOfSegParaTOFC; LR++){
      TH1 *h  = NULL;
      // draw TDC
      c1->cd( ++icanvas );
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kTOFC, 0, kTDC, LR+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
	h->Draw();
      }
    }

    for( int LR=0; LR<NumOfSegParaTOFC; LR++ ){
      TH1 *h  = NULL;
      // draw TOT
      c1->cd( ++icanvas );
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kTOFC, 0, kTOT, LR+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,100000);
	h->Draw();
      }
      h = (TH1*)GHist::get( HistMaker::getUniqueID(kTOFC, 0, kADCwTDC, LR+1) );
      if( h ){
	h->GetXaxis()->SetRangeUser(0,100000);
	h->SetLineColor(kRed);
	h->Draw("same");
      }
    }
    c1->Update();
  }

  {
    static const unsigned int sac3_tdc_min = gUser.GetParameter("TdcSAC3", 0);
    static const unsigned int sac3_tdc_max = gUser.GetParameter("TdcSAC3", 1);
    static const unsigned int tf_tdc_min = gUser.GetParameter("TdcTF_TF", 0);
    static const unsigned int tf_tdc_max = gUser.GetParameter("TdcTF_TF", 1);

    TCanvas *c2 = (TCanvas*)gROOT->FindObject("c2");
    c2->Clear();
    c2->Divide(2,2);
    TH1 *h  = NULL;

    // SS-R ADC (SAC3)
    c2->cd(1);
    gPad->SetLogy();
    h = (TH1*)GHist::get( HistMaker::getUniqueID(kSAC3, 0, kADC) );
    h->Draw();
    h = (TH1*)GHist::get( HistMaker::getUniqueID(kSAC3, 0, kADCwTDC) );
    h->SetLineColor(kRed);
    h->Draw("same");

    // SS-L ADC (TF_TF)
    c2->cd(2);
    gPad->SetLogy();
    h = (TH1*)GHist::get( HistMaker::getUniqueID(kTF_TF, 0, kADC) );
    h->Draw();
    h = (TH1*)GHist::get( HistMaker::getUniqueID(kTF_TF, 0, kADCwTDC) );
    h->SetLineColor(kRed);
    h->Draw("same");

    // SS-R TDC (SAC3)
    c2->cd(3);
    h = (TH1*)GHist::get( HistMaker::getUniqueID(kSAC3, 0, kTDC) );
    h->GetXaxis()->SetRangeUser(sac3_tdc_min, sac3_tdc_max);
    h->Draw();

    // SS-L TDC (TF_TF)
    c2->cd(4);
    h = (TH1*)GHist::get( HistMaker::getUniqueID(kTF_TF, 0, kTDC) );
    h->GetXaxis()->SetRangeUser(tf_tdc_min, tf_tdc_max);
    h->Draw();

    c2->Update();
  }



  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
