// -*- C++ -*-

#ifndef DETECTOR_ID_HH
#define DETECTOR_ID_HH

#include <Rtypes.h>
#include <TString.h>

const Int_t UorD = 2;
const Int_t AorT = 2;

// Counters
const Int_t DetIdBH1     =   1;
const Int_t DetIdBH2     =   2;
const Int_t DetIdBAC     =   3;
const Int_t DetIdSAC     =   9;
const Int_t DetIdSCH     =   7;
const Int_t DetIdBGO     = 114;
const Int_t DetIdPiID    = 115;
const Int_t DetIdTOF     =   8;
const Int_t DetIdTOF_HT  =  11;
const Int_t DetIdLC      =  10;
const Int_t NumOfSegBH1  =  11;
const Int_t NumOfSegBH2  =   8;
const Int_t NumOfSegBAC  =   2;
const Int_t NumOfSegFBH  =  16;
//const Int_t NumOfSegSAC  = 16; // 12 readout & 4 rooms //add
const Int_t NumOfSegSCH  =  64;
const Int_t NumOfSegBGO  =  24;
const Int_t NumOfSegBGO_T=   4;
const Int_t NumOfSegPiID =  32;
const Int_t NumOfSegTOF  =  24;
//const Int_t NumOfSegTOF_HT  = 24;
const Int_t NumOfSegTOF_HT= 16;
const Int_t NumOfSegClusteredFBH = 31;
const Int_t NumOfRoomsSAC=   4;
const Int_t NumOfSegLC   =  28;

// Misc
const Int_t DetIdTFlag     = 21;
const Int_t DetIdScaler    = 22;
const Int_t NumOfSegTFlag  = 32;
const Int_t NumOfSegScaler = 32;
const Int_t SpillEndFlag   = 27; // 0-based
const Int_t NumOfVmeRm     = 10;

// Trackers
const Int_t DetIdBC3  = 103;
const Int_t DetIdBC4  = 104;
const Int_t DetIdSDC1 = 105;
const Int_t DetIdSDC2 = 106;
const Int_t DetIdSDC3 = 107;
const Int_t DetIdBFT  = 110;
const Int_t DetIdSFT  = 111;
const Int_t DetIdCFT  = 113;
const Int_t DetIdFHT1 = 131;
const Int_t DetIdFHT2 = 132;
const Int_t DetIdBcOut  = 20;
const Int_t DetIdSdcIn  = 30;
const Int_t DetIdSdcOut = 40;

const Int_t NumOfLayersBcOut  = 12;
const Int_t NumOfLayersSdcIn  =  9;
const Int_t NumOfLayersSdcOut =  8;
const Int_t NumOfLayersSsdIn  =  4;
const Int_t NumOfLayersSsdOut =  4;
const Int_t NumOfLayersBC3    =  6;
const Int_t NumOfLayersBC4    =  6;
const Int_t NumOfPlaneSFT     =  4; // U, V, X, XP
const Int_t NumOfLayersSFT    =  3; // U, V, X
const Int_t NumOfLayersCFT    =  8;
const Int_t NumOfLayersFHT    =  2;
const Int_t NumOfLayersSDC1   =  6;
const Int_t NumOfDimSDC2      =  2;
const Int_t NumOfLayersSDC2   =  4;
const Int_t NumOfDimSDC3      =  2;
const Int_t NumOfLayersSDC3   =  4;
const Int_t NumOfWireBC3      = 64;
const Int_t NumOfWireBC4      = 64;
const Int_t NumOfWireSDC1     = 64;
const Int_t NumOfWireSDC2     = 128;
const Int_t NumOfWireSDC3X    = 96;
const Int_t NumOfWireSDC3Y    = 64;
const Double_t MaxDriftLengthBC3  =  1.5;
const Double_t MaxDriftLengthBC4  =  1.5;
const Double_t MaxDriftLengthSDC1 =  3.0;
const Double_t MaxDriftLengthSDC2 =  4.5;
const Double_t MaxDriftLengthSDC3 = 10.0;
const Double_t MaxDriftTimeBC3    =  80.0;
const Double_t MaxDriftTimeBC4    =  80.0;
const Double_t MaxDriftTimeSDC1   = 120.0;
const Double_t MaxDriftTimeSDC2   = 120.0;
const Double_t MaxDriftTimeSDC3   = 260.0;

const Int_t PlMinBcOut  = 13;
const Int_t PlMaxBcOut  = 24;
const Int_t PlMinSdcIn  =  1;
const Int_t PlMaxSdcIn  =  6;
const Int_t PlMinSdcOut = 31;
const Int_t PlMaxSdcOut = 38;

