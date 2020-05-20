/**
 *  file: DetectorID.hh
 *  date: 2017.04.10
 *
 */

#ifndef DETECTOR_ID_HH
#define DETECTOR_ID_HH

#include <iostream>
#include <TString.h>

// Counters ___________________________________________________________
const int DetIdBH1      =  1;
const int DetIdBH2      =  2;
const int DetIdBAC      =  3; // K tuning
const int DetIdBH2_E42  =  4; // E42 test
const int DetIdSCH      =  7;
const int DetIdTOF      =  8;
const int DetIdSAC      =  9;
const int DetIdLC       = 10;
const int DetIdHtTOF    = 11; // high threshold TOF
const int DetIdLAC      = 12;
const int DetIdWC       = 13; // E42 test
const int DetIdT1       = 14; // E42 test
const int DetIdT2       = 15; // E42 test
const int NumOfSegBH1     = 11;
const int NumOfSegBH2     =  8;
const int NumOfSegBAC     =  2; // K tuning
const int NumOfSegBH2_E42 = 15; // E42 test
const int NumOfSegFBH     = 16;
const int NumOfSegSCH     = 64;
const int NumOfSegTOF     = 24;
const int NumOfSegSAC     =  4;
const int NumOfRoomsSAC   =  4;
const int NumOfSegLAC     = 30;
const int NumOfSegLC      = 28;
const int NumOfSegHtTOF   = 16;
const int NumOfSegClusteredFBH = 31;
const int NumOfSegWC      =  1; // E42 test
const int NumOfSegT1      =  1; // E42 test
const int NumOfSegT2      =  1; // E42 test

// Misc _______________________________________________________________
const int DetIdTrig       = 21;
const int DetIdScaler     = 22;
const int DetIdMsT        = 25;
const int DetIdMtx        = 26;
const int DetIdFpgaBH2Mt  = 29;
const int DetIdVmeRm      = 81;
const int DetIdMsTRM      = 82;
const int DetIdHulRM      = 83;
const int NumOfSegTrig    = 32;
const int NumOfSegScaler  = 96;
const int SpillEndFlag    = 27; // 0-based
const int NumOfPlaneVmeRm = 2;
const int NumOfVmeRm      = 10;
const int NumOfSegTFlag  = 32;

const int DetIdVmeCalib      = 999;
const int NumOfPlaneVmeCalib =   5;
const int NumOfSegVmeCalib   =  32;

// Trackers ___________________________________________________________
const int DetIdBC3  = 103;
const int DetIdBC4  = 104;
const int DetIdSDC1 = 105;
const int DetIdSDC2 = 106;
const int DetIdSDC3 = 107;
const int DetIdBFT  = 110;
const int DetIdSFT  = 111;
const int DetIdCFT  = 113;
const int DetIdBGO  = 114;
const int DetIdPiID = 115;
const int DetIdFHT1 = 131;
const int DetIdFHT2 = 132;
const int DetIdBcOut  = 20;
const Int_t DetIdSdcIn  = 30;
const Int_t DetIdSdcOut = 40;

const int PlMinBcIn        =   1;
const int PlMaxBcIn        =  12;
const int PlMinBcOut       =  13;
const int PlMaxBcOut       =  24;
const int PlMinSdcIn       =   1;
const int PlMaxSdcIn       =   9;
const int PlMinFHT1        =  80;
const int PlMaxFHT1        =  83;
const int PlMinFHT2        =  84;
const int PlMaxFHT2        =  87;
const int PlMinSdcOut      =  31;
const int PlMaxSdcOut      =  38;
const int PlOffsBc         = 100;
const int PlOffsSdcIn      =   0;
const int PlOffsSft        =   6;
const int PlOffsSdcOut     =  30;
const int PlOffsVP         =  20;
const int PlOffsFbt        =  80;

const int NumOfLayersBc     = 6;
const int NumOfLayersBC3    =  6;
const int NumOfDimBC3       =  3;
const int NumOfLayersBC4    =  6;
const int NumOfDimBC4       =  3;
const int NumOfLayersSFT    = 3;
const int NumOfLayersCFT    =  8;
const int NumOfLayersSDC1   = 6;
const int NumOfDimSDC1      =  3;
const int NumOfLayersSDC2   = 4;
const int NumOfDimSDC2      =  2;
const int NumOfLayersSDC3   = 4;
const int NumOfDimSDC3      =  2;
const int NumOfWireBC3      = 64;
const int NumOfWireBC4      = 64;
const int NumOfWireSDC1     = 64;
const int NumOfWireSDC2     = 128;
const int NumOfWireSDC3X    = 96;
const int NumOfWireSDC3Y    = 64;
const double MaxDriftLengthBC3  =  1.5;
const double MaxDriftLengthBC4  =  1.5;
const double MaxDriftLengthSDC1 =  3.0;
const double MaxDriftLengthSDC2 =  4.5;
const double MaxDriftLengthSDC3 = 10.0;
const double MaxDriftTimeBC3    =  80.0;
const double MaxDriftTimeBC4    =  80.0;
const double MaxDriftTimeSDC1   = 120.0;
const double MaxDriftTimeSDC2   = 120.0;
const double MaxDriftTimeSDC3   = 260.0;
const int NumOfLayersFHT1   = 2;
const int NumOfLayersFHT2   = 2;
const int NumOfLayersBcIn   = PlMaxBcIn   - PlMinBcIn   + 1;
const int NumOfLayersBcOut  = PlMaxBcOut  - PlMinBcOut  + 1;
const int NumOfLayersSdcIn  = PlMaxSdcIn  - PlMinSdcIn  + 1;
//const int NumOfLayersFHT    = PlMaxFHT2   - PlMinFHT1   + 1;
const int NumOfLayersFHT    = 2;
//const int NumOfLayersSdcOut = PlMaxSdcOut - PlMinSdcOut + 1; w/o FHT
const int NumOfLayersSdcOut = PlMaxSdcOut - PlMinSdcOut + 1 + (PlMaxFHT2 - PlMinFHT1 + 1); // including FHT
const int NumOfLayersVP     = 5;

