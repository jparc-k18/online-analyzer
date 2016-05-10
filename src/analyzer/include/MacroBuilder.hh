#ifndef MACROBUILDER_H
#define MACROBUILDER_H
class TObject;

TObject* hoge( void );
TObject* clear_all_canvas( void );
TObject* clear_canvas( void );
TObject* split22( void );
TObject* split32( void );
TObject* split33( void );
TObject* dispBH1( void );
TObject* dispBFT( void );
TObject* dispBH2( void );
TObject* dispACs( void );
TObject* dispFBH( void );
TObject* dispSSD1( void );
TObject* dispSSD2( void );
TObject* dispSSDHitPat( void );
TObject* dispSCH( void );
TObject* dispTOF( void );
TObject* dispMsT( void );
TObject* dispBC3( void );
TObject* dispBC4( void );
TObject* dispSDC1( void );
TObject* dispSDC2( void );
TObject* dispSDC3( void );
TObject* dispHitPat( void );
TObject* dispProfileSSD( void );
TObject* effBcOut( void );
TObject* effSdcIn( void );
TObject* effSdcOut( void );

TObject* dispBeamProfile( void );

/* old functions */
TObject* dispBAC( void );
TObject* dispACs_SFV( void );
TObject* dispGeAdc( void );
TObject* dispGeAdc_60Co( void );
TObject* dispGeAdc_60Co_1170( void );
TObject* dispGeAdc_60Co_1330( void );
TObject* dispGeTdc( void );
TObject* dispPWOTdc( void );
TObject* dispBH2_E07( void );
TObject* dispACs_E07( void );
TObject* dispHitPatE07( void );
TObject* dispSSD0( void );
TObject* dispPWO_E05( void );
TObject* dispKIC( void );
TObject* dispSP0Adc( void );
TObject* dispSP0Tdc( void );
TObject* dispLAC( void );
TObject* dispLC( void );
TObject* dispHDC( void );
TObject* dispSDC4( void );
TObject* dispBeamProfile_e07( void );
TObject* dispBeamProfile_e05( void );

TObject* auto_monitor_all( void );

#endif
