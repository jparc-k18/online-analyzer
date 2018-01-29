#ifndef DETECTOR_ID_HH
#define DETECTOR_ID_HH

const int UorD = 2;
const int AorT = 2;

// Counters
const int DetIdBH1  =  1;
const int DetIdBH2  =  2;
const int DetIdBAC  =  3;
const int DetIdSAC  =  9;
const int DetIdSCH  =  7;
const int DetIdBGO  = 114;
const int DetIdPiID = 115;
const int DetIdTOF  =  8;
const int DetIdTOF_HT  =  11;
const int DetIdLC   = 10; //add
const int NumOfSegBH1  = 11;
const int NumOfSegBH2  =  8; //add
const int NumOfSegBAC  =  2;
const int NumOfSegFBH  = 16;
//const int NumOfSegSAC  = 16; // 12 readout & 4 rooms //add
const int NumOfSegSCH  = 64;
const int NumOfSegBGO  = 24;
const int NumOfSegPiID = 32;
const int NumOfSegTOF  = 24;
//const int NumOfSegTOF_HT  = 24;
const int NumOfSegTOF_HT  = 15;
const int NumOfSegClusteredFBH = 31;
const int NumOfRoomsSAC  = 4;
const int NumOfSegLC  = 28;

// Misc
const int DetIdTFlag     = 21;
const int DetIdScaler    = 22;
const int NumOfSegTFlag  = 32;
const int NumOfSegScaler = 32;
const int SpillEndFlag   = 27; // 0-based
const int NumOfVmeRm     = 10;

// Trackers
const int DetIdBC3  = 103;
const int DetIdBC4  = 104;
const int DetIdSDC1 = 105;
const int DetIdSDC2 = 106;
const int DetIdSDC3 = 107;
const int DetIdBFT  = 110;
const int DetIdSFT  = 111;
const int DetIdCFT  = 113;
const int DetIdFBT1 = 131; //add
const int DetIdFBT2 = 132; //add
const int DetIdBcOut  = 20;
const int DetIdSdcIn  = 30;
const int DetIdSdcOut = 40;

const int NumOfLayersBcOut  = 12;
const int NumOfLayersSdcIn  =  6;
const int NumOfLayersSdcOut =  8;
const int NumOfLayersSsdIn  =  4;
const int NumOfLayersSsdOut =  4;
const int NumOfLayersBC3  = 6;
const int NumOfLayersBC4  = 6;
const int NumOfLayersSFT  = 4; //add
const int NumOfLayersCFT  =8;
const int NumOfLayersFBT = 2; //add
const int NumOfLayersSDC1 = 6;
const int NumOfLayersSDC2 = 4;
const int NumOfLayersSDC3 = 4;
const int NumOfWireBC3   =  64;
const int NumOfWireBC4   =  64;
const int NumOfWireSDC1  =  64;
const int NumOfWireSDC2  = 128;
const int NumOfWireSDC3X =  96;
const int NumOfWireSDC3Y =  64;

const int PlMinBdcOut = 13;
const int PlMaxBdcOut = 24;
const int PlMinSdcIn  =  1;
const int PlMaxSdcIn  =  6;
const int PlMinSdcOut = 31;
const int PlMaxSdcOut = 38;

const int NumOfSegBFT  =  160;
const int NumOfSegSFT_X =  256; //add
const int NumOfSegSFT_UV =  320; //add
const int NumOfSegCFT_UV1   = 426;
const int NumOfSegCFT_PHI1  = 584;
const int NumOfSegCFT_UV2   = 472 + 10; //471
const int NumOfSegCFT_PHI2  = 692;
const int NumOfSegCFT_UV3   = 510;
const int NumOfSegCFT_PHI3  = 800;
const int NumOfSegCFT_UV4   = 538;
const int NumOfSegCFT_PHI4  = 928;//910
const int NumOfSegCFT[NumOfLayersCFT] = {
                            NumOfSegCFT_UV1  , 
                            NumOfSegCFT_PHI1 , 
                            NumOfSegCFT_UV2  , 
                            NumOfSegCFT_PHI2 , 
                            NumOfSegCFT_UV3  , 
                            NumOfSegCFT_PHI3 , 
                            NumOfSegCFT_UV4  , 
                            NumOfSegCFT_PHI4 }; 
const int NumOfSegFBT1 =  48; //add
const int NumOfSegFBT2 =  64; //add

// Tracker const
const int FBTOffset = 200;
const int NumOfUDStructureFBT = 2;
// Old Detectors
// E07 Detectors
const int DetIdFBH  =  4;
const int DetIdPVAC =  5;
const int DetIdFAC  =  6;
const int DetIdEMC      = 23;
const int DetIdSSDT     = 24;
const int NumOfSegPVAC =  1;
const int NumOfSegFAC  =  1;
const int NumOfSegEMC      =  1;
const int NumOfSegSSDT     = 16;
const int DetIdSSD1 = 121;
const int DetIdSSD2 = 122;
const int DetIdSsd    = 50;
const int NumOfLayersSsd  = 8;
const int NumOfLayersSSD1 = 4;
const int NumOfLayersSSD2 = 4;
const int NumOfSamplesSSD = 8;
const int NumOfSegSSD1 = 1536;
const int NumOfSegSSD2 = 1536;
const int DetIdLAC  =  6;
const int DetIdGC   =  8;
const int DetIdYOON =  9;
const int DetIdRC   = 21;
const int DetIdAC1  = 22;
const int DetIdBVH  = 23;
const int DetIdNRC  = 24;
const int DetIdBH2_E07 =  33;
const int DetIdBAC_E07 =  34;
const int DetIdSAC1    =  37;
const int DetIdKFAC    =  44;
const int NumOfSegLAC = 30;
const int NumOfSegBH2_E07 =  2;
const int NumOfSegBAC_E07 =  4 + 2;
const int NumOfSegSAC1    =  1;
const int NumOfSegKFAC    =  2;
const int NumOfLayersSSD0 =  2;
// E05 Detectors
const int DetIdMsT     = 32;
const int DetIdMsTRM   = 82;
const int DetIdKIC     = 41;
const int NumOfSegMsT  = 24;
const int NumOfSegKIC  =  4;
// E13 Detectors
const int DetIdBMW     = 10;
const int DetIdMatrix  = 12;
const int DetIdSFV     = 24;
const int DetIdTOFMT   = 25;
const int DetIdSP0     = 26;
const int DetIdGe      = 27;
const int DetIdPWO     = 28;
const int DetIdPWOADC  = 29;
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

const int DetIdBC1   = 101;
const int DetIdBC2   = 102;
const int DetIdSDC4  = 108;
const int DetIdK6BDC = 109;
const int DetIdHDC   = 113;
const int DetIdSSD0  = 120;

const int DetIdBcIn   = 10;

const int NumOfLayersBC1  =  6;
const int NumOfLayersBC2  =  6;
const int NumOfLayersBcIn = 12;
const int NumOfLayersHDC  =  4;
const int NumOfLayersSDC4 =  6;

const int NumOfWireBC1    =  256;
const int NumOfWireBC2    =  256;
const int NumOfWireHDC    =  112;
const int NumOfWireSDC3x  =  108;
const int NumOfWireSDC4x  =  108;
const int NumOfWireSDC3uv =  120;
const int NumOfWireSDC4uv =  120;
const int NumOfSegSSD0    = 1536;
const int PlMinBdcIn  =  1;
const int PlMaxBdcIn  = 12;


#endif