const Int_t PlOffsBc     = 100;
const Int_t PlOffsSdcIn  =   0;
const Int_t PlOffsSdcOut =  30;
const Int_t PlOffsSsd    =   6;

const Int_t NumOfPlaneBFT  = 2;
const Int_t NumOfSegBFT    = 160;
const Int_t NumOfSegSFT_X  = 256;
const Int_t NumOfSegSFT_UV = 320;
const Int_t NumOfSegSFT[NumOfPlaneSFT] = { NumOfSegSFT_UV,
					   NumOfSegSFT_UV,
					   NumOfSegSFT_X,
					   NumOfSegSFT_X };
const Int_t NumOfSegCFT_UV1   = 426;
const Int_t NumOfSegCFT_PHI1  = 584;
const Int_t NumOfSegCFT_UV2   = 472 + 10; //471
const Int_t NumOfSegCFT_PHI2  = 692;
const Int_t NumOfSegCFT_UV3   = 510;
const Int_t NumOfSegCFT_PHI3  = 800;
const Int_t NumOfSegCFT_UV4   = 538;
const Int_t NumOfSegCFT_PHI4  = 928;//910
const Int_t NumOfSegCFT[NumOfLayersCFT] = { NumOfSegCFT_UV1,
					    NumOfSegCFT_PHI1,
					    NumOfSegCFT_UV2,
					    NumOfSegCFT_PHI2,
					    NumOfSegCFT_UV3,
					    NumOfSegCFT_PHI3,
					    NumOfSegCFT_UV4,
					    NumOfSegCFT_PHI4 };
const Int_t NumOfSegFHT1          = 48;
const Int_t NumOfSegFHT2          = 64;
// VMEEASIROC Board
const Int_t DetIdVEASIROC         = 116;
const Int_t NumOfLayersVMEEASIROC = 95;
const Int_t NumOfSegVMEEASIROC    = 64;

// Tracker const
const Int_t FHTOffset             = 200;
const Int_t NumOfUDStructureFHT   = 2;
// Old Detectors
// E07 Detectors
const Int_t DetIdFBH  =  4;
const Int_t DetIdPVAC =  5;
const Int_t DetIdFAC  =  6;
const Int_t DetIdEMC      = 23;
const Int_t DetIdSSDT     = 24;
const Int_t NumOfSegPVAC =  1;
const Int_t NumOfSegFAC  =  1;
const Int_t NumOfSegEMC      =  1;
const Int_t NumOfSegSSDT     = 16;
const Int_t DetIdSSD1 = 121;
const Int_t DetIdSSD2 = 122;
const Int_t DetIdSsd    = 50;
const Int_t NumOfLayersSsd  = 8;
const Int_t NumOfLayersSSD1 = 4;
const Int_t NumOfLayersSSD2 = 4;
const Int_t NumOfSamplesSSD = 8;
const Int_t NumOfSegSSD1 = 1536;
const Int_t NumOfSegSSD2 = 1536;
const Double_t SamplingIntervalSSD = 25.; //[ns]
const Int_t DetIdLAC  =  6;
const Int_t DetIdGC   =  8;
const Int_t DetIdYOON =  9;
const Int_t DetIdRC   = 21;
const Int_t DetIdAC1  = 22;
const Int_t DetIdBVH  = 23;
const Int_t DetIdNRC  = 24;
const Int_t DetIdBH2_E07 =  33;
const Int_t DetIdBAC_E07 =  34;
const Int_t DetIdSAC1    =  37;
const Int_t DetIdKFAC    =  44;
const Int_t NumOfSegLAC = 30;
const Int_t NumOfSegBH2_E07 =  2;
const Int_t NumOfSegBAC_E07 =  4 + 2;
const Int_t NumOfSegSAC1    =  1;
const Int_t NumOfSegKFAC    =  2;
const Int_t NumOfLayersSSD0 =  2;
// E05 Detectors
const Int_t DetIdMsT     = 32;
const Int_t DetIdMsTRM   = 82;
const Int_t DetIdKIC     = 41;
const Int_t NumOfSegMsT  = 24;
const Int_t NumOfSegKIC  =  4;
// E13 Detectors
const Int_t DetIdBMW     = 10;
const Int_t DetIdMatrix  = 12;
const Int_t DetIdSFV     = 24;
const Int_t DetIdTOFMT   = 25;
const Int_t DetIdSP0     = 26;
const Int_t DetIdGe      = 27;
const Int_t DetIdPWO     = 28;
const Int_t DetIdPWOADC  = 29;
const Int_t NumOfSegBAC_SAC = 6;
const Int_t NumOfSegSFV = 18;
const Int_t NumOfSegGe   = 32;

