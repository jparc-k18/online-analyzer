// -*- C++ -*-

#ifndef MACRO_BUILDER_HH
#define MACRO_BUILDER_HH

class TCanvas;
class TObject;
class TString;

namespace analyzer
{

//______________________________________________________________________________
namespace macro
{
  TObject* Get( TString name );
}

//______________________________________________________________________________
// for HttpServer
namespace http
{
  TCanvas* HitPattern( void );
  TCanvas* BH1ADC( void );
  TCanvas* BH1TDC( void );
  TCanvas* BFT( void );
  TCanvas* BH2ADC( void );
  TCanvas* BH2TDC( void );
  TCanvas* ACs( void );
  TCanvas* FBHTDC( void );
  TCanvas* FBHTOT( void );
  TCanvas* FBHHitMulti( void );
  TCanvas* SFT( void );
  TCanvas* SCHTDC( void );
  TCanvas* SCHTOT( void );
  TCanvas* SCHHitMulti( void );
  TCanvas* TOFADC( void );
  TCanvas* TOFTDC( void );
  TCanvas* MsTTDC( void );
  TCanvas* BC3TDC( void );
  TCanvas* BC3HitMulti( void );
  TCanvas* BC4TDC( void );
  TCanvas* BC4HitMulti( void );
  TCanvas* SDC1TDC( void );
  TCanvas* SDC1HitMulti( void );
  TCanvas* SAC( void );
  TCanvas* SDC2TDC( void );
  TCanvas* SDC2HitMulti( void );
  TCanvas* SDC3TDC( void );
  TCanvas* SDC3HitMulti( void );

  // E07
  TCanvas* HitPatternE07( void );
  TCanvas* SSD1ADCTDC( void );
  TCanvas* SSD2ADCTDC( void );
  TCanvas* SSD1HitMulti( void );
  TCanvas* SSD2HitMulti( void );
  TCanvas* SSD1CHitMulti( void );
  TCanvas* SSD2CHitMulti( void );
  TCanvas* SSD1dETime( void );
  TCanvas* SSD2dETime( void );
  TCanvas* SSD12Chisqr( void );
  TCanvas* BcOutEfficiency( void );
  TCanvas* SdcInOutEfficiency( void );
  TCanvas* SSDEfficiency( void );
  TCanvas* EMC( void );

  // E40
  TCanvas* HitPatternE40( void );
  TCanvas* SFT( void );
  TCanvas* FBT1TDC( void );
  TCanvas* FBT1TOT( void );
  TCanvas* FBT1HitMulti( void );
  TCanvas* FBT2TDC( void );
  TCanvas* FBT2TOT( void );
  TCanvas* FBT2HitMulti( void );

  void     UpdateBcOutEfficiency( void );
  void     UpdateSdcInOutEfficiency( void );
  void     UpdateSSDEfficiency( void );
}

};

#endif
