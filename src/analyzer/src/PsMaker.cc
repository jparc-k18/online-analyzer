#include"PsMaker.hh"
#include"GuiPs.hh"
#include"HistMaker.hh"
#include"DetectorID.hh"
#include"HistHelper.hh"
#include"UnpackerManager.hh"
#include"Unpacker.hh"

#include<iostream>
#include<algorithm>

#include<TROOT.h>
#include<TList.h>
#include<TIterator.h>
#include<TClass.h>
#include<TCanvas.h>
#include<TPad.h>
#include<TPostScript.h>
#include<TH1.h>
#include<TH2.h>
#include<TText.h>
#include<TString.h>

#define CONV_STRING(x) getStr_FromEnum(#x)

static const std::string MyName = "PsMaker::";

using namespace hddaq::gui;

// Constructor --------------------------------------------------------------
PsMaker::PsMaker():
  cps_(NULL), ps_(NULL)
{
  // Initialize option list
  name_option_.resize(sizeOptionList);
  
  name_option_[kExpDataSheet]  = "ExpDataSheet";
  name_option_[kFixXaxis]      = "FixXaxis";
  name_option_[kLogyADC]       = "LogyADC";
  name_option_[kLogyTDC]       = "LogyTDC";
  //  name_option_[kLogyHitPat]    = "LogyHitPat";
  //  name_option_[kLogyMulti]     = "LogyMulti";
}

// Destructor --------------------------------------------------------------
PsMaker::~PsMaker()
{
  
}

// -------------------------------------------------------------------------
// getListOfOption
// -------------------------------------------------------------------------
void PsMaker::getListOfOption(std::vector<std::string>& vec)
{
  PsMaker& g = PsMaker::getInstance();
  std::copy(g.name_option_.begin(), 
	    g.name_option_.end(),
	    back_inserter(vec)
	    );
}

// -------------------------------------------------------------------------
// makePs
// -------------------------------------------------------------------------
void PsMaker::makePs()
{
  gROOT->SetStyle("Plain");
  
  // make ps file instance
  const std::string& filename = GuiPs::getFilename();
  ps_  = new TPostScript(filename.c_str(), 112);
  cps_ = new TCanvas("cps","");

  // Make title page with run number
  drawRunNumber();

  // start draw histograms of each detector
  std::vector<std::string> name_detectors;
  HistMaker::getListOfPsFiles(name_detectors);
  const int NofDetector = name_detectors.size();
  for(int i = 0; i<NofDetector; ++i){
    if(GuiPs::isDevOn(i) || GuiPs::isOptOn(kExpDataSheet)){
      create(name_detectors[i]);
    }
  }

  // close
  if(ps_){
    ps_->Close();
    ps_ = NULL;
  }

  if(cps_){
    delete cps_;
    cps_ = NULL;
  }

  gROOT->SetStyle("Classic");
}

