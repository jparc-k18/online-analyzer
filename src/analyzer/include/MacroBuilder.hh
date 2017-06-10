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
  TCanvas* BH1ADC( void );
  TCanvas* BH1TDC( void );
  TCanvas* BFT( void );
  TCanvas* BH2( void );
  TCanvas* ACs( void );
  TCanvas* FBHTDC( void );
  TCanvas* FBHTOT( void );
  TCanvas* FBHHitMulti( void );
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
  TCanvas* SDC2TDC( void );
  TCanvas* SDC2HitMulti( void );
  TCanvas* SDC3TDC( void );
  TCanvas* SDC3HitMulti( void );

  // E07
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

  void     UpdateBcOutEfficiency( void );
  void     UpdateSdcInOutEfficiency( void );
  void     UpdateSSDEfficiency( void );
}

};

#endif
