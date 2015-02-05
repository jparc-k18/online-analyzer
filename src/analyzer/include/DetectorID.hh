//Author:Hitoshi sugimura

#ifndef DETECTOR_ID_H
#define DETECTOR_ID_H

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
const int DetIdSSD  =112;

const int DetIdBH1  =  3;
const int DetIdBH2  =  4;
const int DetIdTOF  =  5;
const int DetIdAC   =  6;
const int DetIdLC   =  7;
const int DetIdGC   =  8;
//const int DetIdBAC  =  9;
const int DetIdYOON  =  9;
const int DetIdRC  = 21;
const int DetIdAC1  = 22;
const int DetIdBVH  = 23;
const int DetIdNRC  = 24;

// E13 ID
const int DetIdBAC     =  9;
const int DetIdBMW     =  10;
const int DetIdMisc    =  11;
const int DetIdMatrix  =  12;
const int DetIdSFV     =  24;
const int DetIdTOFMT   =  25;
const int DetIdSP0     =  26;
const int DetIdGe      =  27;
const int DetIdPWO     =  28;
const int DetIdPWOADC  =  29;

// E13 seg
const int NumOfSegBAC = 6;
const int NumOfSegSFV = 18;
const int NumOfSegMisc = 18;

const int NumOfSegGe   = 32;
//const int NumOfSegPWO  = 256; // tekitou
const int NumOfSegPWO_B= 21;
const int NumOfSegPWO_E= 14;
const int NumOfSegPWO_C= 9;
const int NumOfSegPWO_L= 6;
const int NumOfSegPWOADC = 32;

const int NumOfUnitPWO_B= 2;
const int NumOfUnitPWO_E= 8;
const int NumOfUnitPWO_C= 4;
const int NumOfUnitPWO_L= 8;

const int NumOfSegPWO[4]={NumOfSegPWO_B, NumOfSegPWO_E,
			  NumOfSegPWO_C, NumOfSegPWO_L};
const int NumOfUnitPWO[4]={NumOfUnitPWO_B, NumOfUnitPWO_E,
			   NumOfUnitPWO_C, NumOfUnitPWO_L};
const char pwo_type[4]={'B', 'E', 'C', 'L'};

const int NumOfLayersSP0 = 8;
const int NumOfSegSP0    = 5;


//const int DetIdBcIn   = 10;
const int DetIdBcOut  = 20;
const int DetIdSdcIn  = 30;
const int DetIdSdcOut = 40;
const int NumOfSegBH1 = 11;
const int NumOfSegBH2 =  5;
const int NumOfSegTOF = 32;
//const int NumOfSegAC  = 19;
const int NumOfSegAC1  = 18;
const int NumOfSegBVH  = 4;
//const int NumOfSegAC2  = 20;
const int NumOfSegAC  = 30;
const int NumOfSegLC  = 28;
//const int NumOfSegBAC = 7;
//const int NumOfSegBAC = 10;
const int NumOfSegRC = 42;//ichikawa_mod
const int NumOfSegNRC = 8;//ichikawa_mod
const int NumOfSegYOON = 4;
const int NumOfSegBFT = 160; // miwa_mod
const int NumOfSegSFTX = 256;
const int NumOfSegSFTVU = 320;

const int PlaneId = 0;
const int PlaneId_RC = 6;
const int UorD = 2;
const int AorT = 2;

const int NumOfLayersBcIn =12;
const int NumOfLayersBcOut=12;
const int NumOfLayersSdcIn =10;
const int NumOfLayersSdcOut=12;

const int NumOfLayersBC1  =6;
const int NumOfLayersBC2  =6;
const int NumOfLayersBC3  =6;
const int NumOfLayersBC4  =6;
const int NumOfLayersSDC1 =4;
const int NumOfLayersSDC2 =6;
const int NumOfLayersHDC  =4;
const int NumOfLayersSDC3 =6;
const int NumOfLayersSDC4 =6;
const int NumOfLayersSSD  =2;

const int NumOfLayersAc = 2;
//const int NumOfLayersAc1 = 2;
const int NumOfLayersRC = 7;
const int NumOfLayersMisc =2;
const int NumOfWireBC1 = 256;
const int NumOfWireBC2 = 256;
const int NumOfWireBC3 = 64;
const int NumOfWireBC4 = 64;
const int NumOfWireK6BDC = 48;
const int NumOfWireSDC1 = 64;
const int NumOfWireSDC2 = 96;
const int NumOfWireHDC  = 112;
const int NumOfWireSDC3x = 108;
const int NumOfWireSDC4x = 108;
const int NumOfWireSDC3uv = 120;
const int NumOfWireSDC4uv = 120;
const int NumOfWireSSD = 768;
const int PlMinBdcIn = 1;
const int PlMaxBdcIn = 12;
const int PlMinBdcOut =13;
const int PlMaxBdcOut =24;

const int PlMinSdcIn = 1;
const int PlMaxSdcIn =10;
const int PlMinSdcOut=11;
const int PlMaxSdcOut=22;

const int PlOffsBdc = 100;

const int PlMinAc =  1;
const int PlMaxAc =  2;
const int PlMinRC =  1;
const int PlMaxRC = 10;
const int PlMinMisc = 1;
const int PlMaxMisc = 4;

const int PlIdK6Target = 130;

#endif
