// -*- C++ -*-

#include "MacroBuilder.hh"

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
  Int_t id = HistMaker::getUniqueID(kBH1, 0, kTDC);

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(6,4);
  for( Int_t i=0; i<NumOfSegBH1; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*(NumOfSegBH1+1))->SetLogy();
      TH1 *h = GHist::get(id+i+j*NumOfSegBH1);
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
      HistMaker::getUniqueID(kBFT, 0, kTDC),
      HistMaker::getUniqueID(kBFT, 0, kADC),
      HistMaker::getUniqueID(kBFT, 0, kHitPat),
      HistMaker::getUniqueID(kBFT, 0, kMulti)
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
BH2( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(2,2);
  Int_t icanvas = 0;
  for( Int_t j=0; j<kUorD; ++j ){
    c1->cd(++icanvas)->SetLogy();
    TH1 *h = GHist::get( HistMaker::getUniqueID(kBH2, 0, kADC, j+1) );
    if( !h ) continue;
    h->GetXaxis()->SetRangeUser(0,2000);
    h->Draw();
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kBH2, 0, kADCwTDC, j+1) );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->GetXaxis()->SetRangeUser(0,2000);
    hh->Draw("same");
    c1->cd(++icanvas)->SetLogy();
    TH1 *hhh = GHist::get( HistMaker::getUniqueID(kBH2, 0, kTDC, j+1) );
    if( !hhh ) continue;
    hhh->GetXaxis()->SetRangeUser(0,2000);
    hhh->Draw();
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
  Int_t id = HistMaker::getUniqueID(kTOF, 0, kTDC);

  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(8,6);
  for( Int_t i=0; i<NumOfSegTOF; ++i ){
    for( Int_t j=0; j<kUorD; ++j ){
      c1->cd(i+1+j*NumOfSegTOF)->SetLogy();
      TH1 *h = GHist::get(id+i+j*NumOfSegTOF);
      if( !h ) continue;
      h->Draw();
    }
  }
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
    if( !hh ) continue;
    hh->Draw();
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
    if( !hh ) continue;
    hh->Draw();
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
    if( !hh ) continue;
    hh->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC2TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(2,2);
  for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kTDC, i+1) );
    if( !h ) continue;
    h->Draw();
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kTDC2D, i+1) );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC2HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSDC2; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kHitPat, i+1) );
    if( !h ) continue;
    h->Draw();
    c1->cd(i+1+NumOfLayersSDC2);
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC2, 0, kMulti, i+1) );
    if( !hh ) continue;
    hh->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC3TDC( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(2,2);
  for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kTDC, i+1) );
    if( !h ) continue;
    h->Draw();
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kTDC2D, i+1) );
    if( !hh ) continue;
    hh->SetLineColor(kRed);
    hh->Draw("same");
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
SDC3HitMulti( void )
{
  TCanvas *c1 = new TCanvas(__func__, __func__);
  c1->Divide(4,2);
  for( Int_t i=0; i<NumOfLayersSDC3; ++i ){
    c1->cd(i+1);
    TH1 *h = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kHitPat, i+1) );
    if( !h ) continue;
    h->Draw();
    c1->cd(i+1+NumOfLayersSDC3);
    TH1 *hh = GHist::get( HistMaker::getUniqueID(kSDC3, 0, kMulti, i+1) );
    if( !hh ) continue;
    hh->Draw();
  }
  return c1;
}

//____________________________________________________________________________
TCanvas*
HitPattern( void )
{
  std::vector<Int_t> id = {
    HistMaker::getUniqueID(kBH1,  0, kHitPat),
    HistMaker::getUniqueID(kBFT,  0, kHitPat),
    HistMaker::getUniqueID(kBC4,  0, kHitPat, 6),
    HistMaker::getUniqueID(kFBH,  0, kHitPat),
    HistMaker::getUniqueID(kSDC1, 0, kHitPat, 1),
    HistMaker::getUniqueID(kSCH,  0, kHitPat),
    HistMaker::getUniqueID(kFBT1,  0, kHitPat),
    HistMaker::getUniqueID(kFBT1,  0, kHitPat,FBTOffset),
    HistMaker::getUniqueID(kFBT1,  1, kHitPat),
    HistMaker::getUniqueID(kFBT1,  1, kHitPat,FBTOffset),
    HistMaker::getUniqueID(kSDC2, 0, kHitPat, 1),
    HistMaker::getUniqueID(kFBT2,  0, kHitPat),
    HistMaker::getUniqueID(kFBT2,  0, kHitPat,FBTOffset),
    HistMaker::getUniqueID(kFBT2,  1, kHitPat),
    HistMaker::getUniqueID(kFBT2,  1, kHitPat,FBTOffset),
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
    tex[i]->SetTextSize(0.160);
    tex[i]->SetText(0.600,0.600,Form("eff. %.3f",
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
    tex[i]->SetTextSize(0.160);
    tex[i]->SetText(0.600,0.600,Form("eff. %.3f",
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
