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
  TCanvas* Correlation( void );
  TCanvas* CorrelationFHT( void );
  TCanvas* HitPattern( void );
  TCanvas* TriggerFlag( void );
  TCanvas* BH1ADC( void );
  TCanvas* BH1TDC( void );
  TCanvas* BFT( void );
  TCanvas* BH2ADC( void );
  TCanvas* BH2TDC( void );
  TCanvas* ACs( void );
  TCanvas* FBHTDC( void );
  TCanvas* FBHTOT( void );
  TCanvas* FBHHitMulti( void );
  TCanvas* FHT1TDC( void );
  TCanvas* FHT1TOT( void );
  TCanvas* FHT1HitMulti( void );
  TCanvas* FHT2TDC( void );
  TCanvas* FHT2TOT( void );
  TCanvas* FHT2HitMulti( void );
  TCanvas* SFTTDCTOT( void );
  TCanvas* SFTHitMulti( void );
  TCanvas* SFT2D( void );
  TCanvas* SCHTDC( void );
  TCanvas* SCHTOT( void );
  TCanvas* SCHHitMulti( void );
  TCanvas* TOFADC( void );
  TCanvas* TOFTDC( void );
  TCanvas* TOF_HT( void );
  TCanvas* LAC( void );
  TCanvas* LC( void );
  TCanvas* MsTTDC( void );
  TCanvas* BC3TDCTOT( void );
  TCanvas* BC3HitMulti( void );
  TCanvas* BC4TDCTOT( void );
  TCanvas* BC4HitMulti( void );
  TCanvas* SDC1TDCTOT( void );
  TCanvas* SDC1HitMulti( void );
  TCanvas* SAC( void );
  TCanvas* SDC2TDCTOT( void );
  TCanvas* SDC2HitMulti( void );
  TCanvas* SDC3TDCTOT( void );
  TCanvas* SDC3HitMulti( void );
  TCanvas* DAQ( void );

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
  TCanvas* FHT1TDC( void );
  TCanvas* FHT1TOT( void );
  TCanvas* FHT1HitMulti( void );
  TCanvas* FHT2TDC( void );
  TCanvas* FHT2TOT( void );
  TCanvas* FHT2HitMulti( void );
  TCanvas* CFTTDC( void );
  TCanvas* CFTTDC2D( void );
  TCanvas* CFTTOT( void );
  TCanvas* CFTTOT2D( void );
  TCanvas* CFTHighGain( void );
  TCanvas* CFTHighGain2D( void );
  TCanvas* CFTLowGain( void );
  TCanvas* CFTLowGain2D( void );
  TCanvas* CFTPedestal( void );
  TCanvas* CFTPedestal2D( void );
  TCanvas* CFTHitPat( void );
  TCanvas* CFTMulti( void );
  TCanvas* CFTEfficiency( void );
  TCanvas* CFTClusterHighGain( void );
  TCanvas* CFTClusterHighGain2D( void );
  TCanvas* CFTClusterLowGain( void );
  TCanvas* CFTClusterLowGain2D( void );
  TCanvas* CFTClusterTDC( void );
  TCanvas* CFTClusterTDC2D( void );
  TCanvas* BGOFADC( void );
  TCanvas* BGOADC( void );
  TCanvas* BGOTDC( void );
  TCanvas* BGOADCTDC2D( void );
  TCanvas* BGOHitMulti( void );
  TCanvas* PiIDTDC( void );
  TCanvas* PiIDHighGain( void );
  TCanvas* PiIDLowGain( void );
  TCanvas* PiIDHitMulti( void );
  void     UpdateBcOutEfficiency( void );
  void     UpdateSdcInOutEfficiency( void );
  void     UpdateCFTEfficiency( void );
  void     UpdateSSDEfficiency( void );
}

};

#endif