const Int_t NumOfSegPWO  = 238;
const Int_t NumOfBoxPWO  = 22;
const Int_t NumOfUnitPWO[NumOfBoxPWO] =
  {
    6, 9, 6, 14, 14, 21, 14, 14, 6, 9, 6,
    6, 9, 6, 14, 14, 21, 14, 14, 6, 9, 6
  };

const Int_t SegIdPWO[NumOfBoxPWO] =
  {
    4, 0, 0, 4, 0, 0, 1, 5, 2, 1, 6,
    1, 2, 5, 6, 2, 1, 3, 7, 7, 3, 3,
  };

const Int_t NumOfLayersSP0 = 8;
const Int_t NumOfSegSP0    = 5;

const Int_t DetIdBC1   = 101;
const Int_t DetIdBC2   = 102;
const Int_t DetIdSDC4  = 108;
const Int_t DetIdK6BDC = 109;
const Int_t DetIdHDC   = 113;
const Int_t DetIdSSD0  = 120;

const Int_t DetIdBcIn   = 10;

const Int_t NumOfLayersBC1  =  6;
const Int_t NumOfLayersBC2  =  6;
const Int_t NumOfLayersBcIn = 12;
const Int_t NumOfLayersHDC  =  4;
const Int_t NumOfLayersSDC4 =  6;

const Int_t NumOfWireBC1    =  256;
const Int_t NumOfWireBC2    =  256;
const Int_t NumOfWireHDC    =  112;
const Int_t NumOfWireSDC3x  =  108;
const Int_t NumOfWireSDC4x  =  108;
const Int_t NumOfWireSDC3uv =  120;
const Int_t NumOfWireSDC4uv =  120;
const Int_t NumOfSegSSD0    = 1536;
const Int_t PlMinBcIn  =  1;
const Int_t PlMaxBcIn  = 12;

// Trigger Flag
namespace trigger
{
  enum ETriggerFlag
    {
      kBH2K_1,
      kBH2K_2,
      kBH2K_3,
      kBH2K_4,
      kBH2K_5,
      kBH2K_6,
      kBH2K_7,
      kBH2K_8,
      kBH2K,
      kElseOR,
      kBeam,
      kBeamTOF,
      kBeamPi,
      kBeamP,
      kCoin1,
      kCoin2,
      kE03,
      kBH2KPS,
      kBeamPS,
      kBeamTOFPS,
      kBeamPiPS,
      kBeamPPS,
      kCoin1PS,
      kCoin2PS,
      kE03PS,
      kClock,
      kReserve2,
      kSpillEnd,
      kMatrix,
      kMsTaccept,
      kMsTclear,
      kTOFtiming,
      NTriggerFlag
    };

  const std::vector<TString> STriggerFlag =
    {
      "BH2K_1",
      "BH2K_2",
      "BH2K_3",
      "BH2K_4",
      "BH2K_5",
      "BH2K_6",
      "BH2K_7",
      "BH2K_8",
      "BH2K",
      "ElseOR",
      "Beam",
      "BeamTOF",
      "BeamPi",
      "BeamP",
      "Coin1",
      "Coin2",
      "E03",
      "BH2KPS",
      "BeamPS",
      "BeamTOFPS",
      "BeamPiPS",
      "BeamPPS",
      "Coin1PS",
      "Coin2PS",
      "E03PS",
      "Clock",
      "Reserve2",
      "SpillEnd",
      "Matrix",
      "MsTaccept",
      "MsTclear",
      "TOFtiming",
    };
}

namespace DetTOF_HT
{
  enum ETOF_HT
    {
      TOF1_20,
      TOF5_22,
      TOF18,
      TOF4_10,
      TOF2_11,
      TOF8_24,
      TOF6_9,
      TOF7_23,
      TOF12,
      TOF19,
      TOF13,
      TOF14,
      TOF15,
      TOF16,
      TOF3_21,
      TOF17,
      NTOF_HT
    };

  const std::vector<TString> STOF_HT     =
    {
      "TOF(1,20)",
      "TOF(5,22)",
      "TOF18",
      "TOF(4,10)",
      "TOF(2,11)",
      "TOF(8,24)",
      "TOF(6,9)",
      "TOF(7,23)",
      "TOF12",
      "TOF19",
      "TOF13",
      "TOF14",
      "TOF15",
      "TOF16",
      "TOF(3,21)",
      "TOF17",
    };
 };

#endif
