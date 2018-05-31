// -*- C++ -*-

#include "MacroBuilder.hh"

#include <iostream>
#include <string>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TMacro.h>
#include <TString.h>
#include <TText.h>

#include "DetectorID.hh"
#include "Main.hh"
#include "HistHelper.hh"
#include "HistMaker.hh"

namespace analyzer
{

namespace macro
{

//____________________________________________________________________________
TObject*
Get( TString name )
{
  std::string process = Main::getInstance().getArgv().at(0);
  Int_t n = process.find("bin");
  TString path = process.substr(0, n)+"src/analyzer/macro/";

  if( name.Contains(".C") )
    name.ReplaceAll(".C","");

  return new TMacro(path+name+".C");
}

}

//____________________________________________________________________________
// For HttpServer
namespace http
{

  namespace
  {
    enum eUorD { kU, kD, kUorD };
    enum eAorT { kA, kT, kAorT };
  }

//____________________________________________________________________________
TCanvas*
Correlation( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3, 2);
  for( Int_t i=0; i<6; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kCorrelation, 0, 0, i+1) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CorrelationFBT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(2, 2);
  for( Int_t i=0; i<4; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kCorrelation, 0, 0, i+7) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
HitPattern( void )
{
  return HitPatternE40();
}

//____________________________________________________________________________
TCanvas*
BH1ADC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(6,4);
  for( Int_t i=0; i<NumOfSegBH1; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*(NumOfSegBH1+1))->SetLogy();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kBH1, 0, kADC, i+j*NumOfSegBH1+1) );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kBH1, 0, kADCwTDC, i+j*NumOfSegBH1+1) );
      if( !hh ) continue;
      hh->GetXaxis()->SetRangeUser(0,2000);
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BH1TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(6,4);
  for( Int_t i=0; i<NumOfSegBH1; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*(NumOfSegBH1+1))->SetLogy();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kBH1, 0, kTDC, i+j*NumOfSegBH1+1) );
      if( !h ) continue;
      h->Draw();
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BFT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  {
    c1->cd(1)->Divide(4,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kBFT, 0, kTDC, 11),
      HistMaker::getUniqueID(kBFT, 0, kADC, 11),
      HistMaker::getUniqueID(kBFT, 0, kHitPat,11),
      HistMaker::getUniqueID(kBFT, 0, kMulti,11)
    };
    for( Int_t i=0, n=id.size(); i<n; ++i ){
      for( Int_t j=0; j<kUorD; ++j ){
	c1->cd(1)->cd(i+1+j*n);
	if( i==n-1 && j==kD ) continue;
	TH1 *h = GHist::get(id[i]+j);
	if( !h ) continue;
	h->Draw();
      }
    }
  }

  {
    c1->cd(2)->Divide(2,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kBFT, 0, kTDC2D),
      HistMaker::getUniqueID(kBFT, 0, kADC2D)
    };

    for( Int_t i=0, n=id.size(); i<n; ++i ){
      for( Int_t j=0; j<kUorD; ++j ){
	c1->cd(2)->cd(i+1+j*n);
	TH1 *h = GHist::get(id[i]+j);
	if( !h ) continue;
	h->Draw("colz");
      }
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
BH2ADC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,4);
  for( Int_t i=0; i<NumOfSegBH2; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*(NumOfSegBH2))->SetLogy();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kBH2, 0, kADC, i+j*NumOfSegBH2+1) );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kBH2, 0, kADCwTDC, i+j*NumOfSegBH2+1) );
      if( !hh ) continue;
      hh->GetXaxis()->SetRangeUser(0,2000);
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BH2TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,4);
  for( Int_t i=0; i<NumOfSegBH2; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*(NumOfSegBH2))->SetLogy();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kBH2, 0, kTDC, i+j*NumOfSegBH2+1) );
      if( !h ) continue;
      h->Draw();
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
ACs( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);

  std::vector<Int_t> adc_id = {
    HistMaker::getUniqueID(kBAC,  0, kADC, 1),
    HistMaker::getUniqueID(kBAC,  0, kADC, 2),
    HistMaker::getUniqueID(kPVAC, 0, kADC, 1),
    HistMaker::getUniqueID(kFAC,  0, kADC, 1)
  };

  std::vector<Int_t> awt_id = {
    HistMaker::getUniqueID(kBAC,  0, kADCwTDC, 1),
    HistMaker::getUniqueID(kBAC,  0, kADCwTDC, 2),
    HistMaker::getUniqueID(kPVAC, 0, kADCwTDC, 1),
    HistMaker::getUniqueID(kFAC,  0, kADCwTDC, 1)
  };

  std::vector<Int_t> tdc_id = {
    HistMaker::getUniqueID(kBAC,  0, kTDC, 1),
    HistMaker::getUniqueID(kBAC,  0, kTDC, 2),
    HistMaker::getUniqueID(kPVAC, 0, kTDC, 1),
    HistMaker::getUniqueID(kFAC,  0, kTDC, 1)
  };

  for( Int_t i=0, n=adc_id.size(); i<n; ++i ){
    c1->cd(i+1)->SetLogy();
    TH1 *h = GHist::get( adc_id[i] );
    if( !h ) continue;
    h->Draw();
    TH1 *hh = GHist::get( awt_id[i] );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->Draw("same");
    c1->cd(i+1+n)->SetLogy();
    TH1 *hhh = GHist::get( tdc_id[i] );
    if( !hhh ) continue;
    hhh->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBHTDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,4);
  for( Int_t i=0; i<NumOfSegFBH*2; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kFBH, 0, kTDC, i+1));
    if( !h ) return 0;
    h->Draw();
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
FBHTOT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,4);
  for( Int_t i=0; i<NumOfSegFBH*2; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kFBH, 0, kADC, i+1));
    if( !h ) return 0;
    h->Draw();
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
FBHHitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);

  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kFBH, 0, kTDC, NumOfSegFBH*2+1),
    HistMaker::getUniqueID(kFBH, 0, kADC, NumOfSegFBH*2+1),
    HistMaker::getUniqueID(kFBH, 0, kHitPat, 1),
    HistMaker::getUniqueID(kFBH, 0, kTDC2D, 1),
    HistMaker::getUniqueID(kFBH, 0, kADC2D, 1),
    HistMaker::getUniqueID(kFBH, 0, kMulti, 1)
  };

  c1->Divide(3,2);
  for( Int_t i=0; i<6; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( id[i] );
    if( !h ) return 0;
    h->Draw("colz");
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SFTTDCTOT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  {
    c1->cd(1)->Divide(4,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSFT, 0, kTDC, 1),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 2),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 3),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 4),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 11),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 12),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 13),
      HistMaker::getUniqueID(kSFT, 0, kTDC, 14)
    };
    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(1)->cd(i+1);
	TH1 *h = GHist::get(id[i]);
	if( !h ) continue;
	h->Draw();
    }
  }

  {
    c1->cd(2)->Divide(4,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSFT, 0, kADC, 1),
      HistMaker::getUniqueID(kSFT, 0, kADC, 2),
      HistMaker::getUniqueID(kSFT, 0, kADC, 3),
      HistMaker::getUniqueID(kSFT, 0, kADC, 4),
      HistMaker::getUniqueID(kSFT, 0, kADC, 11),
      HistMaker::getUniqueID(kSFT, 0, kADC, 12),
      HistMaker::getUniqueID(kSFT, 0, kADC, 13),
      HistMaker::getUniqueID(kSFT, 0, kADC, 14)
    };

    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(2)->cd(i+1);
	TH1 *h = GHist::get(id[i]);
	if( !h ) continue;
	h->Draw();
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SFTHitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  {
    c1->cd(1)->Divide(4,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 1),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 2),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 3),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 4),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 11),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 12),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 13),
      HistMaker::getUniqueID(kSFT, 0, kHitPat, 14)
    };
    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(1)->cd(i+1);
	TH1 *h = GHist::get(id[i]);
	if( !h ) continue;
	h->Draw();
    }
  }

  {
    c1->cd(2)->Divide(3,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSFT, 0, kMulti, 1),
      HistMaker::getUniqueID(kSFT, 0, kMulti, 2),
      HistMaker::getUniqueID(kSFT, 0, kMulti, 3),
      HistMaker::getUniqueID(kSFT, 0, kMulti, 11),
      HistMaker::getUniqueID(kSFT, 0, kMulti, 12),
      HistMaker::getUniqueID(kSFT, 0, kMulti, 13)
    };

    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(2)->cd(i+1);
	TH1 *h = GHist::get(id[i]);
	if( !h ) continue;
	h->Draw();
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SFT2D( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,1);

  {
    c1->cd(1)->Divide(4,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSFT, 0, kTDC2D, 1),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D, 2),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D, 3),
      HistMaker::getUniqueID(kSFT, 0, kTDC2D, 4),
      HistMaker::getUniqueID(kSFT, 0, kADC2D, 1),
      HistMaker::getUniqueID(kSFT, 0, kADC2D, 2),
      HistMaker::getUniqueID(kSFT, 0, kADC2D, 3),
      HistMaker::getUniqueID(kSFT, 0, kADC2D, 4)
    };
    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(1)->cd(i+1);
	TH1 *h = GHist::get(id[i]);
	if( !h ) continue;
	h->Draw("colz");
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SCHTDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,8);
  for( Int_t i=0; i<NumOfSegSCH; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSCH, 0, kTDC, i+1));
    if( !h ) return 0;
    h->Draw();
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SCHTOT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,8);
  for( Int_t i=0; i<NumOfSegSCH; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSCH, 0, kADC, i+1));
    if( !h ) return 0;
    h->Draw();
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SCHHitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);

  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kSCH, 0, kTDC, NumOfSegSCH+1),
    HistMaker::getUniqueID(kSCH, 0, kADC, NumOfSegSCH+1),
    HistMaker::getUniqueID(kSCH, 0, kHitPat, 1),
    HistMaker::getUniqueID(kSCH, 0, kTDC2D, 1),
    HistMaker::getUniqueID(kSCH, 0, kADC2D, 1),
    HistMaker::getUniqueID(kSCH, 0, kMulti, 1)
  };

  c1->Divide(3,2);
  for( Int_t i=0; i<6; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( id[i] );
    if( !h ) return 0;
    h->Draw("colz");
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
TOFADC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,6);
  for( Int_t i=0; i<NumOfSegTOF; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*NumOfSegTOF)->SetLogy();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kTOF, 0, kADC, i+j*NumOfSegTOF+1) );
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kTOF, 0, kADCwTDC, i+j*NumOfSegTOF+1) );
      if( !hh ) continue;
      hh->GetXaxis()->SetRangeUser(0,2000);
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
TOFTDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,6);
  for( Int_t i=0; i<NumOfSegTOF; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*NumOfSegTOF)->SetLogy();
      TH1 *h = GHist::get( HistMaker::getUniqueID(kTOF, 0, kTDC, i+j*NumOfSegTOF+1) );
      if( !h ) continue;
      h->Draw();
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
TOFHT( void )
{
  Int_t id = HistMaker::getUniqueID(kTOF_HT, 0, kTDC, 1);

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(5,3);
  for( Int_t i=0; i<NumOfSegTOF_HT; ++i ){
      c1->cd(i+1);
      TH1 *h = GHist::get(id+i);
      if( !h ) continue;
      h->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
LC( void )
{
  Int_t id = HistMaker::getUniqueID(kLC, 0, kTDC, 1);

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(7,4);
  for( Int_t i=0; i<NumOfSegLC; ++i ){
      c1->cd(i+1);
      TH1 *h = GHist::get(id+i);
      if( !h ) continue;
      h->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
TriggerFlag( void )
{
  Int_t id = HistMaker::getUniqueID(kTriggerFlag, 0, kTDC, 1);

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,6);

  for(int j=0; j<5; j++) {
    c1->cd(j+1)->Divide(6,1);
    for( Int_t i=0; i<6; ++i ){
      c1->cd(j+1)->cd(i+1)->SetLogy();
      TH1 *h = GHist::get(id+i+j*6);
      if( !h ) continue;
      h->Draw();
    }
  }

  Int_t id_hit = HistMaker::getUniqueID(kTriggerFlag, 0, kHitPat, 1);
  c1->cd(6);
  TH1 *hh = GHist::get(id_hit);
  hh->Draw();

  return c1;
}

//____________________________________________________________________________
TCanvas*
MsTTDC( void )
{
  Int_t id = HistMaker::getUniqueID(kMsT, 0, kTDC);

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(6,4);
  for( Int_t i=0; i<NumOfSegTOF; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1)->SetLogy();
      TH1 *h = GHist::get(id+i+j*NumOfSegTOF);
      if( !h ) continue;
      h->GetXaxis()->SetRangeUser(0,2000);
      if( j==kU )
	h->Draw();
      else {
	h->SetLineColor(kRed);
	h->Draw("same");
      }
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BC3TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,2);
  for( Int_t i=0; i<NumOfLayersBC3; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kBC3, 0, kTDC, i+1) );
    if( !h ) continue;
    h->Draw();
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kBC3, 0, kTDC2D, i+1) );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BC3HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,4);
  for( Int_t i=0; i<NumOfLayersBC3; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kBC3, 0, kHitPat, i+1) );
    if( !h ) continue;
    h->Draw();
    c1->cd(i+1+NumOfLayersBC3);
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kBC3, 0, kMulti, i+1) );
    TH1 *hh_wt = GHist::get( HistMaker::getUniqueID(kBC3, 0, kMulti, i+1 + NumOfLayersBC3) );
    if( !hh ) continue;
    if( !hh_wt ) continue;
    hh->Draw();
    hh_wt->SetLineColor(kRed);
    hh_wt->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BC4TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,2);
  for( Int_t i=0; i<NumOfLayersBC4; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kBC4, 0, kTDC, i+1) );
    if( !h ) continue;
    h->Draw();
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kBC4, 0, kTDC2D, i+1) );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BC4HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,4);
  for( Int_t i=0; i<NumOfLayersBC4; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kBC4, 0, kHitPat, i+1) );
    if( !h ) continue;
    h->Draw();
    c1->cd(i+1+NumOfLayersBC4);
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kBC4, 0, kMulti, i+1) );
    TH1 *hh_wt = GHist::get( HistMaker::getUniqueID(kBC4, 0, kMulti, i+1 + NumOfLayersBC4) );
    if( !hh ) continue;
    if( !hh_wt ) continue;
    hh->Draw();
    hh_wt->SetLineColor(kRed);
    hh_wt->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC1TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,2);
  for( Int_t i=0; i<NumOfLayersSDC1; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC1, 0, kTDC, i+1) );
    if( !h ) continue;
    h->Draw();
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC1, 0, kTDC2D, i+1) );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC1HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,4);
  for( Int_t i=0; i<NumOfLayersSDC1; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC1, 0, kHitPat, i+1) );
    if( !h ) continue;
    h->Draw();
    c1->cd(i+1+NumOfLayersSDC1);
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC1, 0, kMulti, i+1) );
    TH1 *hh_wt = GHist::get( HistMaker::getUniqueID(kSDC1, 0, kMulti, i+1 + NumOfLayersSDC1) );
    if( !hh ) continue;
    if( !hh_wt ) continue;
    hh->Draw();
    hh_wt->SetLineColor(kRed);
    hh_wt->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SAC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  {
    c1->cd(1)->Divide(2,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSAC, 0, kADC, 1),
      HistMaker::getUniqueID(kSAC, 0, kADC, 2),
      HistMaker::getUniqueID(kSAC, 0, kADC, 3),
      HistMaker::getUniqueID(kSAC, 0, kADC, 4)
    };
    std::vector<Int_t> id_wt = {
      HistMaker::getUniqueID(kSAC, 0, kADCwTDC, 1),
      HistMaker::getUniqueID(kSAC, 0, kADCwTDC, 2),
      HistMaker::getUniqueID(kSAC, 0, kADCwTDC, 3),
      HistMaker::getUniqueID(kSAC, 0, kADCwTDC, 4)
    };
    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(1)->cd(i+1)->SetLogy();
	TH1 *h = GHist::get(id[i]);
	TH1 *h_wt = GHist::get(id_wt[i]);
	if( !h ) continue;
	if( !h_wt ) continue;
	h->Draw();
	h_wt->SetLineColor(kRed);
	h_wt->Draw("same");
    }
  }

  {
    c1->cd(2)->Divide(2,2);
    std::vector<Int_t> id = {
      HistMaker::getUniqueID(kSAC, 0, kTDC, 1),
      HistMaker::getUniqueID(kSAC, 0, kTDC, 2),
      HistMaker::getUniqueID(kSAC, 0, kTDC, 3),
      HistMaker::getUniqueID(kSAC, 0, kTDC, 4)
    };

    for( Int_t i=0, n=id.size(); i<n; ++i ){
	c1->cd(2)->cd(i+1)->SetLogy();
	TH1 *h = GHist::get(id[i]);
	if( !h ) continue;
	h->GetXaxis()->SetRangeUser(0,1500);
	h->Draw();
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC2TDCTOT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,3);

  {
    c1->cd(1)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
      c1->cd(1)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kTDC, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kTDC, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  {
    c1->cd(2)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
      c1->cd(2)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kTDC2D, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kTDC2D, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  {
    c1->cd(3)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
      c1->cd(3)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kADC, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kADC, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC2HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  {
    c1->cd(1)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
      c1->cd(1)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kHitPat, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kHitPat, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  {
    c1->cd(2)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
      c1->cd(2)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kMulti, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kMulti, i + NumOfLayersSDC2 + 10) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC3TDCTOT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,3);

  {
    c1->cd(1)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
      c1->cd(1)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kTDC, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kTDC, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  {
    c1->cd(2)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
      c1->cd(2)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kTDC2D, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kTDC2D, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  {
    c1->cd(3)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
      c1->cd(3)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kADC, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kADC, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC3HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  {
    c1->cd(1)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
      c1->cd(1)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kHitPat, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kHitPat, i + 11) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  {
    c1->cd(2)->Divide(4,1);
    for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
      c1->cd(2)->cd(i+1);
      TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kMulti, i+1) );
      if( !h ) continue;
      h->Draw();
      TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kMulti, i + NumOfLayersSDC3 + 10) );
      if( !hh ) continue;
      hh->SetLineColor(kRed);
      hh->Draw("same");
    }
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
HitPatternE40( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kBH1,  0, kHitPat),
    HistMaker::getUniqueID(kBFT,  0, kHitPat),
    HistMaker::getUniqueID(kBFT,  0, kHitPat, 2),

    HistMaker::getUniqueID(kBC3,  0, kHitPat, 1),
    HistMaker::getUniqueID(kBC4,  0, kHitPat, 5),
    HistMaker::getUniqueID(kBH2,  0, kHitPat, 1)
  };
  std::vector<Int_t> id2 = {
    HistMaker::getUniqueID(kSDC1, 0, kHitPat, 1),
    HistMaker::getUniqueID(kSFT,  0, kHitPat, 1),
    HistMaker::getUniqueID(kSCH,  0, kHitPat),
    HistMaker::getUniqueID(kFBT1, 0, kHitPat, 1),

    HistMaker::getUniqueID(kSDC2, 0, kHitPat, 1),
    HistMaker::getUniqueID(kSDC3, 0, kHitPat, 3),
    HistMaker::getUniqueID(kTOF,  0, kHitPat),
    HistMaker::getUniqueID(kLC,  0, kHitPat)
  };
    // HistMaker::getUniqueID(kFBT1, 0, kHitPat),
    // HistMaker::getUniqueID(kFBT1, 0, kHitPat,FBTOffset),
    // HistMaker::getUniqueID(kFBT1, 1, kHitPat),
    // HistMaker::getUniqueID(kFBT1, 1, kHitPat,FBTOffset),
    // HistMaker::getUniqueID(kFBT2, 0, kHitPat),
    // HistMaker::getUniqueID(kFBT2, 0, kHitPat,FBTOffset),
    // HistMaker::getUniqueID(kFBT2, 1, kHitPat),
    // HistMaker::getUniqueID(kFBT2, 1, kHitPat,FBTOffset),

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);
  {
    c1->cd(1)->Divide(3,2);
    for( Int_t i=0, n=id.size(); i<n; ++i ){
      c1->cd(1)->cd(i+1);
      TH1* h = GHist::get( id.at(i) );
      if( !h ) continue;
      h->SetMinimum(0);
      h->Draw();
    }
  }
  {
    c1->cd(2)->Divide(4,2);
    for( Int_t i=0, n=id2.size(); i<n; ++i ){
      c1->cd(2)->cd(i+1);
      TH1* h = GHist::get( id2.at(i) );
      if( !h ) continue;
      h->SetMinimum(0);
      h->Draw();
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
DAQ( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kDAQ,  kEB,      kHitPat,  1),
    HistMaker::getUniqueID(kDAQ,  kVME,     kHitPat2D,  1),
    HistMaker::getUniqueID(kDAQ,  kCLite,   kHitPat2D,  1),
    HistMaker::getUniqueID(kDAQ,  kEASIROC, kHitPat2D,  1),
    HistMaker::getUniqueID(kDAQ,  kHUL,     kHitPat2D,  1),
    HistMaker::getUniqueID(kDAQ,  kOpt,     kHitPat2D,  1),
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,2);
  for( Int_t i=0, n=id.size(); i<n; ++i ){
    c1->cd(i+1)->SetGrid();
    TH1* h = GHist::get( id.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBT1TDC( void )
{
  const int FBTOffset = 200;

  std::vector<Int_t> hid = {
    HistMaker::getUniqueID(kFBT1, 0, kTDC,    NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 0, kTDC,    FBTOffset+NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 1, kTDC,    NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 1, kTDC,    FBTOffset+NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 0, kTDC2D,  1),
    HistMaker::getUniqueID(kFBT1, 0, kTDC2D,  FBTOffset+1),
    HistMaker::getUniqueID(kFBT1, 1, kTDC2D,  1),
    HistMaker::getUniqueID(kFBT1, 1, kTDC2D,  FBTOffset+1)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t i=0, n=hid.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( hid.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBT1TOT( void )
{
  const int FBTOffset = 200;

  std::vector<Int_t> hid = {
    HistMaker::getUniqueID(kFBT1, 0, kADC,    NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 0, kADC,    FBTOffset+NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 1, kADC,    NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 1, kADC,    FBTOffset+NumOfSegFBT1+1),
    HistMaker::getUniqueID(kFBT1, 0, kADC2D,  1),
    HistMaker::getUniqueID(kFBT1, 0, kADC2D,  FBTOffset+1),
    HistMaker::getUniqueID(kFBT1, 1, kADC2D,  1),
    HistMaker::getUniqueID(kFBT1, 1, kADC2D,  FBTOffset+1)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t i=0, n=hid.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( hid.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBT1HitMulti( void )
{
  const int FBTOffset = 200;

  std::vector<Int_t> hid = {
      HistMaker::getUniqueID(kFBT1, 0, kHitPat, 1),
      HistMaker::getUniqueID(kFBT1, 0, kHitPat, FBTOffset+1),
      HistMaker::getUniqueID(kFBT1, 1, kHitPat, 1),
      HistMaker::getUniqueID(kFBT1, 1, kHitPat, FBTOffset+1),
      HistMaker::getUniqueID(kFBT1, 0, kMulti,  1),
      HistMaker::getUniqueID(kFBT1, 0, kMulti,  FBTOffset+1),
      HistMaker::getUniqueID(kFBT1, 1, kMulti,  1),
      HistMaker::getUniqueID(kFBT1, 1, kMulti,  FBTOffset+1),
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t i=0, n=hid.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( hid.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBT2TDC( void )
{
  const int FBTOffset = 200;

  std::vector<Int_t> hid = {
    HistMaker::getUniqueID(kFBT2, 0, kTDC,    NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 0, kTDC,    FBTOffset+NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 1, kTDC,    NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 1, kTDC,    FBTOffset+NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 0, kTDC2D,  1),
    HistMaker::getUniqueID(kFBT2, 0, kTDC2D,  FBTOffset+1),
    HistMaker::getUniqueID(kFBT2, 1, kTDC2D,  1),
    HistMaker::getUniqueID(kFBT2, 1, kTDC2D,  FBTOffset+1)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t i=0, n=hid.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( hid.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBT2TOT( void )
{
  const int FBTOffset = 200;

  std::vector<Int_t> hid = {
    HistMaker::getUniqueID(kFBT2, 0, kADC,    NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 0, kADC,    FBTOffset+NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 1, kADC,    NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 1, kADC,    FBTOffset+NumOfSegFBT2+1),
    HistMaker::getUniqueID(kFBT2, 0, kADC2D,  1),
    HistMaker::getUniqueID(kFBT2, 0, kADC2D,  FBTOffset+1),
    HistMaker::getUniqueID(kFBT2, 1, kADC2D,  1),
    HistMaker::getUniqueID(kFBT2, 1, kADC2D,  FBTOffset+1)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t i=0, n=hid.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( hid.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
FBT2HitMulti( void )
{
  const int FBTOffset = 200;

  std::vector<Int_t> hid = {
      HistMaker::getUniqueID(kFBT2, 0, kHitPat, 1),
      HistMaker::getUniqueID(kFBT2, 0, kHitPat, FBTOffset+1),
      HistMaker::getUniqueID(kFBT2, 1, kHitPat, 1),
      HistMaker::getUniqueID(kFBT2, 1, kHitPat, FBTOffset+1),
      HistMaker::getUniqueID(kFBT2, 0, kMulti,  1),
      HistMaker::getUniqueID(kFBT2, 0, kMulti,  FBTOffset+1),
      HistMaker::getUniqueID(kFBT2, 1, kMulti,  1),
      HistMaker::getUniqueID(kFBT2, 1, kMulti,  FBTOffset+1),
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t i=0, n=hid.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( hid.at(i) );
    if( !h ) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTTDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kTDC, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTTDC2D( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kTDC2D, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTTOT( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kADC, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTTOT2D( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kADC2D, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTHighGain( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kHighGain, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTHighGain2D( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kHighGain, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTLowGain( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kLowGain, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTLowGain2D( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kLowGain, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTPedestal( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kPede, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTPedestal2D( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    TH1 *h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kPede, l+1));
    if(!h) continue;
    h->Draw("colz");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTHitPat( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  TH1 *h;
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kHitPat, l+1));
    if(!h) continue;
    h->Draw();
    h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kHitPat, l+11));
    if(!h) continue;
    h->SetLineColor(kRed);
    h->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4, 2);
  TH1 *h;
  for( Int_t l=0; l<NumOfLayersCFT; ++l ){
    c1->cd(l+1);
    h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kMulti, l+1));
    if(!h) continue;
    h->Draw();
    h = GHist::get(HistMaker::getUniqueID(kCFT, 0, kMulti, l+11));
    if(!h) continue;
    h->SetLineColor(kRed);
    h->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
HitPatternE07( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kBH1,  0, kHitPat),
    HistMaker::getUniqueID(kBFT,  0, kHitPat),
    HistMaker::getUniqueID(kBC4,  0, kHitPat, 6),
    HistMaker::getUniqueID(kFBH,  0, kHitPat),
    HistMaker::getUniqueID(kSDC1, 0, kHitPat, 1),
    HistMaker::getUniqueID(kSCH,  0, kHitPat),
    HistMaker::getUniqueID(kSDC2, 0, kHitPat, 1),
    HistMaker::getUniqueID(kTOF,  0, kHitPat)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,4);
  for( Int_t i=0, n=id.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1* h = GHist::get( id[i] );
    h->SetMinimum(0);
    h->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
EMC( void )
{
  Int_t id = HistMaker::getUniqueID(kEMC, 0, kXYpos);
  TH1 *h = GHist::get(id);
  if( !h ) return 0;
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->cd();
  h->SetStats(kFALSE);
  h->Draw("colz");
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD1ADCTDC( void )
{
  Int_t id[kAorT] = {
    HistMaker::getUniqueID(kSSD1, 0, kADC2D),
    HistMaker::getUniqueID(kSSD1, 0, kTDC2D)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD1; ++i ){
    for( Int_t j=0; j<kAorT; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD1);
      TH1 *h = GHist::get(id[j]+i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD2ADCTDC( void )
{
  Int_t id[kAorT] = {
    HistMaker::getUniqueID(kSSD2, 0, kADC2D),
    HistMaker::getUniqueID(kSSD2, 0, kTDC2D)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD2; ++i ){
    for( Int_t j=0; j<kAorT; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD2);
      TH1 *h = GHist::get(id[j]+i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD1HitMulti( void )
{
  Int_t id[kAorT] = {
    HistMaker::getUniqueID(kSSD1, 0, kHitPat),
    HistMaker::getUniqueID(kSSD1, 0, kMulti)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD1; ++i ){
    for( Int_t j=0; j<kAorT; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD1);
      TH1 *h = GHist::get(id[j]+2*i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD2HitMulti( void )
{
  Int_t id[kAorT] = {
    HistMaker::getUniqueID(kSSD2, 0, kHitPat),
    HistMaker::getUniqueID(kSSD2, 0, kMulti)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD2; ++i ){
    for( Int_t j=0; j<kAorT; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD2);
      TH1 *h = GHist::get(id[j]+2*i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD1CHitMulti( void )
{
  Int_t id[kAorT] = {
    HistMaker::getUniqueID(kSSD1, 0, kHitPat),
    HistMaker::getUniqueID(kSSD1, 0, kMulti)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD1; ++i ){
    for( Int_t j=0; j<kAorT; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD1);
      Int_t uid = j==kA ? id[j]+2*i+1 : id[j]+2*i;
      TH1 *h = GHist::get(uid);
      if( !h ) continue;
      if(j==kT) h->GetXaxis()->SetRangeUser(0,40);
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD2CHitMulti( void )
{
  Int_t id[kAorT] = {
    HistMaker::getUniqueID(kSSD2, 0, kHitPat),
    HistMaker::getUniqueID(kSSD2, 0, kMulti)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD2; ++i ){
    for( Int_t j=0; j<kAorT; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD2);
      Int_t uid = j==kA ? id[j]+2*i+1 : id[j]+2*i;
      TH1 *h = GHist::get(uid);
      if( !h ) continue;
      if(j==kT) h->GetXaxis()->SetRangeUser(0,40);
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD1dETime( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kSSD1, 0, kDeltaE),
    HistMaker::getUniqueID(kSSD1, 0, kDeltaE2D),
    HistMaker::getUniqueID(kSSD1, 0, kCTime),
    HistMaker::getUniqueID(kSSD1, 0, kCTime2D)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,4);
  for( Int_t i=0; i<NumOfLayersSSD1; ++i ){
    for( Int_t j=0, n=id.size(); j<n; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD1);
      TH1 *h = GHist::get(id[j]+i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD2dETime( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kSSD2, 0, kDeltaE),
    HistMaker::getUniqueID(kSSD2, 0, kDeltaE2D),
    HistMaker::getUniqueID(kSSD2, 0, kCTime),
    HistMaker::getUniqueID(kSSD2, 0, kCTime2D)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,4);
  for( Int_t i=0; i<NumOfLayersSSD2; ++i ){
    for( Int_t j=0, n=id.size(); j<n; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD2);
      TH1 *h = GHist::get(id[j]+i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSD12Chisqr( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kSSD1, 0, kChisqr),
    HistMaker::getUniqueID(kSSD2, 0, kChisqr)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD1; ++i ){
    for( Int_t j=0, n=id.size(); j<n; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD1);
      TH1 *h = GHist::get(id[j]+i);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
BcOutEfficiency( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kBC3, 0, kMulti, NumOfLayersBC3),
    HistMaker::getUniqueID(kBC4, 0, kMulti, NumOfLayersBC4)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(3,4);
  for( Int_t i=0; i<NumOfLayersBC3; ++i ){
    for( Int_t j=0, n=id.size(); j<n; ++j ){
      c1->cd(i+1+j*NumOfLayersBC3);
      TH1 *h = GHist::get(id[j]+i+1);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SdcInOutEfficiency( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kSDC1, 0, kMulti, NumOfLayersSDC1),
    HistMaker::getUniqueID(kSDC2, 0, kMulti, NumOfLayersSDC2),
    HistMaker::getUniqueID(kSDC3, 0, kMulti, NumOfLayersSDC3)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(1,2);

  c1->cd(1)->Divide(3,2);
  for( Int_t i=0; i<NumOfLayersSDC1; ++i ){
    c1->cd(1)->cd(i+1);
    TH1 *h = GHist::get(id[0]+i+1);
    if( !h ) continue;
    h->Draw();
  }

  c1->cd(2)->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
    c1->cd(2)->cd(i+1);
    TH1 *h = GHist::get(id[1]+i+1);
    if( !h ) continue;
    h->Draw();
  }

  for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
    c1->cd(2)->cd(i+1+NumOfLayersSDC2);
    TH1 *h = GHist::get(id[2]+i+1);
    if( !h ) continue;
    h->Draw();
  }

  return c1;
}

//____________________________________________________________________________
TCanvas*
CFTEfficiency( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,4);
  TH1 *h;
  for( Int_t i=0; i<NumOfLayersCFT; ++i ){
    c1->cd(i+1);
    h = GHist::get( HistMaker::getUniqueID(kCFT, 0, kMulti, i+11) );
    if( !h ) continue;
    h->Draw();
    c1->cd(i+1+NumOfLayersCFT);
    h = GHist::get( HistMaker::getUniqueID(kCFT, 0, kMulti, i+21) );
    if( !h ) continue;
    h->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SSDEfficiency( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kSSD1, 0, kMulti),
    HistMaker::getUniqueID(kSSD2, 0, kMulti)
  };

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSSD1; ++i ){
    for( Int_t j=0, n=id.size(); j<n; ++j ){
      c1->cd(i+1+j*NumOfLayersSSD1);
      TH1 *h = GHist::get(id[j]+2*i+1);
      if( !h ) continue;
      h->Draw("colz");
    }
  }
  return c1;
}

//____________________________________________________________________________
void
UpdateBcOutEfficiency( void )
{
  static TCanvas *c1 = (TCanvas*)gROOT->FindObject("BcOutEfficiency");
  static std::vector<TString> name = {
    "BC3_Multi_x0", "BC3_Multi_x1", "BC3_Multi_v0", "BC3_Multi_v1", "BC3_Multi_u0", "BC3_Multi_u1",
    "BC4_Multi_u0", "BC4_Multi_u1", "BC4_Multi_v0", "BC4_Multi_v1", "BC4_Multi_x0", "BC4_Multi_x1"
  };
  static std::vector<TText*> tex(name.size());
  for( Int_t i=0, n=name.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1 *h = (TH1*)gPad->FindObject(name[i]+"_wTDC");
    if( !h ) continue;
    Double_t zero = h->GetBinContent(1);
    Double_t all  = h->GetEntries();
    Double_t eff  = 1. - zero/all;
    if( tex[i] ) delete tex[i];
    tex[i] = new TText;
    tex[i]->SetNDC();
    tex[i]->SetTextSize(0.130);
    tex[i]->SetText(0.400,0.600,Form("eff. %.3f",
				     eff));
    tex[i]->Draw();
  }
}

//____________________________________________________________________________
void
UpdateSdcInOutEfficiency( void )
{
  static TCanvas *c1 = (TCanvas*)gROOT->FindObject("SdcInOutEfficiency");
  static std::vector<TString> name = {
    "SDC1_Multi_v0", "SDC1_Multi_v1", "SDC1_Multi_x0", "SDC1_Multi_x1", "SDC1_Multi_u0", "SDC1_Multi_u1",
    "SDC2_Multi_x0", "SDC2_Multi_x1", "SDC2_Multi_y0", "SDC2_Multi_y1",
    "SDC3_Multi_y0", "SDC3_Multi_y1", "SDC3_Multi_x0", "SDC3_Multi_x1"
  };
  static std::vector<TText*> tex(name.size());

  for( Int_t i=0, n=name.size(); i<n; ++i ){
    if( i<NumOfLayersSDC1 )
      c1->cd(1)->cd(i+1);
    else
      c1->cd(2)->cd(i+1-NumOfLayersSDC1);
    TH1 *h = (TH1*)gPad->FindObject(name[i]+"_wTDC");
    if( !h ) continue;
    Double_t zero = h->GetBinContent(1);
    Double_t all  = h->GetEntries();
    Double_t eff  = 1. - zero/all;
    if( tex[i] ) delete tex[i];
    tex[i] = new TText;
    tex[i]->SetNDC();
    tex[i]->SetTextSize(0.130);
    tex[i]->SetText(0.300,0.600,Form("eff. %.3f",
				     eff));
    tex[i]->Draw();
  }
}

//____________________________________________________________________________
void
UpdateCFTEfficiency( void )
{
  static TCanvas *c1 = (TCanvas*)gROOT->FindObject("CFTEfficiency");
  static std::vector<TString> name = {
    "CFT_CMulti_UV1", "CFT_CMulti_PHI1", "CFT_CMulti_UV2", "CFT_CMulti_PHI2",
    "CFT_CMulti_UV3", "CFT_CMulti_PHI3", "CFT_CMulti_UV4", "CFT_CMulti_PHI4",
    "CFT_CMulti_UV1_wBGO", "CFT_CMulti_PHI1_wBGO",
    "CFT_CMulti_UV2_wBGO", "CFT_CMulti_PHI2_wBGO",
    "CFT_CMulti_UV3_wBGO", "CFT_CMulti_PHI3_wBGO",
    "CFT_CMulti_UV4_wBGO", "CFT_CMulti_PHI4_wBGO"
  };
  static std::vector<TText*> tex(name.size());

  for( Int_t i=0, n=name.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1 *h = (TH1*)gPad->FindObject(name[i]);
    if( !h ){ std::cout << "!?!?!?" << std::endl; continue; }
    Double_t zero = h->GetBinContent(1);
    Double_t all  = h->GetEntries();
    Double_t eff  = 1. - zero/all;
    if( tex[i] ) delete tex[i];
    tex[i] = new TText;
    tex[i]->SetNDC();
    tex[i]->SetTextSize(0.130);
    tex[i]->SetText(0.300,0.600,Form("eff. %.3f",
				     eff));
    tex[i]->Draw();
  }
}

//____________________________________________________________________________
void
UpdateSSDEfficiency( void )
{
  static TCanvas *c1 = (TCanvas*)gROOT->FindObject("SSDEfficiency");
  static std::vector<TString> name = {
    "SSD1_CMulti_y0", "SSD1_CMulti_x0", "SSD1_CMulti_y1", "SSD1_CMulti_x1",
    "SSD2_CMulti_x0", "SSD2_CMulti_y0", "SSD2_CMulti_x1", "SSD2_CMulti_y1"
  };
  static std::vector<TText*> tex(name.size());
  for( Int_t i=0, n=name.size(); i<n; ++i ){
    c1->cd(i+1);
    TH1 *h = (TH1*)gPad->FindObject(name[i]);
    if( !h ) continue;
    Double_t zero = h->GetBinContent(1);
    Double_t all  = h->GetEntries();
    Double_t eff  = 1. - zero/all;
    if( tex[i] ) delete tex[i];
    tex[i] = new TText;
    tex[i]->SetNDC();
    tex[i]->SetTextSize(0.100);
    tex[i]->SetText(0.500,0.600,Form("eff. %.3f",
				     eff));
    tex[i]->Draw();
  }
}

}

}
