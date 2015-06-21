#ifndef MACROBUILDER_H
#define MACROBUILDER_H
class TObject;

TObject* hoge();
TObject* clear_all_canvas();
TObject* clear_canvas();
TObject* split22();
TObject* split32();
TObject* split33();
TObject* dispBH1();
TObject* dispBFT();
TObject* dispBH2();
TObject* dispACs_SFV();
TObject* dispSP0Adc();
TObject* dispSP0Tdc();
TObject* dispTOF();
TObject* dispLC();
TObject* dispBC3();
TObject* dispBC4();
TObject* dispSDC2();
TObject* dispHDC();
TObject* dispSDC3();
TObject* dispSDC4();
TObject* dispHitPat();
TObject* effBcOut();
TObject* effSdcIn();
TObject* effSdcOut();

TObject* dispBeamProfile();
TObject* dispBeamProfile_e07();

TObject* dispGeAdc();
TObject* dispGeAdc_60Co();
TObject* dispGeAdc_60Co_1170();
TObject* dispGeAdc_60Co_1330();
TObject* dispGeTdc();
TObject* dispPWOTdc();

TObject* auto_monitor_all();

#endif