const int MaxWireBC3      =  64;
const int MaxWireBC4      =  64;

const int MaxWireSDC1     =  64;
const int MaxWireSDC2     = 128;
const int MaxWireSDC3X    =  96;
const int MaxWireSDC3Y    =  64;

const int MaxSegFHT1      =  48;
const int MaxSegFHT2      =  64;
const int NumOfSegFHT1          = 48;
const int NumOfSegFHT2          = 64;

const int NumOfPlaneBFT   =   2;
const int NumOfSegBFT     = 160;
// SFT X layer has U D plane.
// SFT UV layers have only U plnane.
// enum SFT_PLANE{ SFT_X1, SFT_X2, SFT_V, SFT_U };
enum SFT_PLANE{ SFT_U, SFT_V, SFT_X1, SFT_X2 };
const int NumOfPlaneSFT   =   4;
const int NumOfSegSFT_X   = 256;
const int NumOfSegSFT_UV  = 320;
const int NumOfSegSFT[NumOfPlaneSFT] = { NumOfSegSFT_UV,
					   NumOfSegSFT_UV,
					   NumOfSegSFT_X,
					   NumOfSegSFT_X };
const int NumOfSegCSFT    = 48;

// CFT
const int NumOfPlaneCFT   =   8;
enum CFT_PLANE{CFT_U1, CFT_PHI1, CFT_V2, CFT_PHI2, CFT_U3, CFT_PHI3, CFT_V4, CFT_PHI4};
enum CFT_PLANE_{CFT_UV1, CFT_PHI1_, CFT_UV2, CFT_PHI2_, CFT_UV3, CFT_PHI3_, CFT_UV4, CFT_PHI4_};
const int NumOfSegCFT_UV1   = 426;
const int NumOfSegCFT_PHI1  = 584;
const int NumOfSegCFT_UV2   = 472;
const int NumOfSegCFT_PHI2  = 692;
const int NumOfSegCFT_UV3   = 510;
const int NumOfSegCFT_PHI3  = 800;
const int NumOfSegCFT_UV4   = 538;
const int NumOfSegCFT_PHI4  = 910;
const int NumOfSegCFT[NumOfPlaneCFT]  = {426,584,472,692,510,800,538,910};

// BGO
const double BGO_X = 30.;
const double BGO_Y = 25.;
const double BGO_Z = 400.;
const int    NumOfBGOUnit = 8;
const int    NumOfBGOInOneUnit = 2;//pair unit
const double RadiusOfBGOSurface = 100.;
const int    NumOfBGOInOneUnit2 = 1;//single unit
const double RadiusOfBGOSurface2 = 120.;
const int NumOfSegBGO = NumOfBGOUnit*(NumOfBGOInOneUnit+NumOfBGOInOneUnit2);//24
const int NumOfSegBGO_T=   4;

// PiID counter
const int NumOfSegPiID =  32;

const int NumOfPiIDUnit = 8;
const int NumOfPiIDInOneUnit = 3;
const double PiID_X = 30.;
const double PiID_Y = 10.;
const double PiID_Z = 400.;
const double RadiusOfPiIDSurface = 164.;

const int    NumOfPiIDInOneUnit2 = 1;//single unit
const double PiID2_X = 40.;
const double PiID2_Y = 10.;
const double PiID2_Z = 400.;
const double RadiusOfPiID2Surface = 180.;

// HulRm -----------------------------------------------
const int NumOfHulRm   = 4;

// Matrix ----------------------------------------------
const int NumOfSegSFT_Mtx = 48;

// MsT -------------------------------------------------
enum TypesMst{typeHrTdc, typeLrTdc, typeFlag, NumOfTypesMst};
const int NumOfMstHrTdc = 32;
const int NumOfMstLrTdc = 64;
const int NumOfMstFlag  = 7;
enum dTypesMst
  {
    mstClear,
    mstAccept,
    finalClear,
    cosolationAccept,
    fastClear,
    level2,
    noDecision,
    size_dTypsMsT
  };

// Scaler ----------------------------------------------
const int NumOfScaler  = 2;

// VMEEASIROC Board
const int DetIdVEASIROC         = 116;
const int NumOfLayersVMEEASIROC = 95;
const int NumOfSegVMEEASIROC    = 64;

// Tracker const
const int FHTOffset             = 200;
const int NumOfUDStructureFHT   = 2;

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

namespace DetHtTOF
{
  enum EHtTOF
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

  const std::vector<TString> SHtTOF     =
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
const double SamplingIntervalSSD = 25.; //[ns]
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
const int NumOfSegBH2_E07 =  2;
const int NumOfSegBAC_E07 =  4 + 2;
const int NumOfSegSAC1    =  1;
const int NumOfSegKFAC    =  2;
const int NumOfLayersSSD0 =  2;
// E05 Detectors
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

#endif