// -------------------------------------------------------------------------
// create
// -------------------------------------------------------------------------
void PsMaker::create(std::string& name)
{
  std::vector<int> id_list;
  std::vector<int> par_list(sizeParameterList);
  bool flag_xaxis = false; // case by case in each detector
  bool flag_log   = false; // case by case in each detector

  // BH1 ----------------------------------------------------------------
  if(name == CONV_STRING(kBH1)){
    int base_id = 0;
    // ADC
    par_list[kXdiv] = 3; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    // ADC U
    base_id = HistMaker::getUniqueID(kBH1, 0, kADC);
    for(int i = 0; i<NumOfSegBH1; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // ADC D
    base_id = HistMaker::getUniqueID(kBH1, 0, kADC, NumOfSegBH1+1);
    for(int i = 0; i<NumOfSegBH1; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC
    par_list[kXdiv] = 3; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC U
    base_id = HistMaker::getUniqueID(kBH1, 0, kTDC);
    for(int i = 0; i<NumOfSegBH1; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC D
    base_id = HistMaker::getUniqueID(kBH1, 0, kTDC, NumOfSegBH1+1);
    for(int i = 0; i<NumOfSegBH1; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // HitPat and multiplicity
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;

    id_list.push_back(HistMaker::getUniqueID(kBH1, 0, kHitPat));
    id_list.push_back(HistMaker::getUniqueID(kBH1, 0, kMulti));
    drawOneCanvas(id_list, par_list, false, false);
  }

  // BFT ----------------------------------------------------------------
  if(name == CONV_STRING(kBFT)){
    // BFT all
    par_list[kXdiv] = 3; par_list[kYdiv] = 2;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 1000; // for TDC
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    id_list.push_back(HistMaker::getUniqueID(kBFT, 0, kTDC, 1));
    id_list.push_back(HistMaker::getUniqueID(kBFT, 0, kTDC, 2));
    id_list.push_back(HistMaker::getUniqueID(kBFT, 0, kHitPat, 1));
    id_list.push_back(HistMaker::getUniqueID(kBFT, 0, kHitPat, 2));
    id_list.push_back(HistMaker::getUniqueID(kBFT, 0, kMulti, 1));
    drawOneCanvas(id_list, par_list, flag_xaxis, false);
  }

  // BC3 ----------------------------------------------------------------
  if(name == CONV_STRING(kBC3)){
    // For all
    int base_id = 0;
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    // BC3 TDC
    par_list[kXrange_min] = 256; par_list[kXrange_max] = 1000;
    base_id = HistMaker::getUniqueID(kBC3, 0, kTDC);
    for(int i = 0; i<NumOfLayersBC3; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, false);

    // BC3 HitPat
    base_id = HistMaker::getUniqueID(kBC3, 0, kHitPat);
    for(int i = 0; i<NumOfLayersBC3; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);

    // BC3 Multi
    base_id = HistMaker::getUniqueID(kBC3, 0, kMulti);
    for(int i = 0; i<NumOfLayersBC3; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);
  }

  // BC4 ----------------------------------------------------------------
  if(name == CONV_STRING(kBC4)){
    // For all
    int base_id = 0;
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    // BC4 TDC
    par_list[kXrange_min] = 256; par_list[kXrange_max] = 1000;
    base_id = HistMaker::getUniqueID(kBC4, 0, kTDC);
    for(int i = 0; i<NumOfLayersBC4; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, false);

    // BC4 HitPat
    base_id = HistMaker::getUniqueID(kBC4, 0, kHitPat);
    for(int i = 0; i<NumOfLayersBC4; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);

    // BC4 Multi
    base_id = HistMaker::getUniqueID(kBC4, 0, kMulti);
    for(int i = 0; i<NumOfLayersBC4; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);
  }

  // BMW ----------------------------------------------------------------
  if(name == CONV_STRING(kBMW)){
    // For all
    par_list[kXdiv] = 2; par_list[kYdiv] = 1;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kLogyTDC) | GuiPs::isOptOn(kExpDataSheet);

    // ADC && TDC
    id_list.push_back(HistMaker::getUniqueID(kBMW, 0, kADC));
    id_list.push_back(HistMaker::getUniqueID(kBMW, 0, kTDC));
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);
  }

  // BAC_SAC ----------------------------------------------------------------
  if(name == CONV_STRING(kBAC_SAC)){
    int base_id = 0;
    // ADC
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    // ADC D
    base_id = HistMaker::getUniqueID(kBAC_SAC, 0, kADC);
    for(int i = 0; i<NumOfSegBAC; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC D
    base_id = HistMaker::getUniqueID(kBAC_SAC, 0, kTDC);
    for(int i = 0; i<NumOfSegBAC; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);
  }

  // BH2 ----------------------------------------------------------------
  if(name == CONV_STRING(kBH2)){
    int base_id = 0;
    // ADC
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    // ADC D
    base_id = HistMaker::getUniqueID(kBH2, 0, kADC);
    for(int i = 0; i<NumOfSegBH2; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC D
    base_id = HistMaker::getUniqueID(kBH2, 0, kTDC);
    for(int i = 0; i<NumOfSegBH2; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // HitPat and multiplicity
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;

    id_list.push_back(HistMaker::getUniqueID(kBH2, 0, kHitPat));
    id_list.push_back(HistMaker::getUniqueID(kBH2, 0, kMulti));
    drawOneCanvas(id_list, par_list, false, false);
  }
  
  // SDC2 ----------------------------------------------------------------
  if(name == CONV_STRING(kSDC2)){
    // For all
    int base_id = 0;
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    // SDC2 TDC
    par_list[kXrange_min] = 256; par_list[kXrange_max] = 1000;
    base_id = HistMaker::getUniqueID(kSDC2, 0, kTDC);
    for(int i = 0; i<NumOfLayersSDC2; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, false);

    // SDC2 HitPat
    base_id = HistMaker::getUniqueID(kSDC2, 0, kHitPat);
    for(int i = 0; i<NumOfLayersSDC2; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);

    // SDC2 Multi
    base_id = HistMaker::getUniqueID(kSDC2, 0, kMulti);
    for(int i = 0; i<NumOfLayersSDC2; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);
  }

  // HDC ----------------------------------------------------------------
  if(name == CONV_STRING(kHDC)){
    // For all
    int base_id = 0;
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    // HDC TDC
    par_list[kXrange_min] = 256; par_list[kXrange_max] = 1000;
    base_id = HistMaker::getUniqueID(kHDC, 0, kTDC);
    for(int i = 0; i<NumOfLayersHDC; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, false);

    // HDC HitPat
    base_id = HistMaker::getUniqueID(kHDC, 0, kHitPat);
    for(int i = 0; i<NumOfLayersHDC; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);

    // HDC Multi
    base_id = HistMaker::getUniqueID(kHDC, 0, kMulti);
    for(int i = 0; i<NumOfLayersHDC; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);
  }

  // SP0 -----------------------------------------------------------------
  if(name == CONV_STRING(kSP0)){
    int base_id = 0;
    // ADC
    par_list[kXdiv] = 5; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    for(int draw = 0; draw<4; ++draw){
      for(int l = 0; l<2; ++l){
	// ADC U
	base_id = HistMaker::getUniqueID(kSP0, kSP0_L1+l+2*draw, kADC);
	for(int i = 0; i<NumOfSegSP0; ++i){id_list.push_back(base_id + i);}

	// ADC D
	base_id = HistMaker::getUniqueID(kSP0, kSP0_L1+l+2*draw, kADC, NumOfSegSP0+1);
	for(int i = 0; i<NumOfSegSP0; ++i){id_list.push_back(base_id + i);}
      }
      drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);
    }

    // TDC
    par_list[kXdiv] = 5; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    for(int draw = 0; draw<4; ++draw){
      for(int l = 0; l<2; ++l){
	// TDC U
	base_id = HistMaker::getUniqueID(kSP0, kSP0_L1+l+2*draw, kTDC);
	for(int i = 0; i<NumOfSegSP0; ++i){id_list.push_back(base_id + i);}

	// TDC D
	base_id = HistMaker::getUniqueID(kSP0, kSP0_L1+l+2*draw, kTDC, NumOfSegSP0+1);
	for(int i = 0; i<NumOfSegSP0; ++i){id_list.push_back(base_id + i);}
      }
      drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);
    }
  }

  // SDC3 ----------------------------------------------------------------
  if(name == CONV_STRING(kSDC3)){
    // For all
    int base_id = 0;
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    // SDC3 TDC
    par_list[kXrange_min] = 700; par_list[kXrange_max] = 1600;
    base_id = HistMaker::getUniqueID(kSDC3, 0, kTDC);
    for(int i = 0; i<NumOfLayersSDC3; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, false);

    // SDC3 HitPat
    base_id = HistMaker::getUniqueID(kSDC3, 0, kHitPat);
    for(int i = 0; i<NumOfLayersSDC3; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);

    // SDC3 Multi
    base_id = HistMaker::getUniqueID(kSDC3, 0, kMulti);
    for(int i = 0; i<NumOfLayersSDC3; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);
  }

  // SDC4 ----------------------------------------------------------------
  if(name == CONV_STRING(kSDC4)){
    // For all
    int base_id = 0;
    par_list[kXdiv] = 2; par_list[kYdiv] = 3;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);

    // SDC4 TDC
    par_list[kXrange_min] = 700; par_list[kXrange_max] = 1600;
    base_id = HistMaker::getUniqueID(kSDC4, 0, kTDC);
    for(int i = 0; i<NumOfLayersSDC4; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, false);

    // SDC4 HitPat
    base_id = HistMaker::getUniqueID(kSDC4, 0, kHitPat);
    for(int i = 0; i<NumOfLayersSDC4; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);

    // SDC4 Multi
    base_id = HistMaker::getUniqueID(kSDC4, 0, kMulti);
    for(int i = 0; i<NumOfLayersSDC4; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, false, false);
  }

  // TOF ----------------------------------------------------------------
  if(name == CONV_STRING(kTOF)){
    int base_id = 0;
    int index   = 0;
    // ADC
    par_list[kXdiv] = 4; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    // ADC U
    base_id = HistMaker::getUniqueID(kTOF, 0, kADC);
    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // ADC D
    index = 0;
    base_id = HistMaker::getUniqueID(kTOF, 0, kADC, NumOfSegTOF+1);
    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC
    par_list[kXdiv] = 4; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC U
    index = 0;
    base_id = HistMaker::getUniqueID(kTOF, 0, kTDC);
    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC D
    index = 0;
    base_id = HistMaker::getUniqueID(kTOF, 0, kTDC, NumOfSegTOF+1);
    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // HitPat and multiplicity
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;

    id_list.push_back(HistMaker::getUniqueID(kTOF, 0, kHitPat));
    id_list.push_back(HistMaker::getUniqueID(kTOF, 0, kMulti));
    drawOneCanvas(id_list, par_list, false, false);
  }

  // TOFMT --------------------------------------------------------------
  if(name == CONV_STRING(kTOFMT)){
    int base_id = 0;
    int index   = 0;

    // TDC
    par_list[kXdiv] = 4; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC MT
    index = 0;
    base_id = HistMaker::getUniqueID(kTOFMT, 0, kTDC);
    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegTOF/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // HitPat and multiplicity
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;

    id_list.push_back(HistMaker::getUniqueID(kTOFMT, 0, kHitPat));
    id_list.push_back(HistMaker::getUniqueID(kTOFMT, 0, kMulti));
    drawOneCanvas(id_list, par_list, false, false);
  }

  // SFV ----------------------------------------------------------------
  if(name == CONV_STRING(kSFV_SAC3)){
    int base_id = 0;
    int NofLoop = 7;
    
    // ADC
    par_list[kXdiv] = 3; par_list[kYdiv] = 3;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    // ADC D
    base_id = HistMaker::getUniqueID(kSFV_SAC3, 0, kADC);
    for(int i = 0; i<NofLoop; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC D
    base_id = HistMaker::getUniqueID(kSFV_SAC3, 0, kTDC);
    for(int i = 0; i<NofLoop; ++i){id_list.push_back(base_id + i);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // HitPat and multiplicity
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;

    id_list.push_back(HistMaker::getUniqueID(kSFV_SAC3, 0, kHitPat));
    drawOneCanvas(id_list, par_list, false, false);
  }

  // LC ----------------------------------------------------------------
  if(name == CONV_STRING(kLC)){
    int base_id = 0;
    int index   = 0;
    // ADC
    par_list[kXdiv] = 4; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    // ADC U
    base_id = HistMaker::getUniqueID(kLC, 0, kADC);
    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // ADC D
    index = 0;
    base_id = HistMaker::getUniqueID(kLC, 0, kADC, NumOfSegLC+1);
    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC
    par_list[kXdiv] = 4; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 4000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    // TDC U
    index = 0;
    base_id = HistMaker::getUniqueID(kLC, 0, kTDC);
    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // TDC D
    index = 0;
    base_id = HistMaker::getUniqueID(kLC, 0, kTDC, NumOfSegLC+1);
    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    for(int i = 0; i<NumOfSegLC/2; ++i){id_list.push_back(base_id + index++);}
    drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);

    // HitPat and multiplicity
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;

    id_list.push_back(HistMaker::getUniqueID(kLC, 0, kHitPat));
    id_list.push_back(HistMaker::getUniqueID(kLC, 0, kMulti));
    drawOneCanvas(id_list, par_list, false, false);
  }


  // Correlation ------------------------------------------------------------
  if(name == CONV_STRING(kCorrelation)){
    par_list[kXdiv] = 2; par_list[kYdiv] = 2;
    
    id_list.push_back(HistMaker::getUniqueID(kCorrelation, 0, 0, 1));
    id_list.push_back(HistMaker::getUniqueID(kCorrelation, 0, 0, 2));
    id_list.push_back(HistMaker::getUniqueID(kCorrelation, 0, 0, 3));
    id_list.push_back(HistMaker::getUniqueID(kCorrelation, 0, 0, 4));
    drawOneCanvas(id_list, par_list, false, false, "box");
  }

  // --------------------------------------------------------------------
  // Hyperball
  // --------------------------------------------------------------------

  // Ge ----------------------------------------------------------------
  if(name == CONV_STRING(kGe)){
    int base_id = 0;
    int index   = 0;
    
    // ADC
    par_list[kXdiv] = 2; par_list[kYdiv] = 4;
    par_list[kXrange_min] = 0; par_list[kXrange_max] = 0x2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyADC)  | GuiPs::isOptOn(kExpDataSheet);

    base_id = HistMaker::getUniqueID(kGe, 0, kADC);
    for(int i = 0; i<4; ++i){
      for(int j = 0; j<8; ++j){id_list.push_back(base_id + index++);}
      drawOneCanvas(id_list, par_list, flag_xaxis, flag_log);
    }

    // CRM
    //    par_list[kXrange_min] = 0; par_list[kXrange_max] = 0x2000;
    flag_xaxis = GuiPs::isOptOn(kFixXaxis) | GuiPs::isOptOn(kExpDataSheet);
    flag_log   = GuiPs::isOptOn(kLogyTDC)  | GuiPs::isOptOn(kExpDataSheet);

    index = 0;
    base_id = HistMaker::getUniqueID(kGe, 0, kCRM);
    for(int i = 0; i<4; ++i){
      for(int j = 0; j<8; ++j){id_list.push_back(base_id + index++);}
      drawOneCanvas(id_list, par_list, false, flag_log);
    }

    // TFA
    //    par_list[kXrange_min] = 0; par_list[kXrange_max] = 0x2000;

    index = 0;
    base_id = HistMaker::getUniqueID(kGe, 0, kTFA);
    for(int i = 0; i<4; ++i){
      for(int j = 0; j<8; ++j){id_list.push_back(base_id + index++);}
      drawOneCanvas(id_list, par_list, false, flag_log);
    }

    // PUR
    //    par_list[kXrange_min] = 0; par_list[kXrange_max] = 0x2000;

    index = 0;
    base_id = HistMaker::getUniqueID(kGe, 0, kPUR);
    for(int i = 0; i<4; ++i){
      for(int j = 0; j<8; ++j){id_list.push_back(base_id + index++);}
      drawOneCanvas(id_list, par_list, false, flag_log);
    }

    // RST
    //    par_list[kXrange_min] = 0; par_list[kXrange_max] = 0x2000;

    index = 0;
    base_id = HistMaker::getUniqueID(kGe, 0, kRST);
    for(int i = 0; i<4; ++i){
      for(int j = 0; j<8; ++j){id_list.push_back(base_id + index++);}
      drawOneCanvas(id_list, par_list, false, flag_log);
    }
  }

}



// -------------------------------------------------------------------------
// drawOneChanvas
// -------------------------------------------------------------------------
void PsMaker::drawOneCanvas(std::vector<int>& id_list,
			    std::vector<int>& par_list,
			    bool flag_xaxis,
			    bool flag_log,
			    const char* optDraw
			    )
{
  static const std::string MyFunc = "drawOneCanvas ";
  // error check
  int NofPad  = par_list[kXdiv]*par_list[kYdiv];
  int NofHist = id_list.size();
  if(NofHist > NofPad){
    std::cerr << "#E: " << MyName << MyFunc
	      << "Num of pad is not enough\n"
	      << " 1st unique ID: " << id_list[0] << std::endl;
    return;
  }

  // make new ps page
  ps_->NewPage();
  cps_->Divide(par_list[kXdiv], par_list[kYdiv]);
  for(int i = 0; i<(int)id_list.size(); ++i){

    cps_->cd(i + 1);
    if(flag_log){
      // log scale flag
      cps_->GetPad(i + 1)->SetLogy(1);
    }

    TH1* h = GHist::get(id_list[i]);
    if(!h){
      std::cerr << "#E: " << MyName << MyFunc
		<< "Pointer is NULL\n"
		<< " Unique ID: " << id_list[i] << std::endl;
      return;
    }

    if(flag_xaxis){
      // xaxis fix flag
      h->GetXaxis()->SetRangeUser(par_list[kXrange_min],
				  par_list[kXrange_max]);
    }

    h->SetLineColor(1);    
    h->Draw(optDraw);
  }

  cps_->Update();
  cps_->cd();
  clearOneCanvas(par_list[kXdiv]*par_list[kYdiv]);

  id_list.clear();
}

// -------------------------------------------------------------------------
// clearOneCanvas
// -------------------------------------------------------------------------
void PsMaker::clearOneCanvas(int npad)
{
  for(int i = 0; i<npad; ++i){
    cps_->GetPad(i + 1)->Close();
  }
  return;
}

// -------------------------------------------------------------------------
// drawRunNumber
// -------------------------------------------------------------------------
void PsMaker::drawRunNumber()
{
  hddaq::unpacker::UnpackerManager& g_unpacker =
    hddaq::unpacker::GUnpacker::get_instance();
  int runno = g_unpacker.get_root()->get_run_number();

  ps_->NewPage();
  cps_->Divide(1,1);
  cps_->cd(1);
  cps_->GetPad(1)->Range(0,0,100,100);
  
  TText text;
  text.SetTextSize(0.2);
  text.SetTextAlign(22);
  text.SetText(50.,50., Form("Run# %d", runno));
  text.Draw();
  
  cps_->Update();
  cps_->cd();
  cps_->GetPad(1)->Close();
}


