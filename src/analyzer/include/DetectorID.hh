#ifndef DETECTOR_ID_HH
#define DETECTOR_ID_HH

const int DetIdBC1  =101;
const int DetIdBC2  =102;
const int DetIdBC3  =103;
const int DetIdBC4  =104;
const int DetIdSDC1 =105;
const int DetIdSDC2 =106;
const int DetIdHDC  =113;

const int DetIdSDC3 =107;
const int DetIdSDC4 =108;
const int DetIdK6BDC=109;
const int DetIdBFT  =110;
const int DetIdSFT  =111;
//const int DetIdSSD  =112;

const int DetIdBH1  =  3;
const int DetIdBH2  =  4;
const int DetIdTOF  =  5;
const int DetIdLAC  =  6;
const int DetIdLC   =  7;
const int DetIdGC   =  8;
//const int DetIdBAC  =  9;
const int DetIdYOON =  9;
const int DetIdRC   = 21;
const int DetIdAC1  = 22;
const int DetIdBVH  = 23;
const int DetIdNRC  = 24;

const int DetIdMisc    = 11;
const int NumOfSegMisc = 16;

// E07 Detectors
const int DetIdBH2_E07 =  33;
const int DetIdBAC_E07 =  34;
const int DetIdPVAC    =  35;
const int DetIdFAC     =  36;
const int DetIdSAC1    =  37;
const int DetIdSCH     =  38;
const int DetIdEMC     =  39;
const int DetIdFBH     =  40;
const int DetIdKFAC    =  44;
const int DetIdSSDT    = 119;
const int DetIdSSD0    = 120;
const int DetIdSSD1    = 121;
const int DetIdSSD2    = 122;
const int NumOfSegBH2_E07 =  2;
const int NumOfSegBAC_E07 =  4 + 2;
const int NumOfSegPVAC    =  4 + 1;
const int NumOfSegFAC     =  6 + 1;
const int NumOfSegSAC1    =  1;
const int NumOfSegSCH     = 64;
const int NumOfSegEMC     =  1;
const int NumOfSegFBH     = 16;
const int NumOfSegKFAC    =  2;
const int NumOfLayersSSD0 =  2;
const int NumOfLayersSSD1 =  4;
const int NumOfLayersSSD2 =  4;
const int NumOfSegSSDT    = 16;
const int NumOfSegSSD0    = 1536;
const int NumOfSegSSD1    = 1536;
const int NumOfSegSSD2    = 1536;
const int NumOfSamplesSSD =  8;

// E05 Detectors
const int DetIdMsT     = 32;
const int DetIdKIC     = 41;
const int NumOfSegMsT  = 32;
const int NumOfSegKIC  =  4;

// E13 Detectors
const int DetIdBAC     =  9;
const int DetIdBMW     = 10;
const int DetIdMatrix  = 12;
const int DetIdSFV     = 24;
const int DetIdTOFMT   = 25;
const int DetIdSP0     = 26;
const int DetIdGe      = 27;
const int DetIdPWO     = 28;
const int DetIdPWOADC  = 29;
const int NumOfSegBAC = 2;
const int NumOfSegBAC_SAC = 6;
const int NumOfSegSFV = 18;
const int NumOfSegGe   = 32;

const int NumOfSegPWO  = 238;
const int NumOfBoxPWO  = 22;
const int NumOfUnitPWO[NumOfBoxPWO] =
  {
    6, 9, 6, 14, 14, 21, 14, 14, 6, 9, 6,
    6, 9, 6, 14, 14, 21, 14, 14, 6, 9, 6
  };

const int SegIdPWO[NumOfBoxPWO] =
  {
    4, 0, 0, 4, 0, 0, 1, 5, 2, 1, 6,
    1, 2, 5, 6, 2, 1, 3, 7, 7, 3, 3,
  };

const int NumOfLayersSP0 = 8;
const int NumOfSegSP0    = 5;

//
const int SegIdScalerTrigger = 15;

const int NumOfSegBH1 = 11;
const int NumOfSegBH2 =  5;
const int NumOfSegTOF = 32;
const int NumOfSegLAC = 30;
const int NumOfSegLC  = 28;
const int NumOfSegBFT = 160;

const int UorD = 2;
const int AorT = 2;

const int DetIdBcIn   = 10;
const int DetIdBcOut  = 20;
const int DetIdSdcIn  = 30;
const int DetIdSdcOut = 40;
const int DetIdSsd    = 50;

const int NumOfLayersBcIn   = 12;
const int NumOfLayersBcOut  = 12;
const int NumOfLayersSdcIn  = 10;
const int NumOfLayersSdcOut = 12;
const int NumOfLayersSsd    = 10;

const int NumOfLayersBC1  = 6;
const int NumOfLayersBC2  = 6;
const int NumOfLayersBC3  = 6;
const int NumOfLayersBC4  = 6;
const int NumOfLayersSDC1 = 4;
const int NumOfLayersSDC2 = 6;
const int NumOfLayersHDC  = 4;
const int NumOfLayersSDC3 = 6;
const int NumOfLayersSDC4 = 6;

const int NumOfWireBC1 = 256;
const int NumOfWireBC2 = 256;
const int NumOfWireBC3 = 64;
const int NumOfWireBC4 = 64;
const int NumOfWireSDC1 = 64;
const int NumOfWireSDC2 = 96;
const int NumOfWireHDC  = 112;
const int NumOfWireSDC3x = 108;
const int NumOfWireSDC4x = 108;
const int NumOfWireSDC3uv = 120;
const int NumOfWireSDC4uv = 120;
const int PlMinBdcIn = 1;
const int PlMaxBdcIn = 12;
const int PlMinBdcOut =13;
const int PlMaxBdcOut =24;

const int PlMinSdcIn = 1;
const int PlMaxSdcIn =10;
const int PlMinSdcOut=11;
const int PlMaxSdcOut=22;

#endif
