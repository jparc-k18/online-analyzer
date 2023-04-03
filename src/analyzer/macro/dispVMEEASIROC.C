// -*- C++ -*-

#include "DetectorID.hh"

#include "UserParamMan.hh"

using namespace hddaq::gui; // for Updater

//_____________________________________________________________________________
void
dispVMEEASIROC( void )
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------


  // draw TDC-2D
  { // aft01
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_t_2d_id = HistMaker::getUniqueID(kVMEEASIROC, 0, kTDC2D,   1);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_t_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw TDC-2D
  { // aft02
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_t_2d_id = HistMaker::getUniqueID(kVMEEASIROC, 0, kTDC2D,   1);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_t_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw TOT-2D
  { // aft01
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_tot_2d_id = HistMaker::getUniqueID(kVMEEASIROC, 0, kTOT2D,   1);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_tot_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw TOT-2D
  { // aft02
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_tot_2d_id = HistMaker::getUniqueID(kVMEEASIROC, 0, kTOT2D,   1);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_tot_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw HighGain-2D
  { // aft01
    TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_hg_2d_id  = HistMaker::getUniqueID(kVMEEASIROC, 0, kHighGain, 11);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_hg_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw HighGain-2D
  { // aft02
    TCanvas *c = (TCanvas*)gROOT->FindObject("c6");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_hg_2d_id  = HistMaker::getUniqueID(kVMEEASIROC, 0, kHighGain, 11);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_hg_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw LowGain-2D
  { // aft01
    TCanvas *c = (TCanvas*)gROOT->FindObject("c7");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_lg_2d_id  = HistMaker::getUniqueID(kVMEEASIROC, 0, kLowGain, 11);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i >= 12 ) break;
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_lg_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // draw HighGain-2D
  { // aft02
    TCanvas *c = (TCanvas*)gROOT->FindObject("c8");
    c->Clear();
    c->Divide(3, 5);
    int vmeeasiroc_lg_2d_id  = HistMaker::getUniqueID(kVMEEASIROC, 0, kLowGain, 11);
    for( int i=0; i<NumOfPlaneVMEEASIROC; ++i ){
      if( i < 12 ) continue;
      c->cd((i-12)+1);
      TH1 *h = (TH1*)GHist::get( vmeeasiroc_lg_2d_id + i );
      if( !h ) continue;
      h->Draw("colz");
    }
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------

}
