// -*- C++ -*-

#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <utility>

#include "DetectorID.hh"
#include "FuncName.hh"
#include "HistHelper.hh"
#include "HistMaker.hh"

#include <TH1.h>
#include <TH2.h>
#include <TList.h>
#include <TDirectory.h>
#include <TString.h>

ClassImp(HistMaker)

// getStr_FromEnum ----------------------------------------------------------
// The method to get TString from enum value
#define CONV_STRING(x) getStr_FromEnum(#x)
TString getStr_FromEnum(const char* c){
  TString str = c;
  return str.Remove(0,1);
}

// #define DETECTOR_NAME(x) TString(x).ReplaceAll("create", "");

// Constructor -------------------------------------------------------------
HistMaker::HistMaker( void )
  : TObject(),
    current_hist_id_(0)
{
}

// Destructor -------------------------------------------------------------
HistMaker::~HistMaker( void )
{
}

// -------------------------------------------------------------------------
// getListOfDetectors
// -------------------------------------------------------------------------
void HistMaker::getListOfDetectors( std::vector<TString>& vec )
{
  HistMaker& g = HistMaker::getInstance();
  std::copy( g.name_created_detectors_.begin(),
	     g.name_created_detectors_.end(),
	     back_inserter(vec) );
}

// -------------------------------------------------------------------------
// getListOfPsFiles
// -------------------------------------------------------------------------
void HistMaker::getListOfPsFiles( std::vector<TString>& vec )
{
  HistMaker& g = HistMaker::getInstance();
  std::copy( g.name_ps_files_.begin(),
	     g.name_ps_files_.end(),
	     back_inserter(vec) );
}

// -------------------------------------------------------------------------
// setHistPtr
// -------------------------------------------------------------------------
Int_t HistMaker::setHistPtr( std::vector<TH1*>& vec )
{
  vec.resize(current_hist_id_);
  for( Int_t i = 0; i<current_hist_id_; ++i ){
    Int_t unique_id = getUniqueID(i);
    vec[i] = GHist::get(unique_id);
    if( !vec[i] ){
      std::cerr << "#E " << FUNC_NAME
		<< " Pointer is NULL" << std::endl
		<< " Unique ID    : " << unique_id << std::endl
		<< " Sequential ID: " << i << std::endl;
      gDirectory->ls();
      return -1;
    }
  }
  return 0;
}

// -------------------------------------------------------------------------
// CreateTH1
// -------------------------------------------------------------------------
TH1* HistMaker::createTH1( Int_t unique_id, const TString& title,
			   Int_t nbinx, Int_t xmin, Int_t xmax,
			   const TString& xtitle, const TString& ytitle )
{
  // Add information to dictionaries which will be used to find the histogram
  Int_t sequential_id = current_hist_id_++;
  TypeRetInsert ret =
    idmap_seq_from_unique_.insert( std::make_pair( unique_id, sequential_id ) );
  idmap_seq_from_name_.insert( std::make_pair( title, sequential_id ) );
  idmap_unique_from_seq_.insert( std::make_pair( sequential_id, unique_id ) );
  if(!ret.second){
    std::cerr << "#E " << FUNC_NAME
	      << " The unique id overlaps with other id"
	      << std::endl;
    std::cerr << " " << unique_id << " " << title << std::endl;
    std::exit(-1);
  }

  // create histogram using the static method of HistHelper class
  TH1 *h = GHist::I1(unique_id, title, nbinx, xmin, xmax);
  if(!h){
    std::cerr << "#E " << FUNC_NAME
	      << " Fail to create TH1"
	      << std::endl;
    std::cerr << " " << unique_id << " " << title << std::endl;
    std::exit(-1);
    //    return h;
  }

  h->GetXaxis()->SetTitle(xtitle);
  h->GetYaxis()->SetTitle(ytitle);
  return h;
}

// -------------------------------------------------------------------------
// CreateTH2
// -------------------------------------------------------------------------
TH2* HistMaker::createTH2( Int_t unique_id, const TString& title,
			   Int_t nbinx, Int_t xmin, Int_t xmax,
			   Int_t nbiny, Int_t ymin, Int_t ymax,
			   const TString& xtitle, const TString& ytitle )
{
  // Add information to dictionaries which will be used to find the histogram
  Int_t sequential_id = current_hist_id_++;
  TypeRetInsert ret =
    idmap_seq_from_unique_.insert( std::make_pair(unique_id, sequential_id ) );
  idmap_seq_from_name_.insert( std::make_pair(title, sequential_id ) );
  idmap_unique_from_seq_.insert( std::make_pair(sequential_id, unique_id ) );
  if( !ret.second ){
    std::cerr << "#E " << FUNC_NAME
	      << " The unique id overlaps with other id"
	      << std::endl;
    std::cerr << " " << unique_id << " " << title << std::endl;
    std::exit(-1);
  }

  // create histogram using the static method of HistHelper class
  TH2 *h = GHist::I2( unique_id, title,
		      nbinx, xmin, xmax,
		      nbiny, ymin, ymax );
  if(!h){
    std::cerr << "#E " << FUNC_NAME
	      << " Fail to create TH2"
	      << std::endl;
    std::cerr << " " << unique_id << " " << title << std::endl;
    std::exit(-1);
    //    return h;
  }

  h->GetXaxis()->SetTitle(xtitle);
  h->GetYaxis()->SetTitle(ytitle);
  return h;
}

//______________________________________________________________________________
TString
HistMaker::MakeDetectorName( const TString& name )
{
  TString det = name;
  return det.ReplaceAll("create", "");
}

//_____________________________________________________________________
TList*
HistMaker::createTimeStamp( Bool_t flag_ps )
{
  TString det_name = MakeDetectorName(__func__);
  name_created_detectors_.push_back( det_name );

  if( flag_ps )
    name_ps_files_.push_back( det_name );

  TList *top_dir = new TList;
  top_dir->SetName( det_name );

  Int_t target_id = getUniqueID( kTimeStamp, 0, kTDC );
  for( Int_t i=0; i<NumOfVmeRm; ++i ){
    top_dir->Add( createTH1( target_id++,
			     Form("%s_%d", det_name.Data(), i+1),
			     0x1000, -0x1000, 0x1000,
			     "TimeStamp", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createBH1
// -------------------------------------------------------------------------
TList* HistMaker::createBH1( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBH1);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kBH1, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegBH1*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH1){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH1; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC w/TDC ---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kBH1, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegBH1*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegBH1 ){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH1; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH1, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBH1*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH1){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH1; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }
  
  // ADC w/TDC (FPGA) ---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = "ADCwFPGA_TDC";
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kBH1, 0, kADCwTDC, NumOfSegBH1*2);
    for( Int_t i=0; i<NumOfSegBH1*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegBH1 ){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH1; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC (FPGA)----------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = "FPGA_TDC";
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH1, 0, kTDC, NumOfSegBH1*2);
    for(Int_t i = 0; i<NumOfSegBH1*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH1){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH1; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     10000, 0, 400000,
     			     50000, 0, 2000000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kBH1, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BH1_hit_pattern", // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   "Segment", ""));

    top_dir->Add(createTH1(++target_id, "BH1_chit_pattern", // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kBH1, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BH1_multiplicity", // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   "Multiplicity", ""));

    top_dir->Add(createTH1(++target_id, "BH1_cmultiplicity", // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBFT
// -------------------------------------------------------------------------
TList* HistMaker::createBFT( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBFT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kTDC, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BFT_TDC_U", // 1 origin
			   1024, 0, 1024,
			   "TDC [ch]", ""));

    top_dir->Add(createTH1(++target_id, "BFT_TDC_D", // 1 origin
			   1024, 0, 1024,
			   "TDC [ch]", ""));
  }

  // TDC (after cut) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kTDC, 10);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BFT_CTDC_U", // 1 origin
			   1024, 0, 1024,
			   "TDC [ch]", ""));

    top_dir->Add(createTH1(++target_id, "BFT_CTDC_D", // 1 origin
			   1024, 0, 1024,
			   "TDC [ch]", ""));
  }

  // TOT---------------------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kADC, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BFT_TOT_U", // 1 origin
			   200, -50, 150,
			   "TOT [ch]", ""));

    top_dir->Add(createTH1(++target_id, "BFT_TOT_D", // 1 origin
			   200, -50, 150,
			   "TOT [ch]", ""));
  }

  // TOT (after cut) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kADC, 10);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BFT_CTOT_U", // 1 origin
			   200, -50, 150,
			   "TOT [ch]", ""));

    top_dir->Add(createTH1(++target_id, "BFT_CTOT_D", // 1 origin
			   200, -50, 150,
			   "TOT [ch]", ""));
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BFT_HitPat_U", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   "Segment", ""));

    top_dir->Add(createTH1(++target_id, "BFT_HitPat_D", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   "Segment", ""));
  }

  // Hit parttern (after cut) -----------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kHitPat, 10);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BFT_CHitPat_U", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   "Segment", ""));

    top_dir->Add(createTH1(++target_id, "BFT_CHitPat_D", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "BFT_multiplicity";
    Int_t target_id = getUniqueID(kBFT, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   30, 0, 30,
			   "Multiplicity", ""));
  }

  // Multiplicity (after cut)------------------------------------
  {
    const char* title = "BFT_CMulti";
    Int_t target_id = getUniqueID(kBFT, 0, kMulti, 10);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   30, 0, 30,
			   "Multiplicity", ""));
  }

  // TDC-2D (after cut) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kTDC2D, 0);
    // Add to the top directory
    top_dir->Add(createTH2(++target_id, "BFT_CTDC_U_2D", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   1024, 0, 1024,
			   "Segment", "TDC [ch]"));

    top_dir->Add(createTH2(++target_id, "BFT_CTDC_D_2D", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   1024, 0, 1024,
			   "Segment", "TDC [ch]"));
  }

  // TOT-2D (after cut) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kBFT, 0, kADC2D, 0);
    // Add to the top directory
    top_dir->Add(createTH2(++target_id, "BFT_CTOT_U_2D", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   200, -50, 150,
			   "Segment", "TOT [ch]"));

    top_dir->Add(createTH2(++target_id, "BFT_CTOT_D_2D", // 1 origin
			   NumOfSegBFT, 0, NumOfSegBFT,
			   200, -50, 150,
			   "Segment", "TOT [ch]"));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBC3
// -------------------------------------------------------------------------
TList* HistMaker::createBC3( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBC3);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"x0", "x1", "v0", "v1", "u0", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBC3, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kBC3, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBC3, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireBC3, 0, NumOfWireBC3,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kBC3, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kBC3, 0, kMulti, NumOfLayersBC3);
    for(Int_t i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBC4
// -------------------------------------------------------------------------
TList* HistMaker::createBC4( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBC4);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"u0", "u1", "v0", "v1", "x0", "x1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBC4, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kBC4, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBC4, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireBC4, 0, NumOfWireBC4,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kBC4, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kBC4, 0, kMulti, NumOfLayersBC4);
    for(Int_t i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBMW
// -------------------------------------------------------------------------
TList* HistMaker::createBMW( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBMW);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    Int_t target_id = getUniqueID(kBMW, 0, kADC, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, "BMW_ADC", // 1 origin
			   0x1000, 0, 0x1000,
			   "ADC [ch]", ""));
  }

  // TDC---------------------------------------------------------
  {
    Int_t target_id = getUniqueID(kBMW, 0, kTDC, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, "BMW_TDC", // 1 origin
			   0x1000, 0, 0x1000,
			   "TDC [ch]", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBH2
// -------------------------------------------------------------------------
TList* HistMaker::createBH2( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBH2);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH2, 0, kADC, 0);
    for( Int_t i=0; i<NumOfSegBH2*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegBH2 ){
	Int_t seg = i +1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i +1 -NumOfSegBH2; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC w/TDC ---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH2, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegBH2*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegBH2 ){
	Int_t seg = i +1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i +1 -NumOfSegBH2; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH2, 0, kTDC, 0);
    for( Int_t i=0; i<NumOfSegBH2*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegBH2 ){
	Int_t seg = i +1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i +1 -NumOfSegBH2; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }
  
  // ADC w/TDC (FPGA)---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = "ADCwFPGA_TDC";
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH2, 0, kADCwTDC, NumOfSegBH2*2);
    for( Int_t i=0; i<NumOfSegBH2*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegBH2 ){
	Int_t seg = i +1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i +1 -NumOfSegBH2; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC (FPGA)----------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = "FPGA_TDC";
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH2, 0, kTDC, NumOfSegBH2*2);
    for(Int_t i = 0; i<NumOfSegBH2*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH2){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH2; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     10000, 0, 400000,
			     50000, 0, 2000000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // MeanTimer (FPGA)----------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = "FPGA_TDC_MeanTimer";
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBH2, 0, kBH2MT, 0);
    for(Int_t i = 0; i<NumOfSegBH2; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     10000, 0, 400000,
			     50000, 0, 2000000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "BH2_hit_pattern";
    Int_t target_id = getUniqueID(kBH2, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegBH2, 0, NumOfSegBH2,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "BH2_multiplicity";
    Int_t target_id = getUniqueID(kBH2, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegBH2+1, 0, NumOfSegBH2+1,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBH2_E07
// -------------------------------------------------------------------------
TList* HistMaker::createBH2_E07( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kBH2_E07);   // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  { // ADC---------------------------------------------------------
    TString strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kBH2_E07, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegBH2_E07*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH2_E07){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH2_E07; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    TString strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kBH2_E07, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBH2_E07*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH2_E07){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegBH2_E07; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    const char* title = "BH2_E07_hit_pattern";
    Int_t target_id = getUniqueID(kBH2_E07, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   NumOfSegBH2_E07, 0, NumOfSegBH2_E07,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    const char* title = "BH2_E07_multiplicity";
    Int_t target_id = getUniqueID(kBH2_E07, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   NumOfSegBH2_E07+1, 0, NumOfSegBH2_E07+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createBAC
// -------------------------------------------------------------------------
TList* HistMaker::createBAC( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kBAC);       // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab

  const char* nameDetector = strDet.Data(); // Declaration of the directory
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[] = { "BAC1", "BAC2" };

  // ADC ---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName( nameSubDir );

    Int_t target_id = getUniqueID(kBAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegBAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // ADC w/TDC ---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADCwTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName( nameSubDir );

    Int_t target_id = getUniqueID(kBAC, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegBAC; ++i ){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kBAC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    Int_t target_id = getUniqueID(kBAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "BAC_hit_pattern",
			   NumOfSegBAC, 0, NumOfSegBAC,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    Int_t target_id = getUniqueID(kBAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "BAC_multiplicity",
			   NumOfSegBAC+1, 0, NumOfSegBAC+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createBAC_E07
// -------------------------------------------------------------------------
TList* HistMaker::createBAC_E07( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kBAC_E07);   // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab

  const char* nameDetector = strDet.Data(); // Declaration of the directory
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegBAC_E07] = {
    "BAC1-SUM", "BAC2-SUM", "BAC1-1", "BAC1-2", "BAC2-1", "BAC2-2"
  };

  { // ADC---------------------------------------------------------
    TString strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kBAC_E07, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegBAC_E07; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1( ++target_id, title, // 1 origin
			      0x1000, 0, 0x1000,
			      "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    TString strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kBAC_E07, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBAC_E07; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    Int_t target_id = getUniqueID(kBAC_E07, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "BAC_E07_hit_pattern",
			   NumOfSegBAC_E07, 0, NumOfSegBAC_E07,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    Int_t target_id = getUniqueID(kBAC_E07, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "BAC_E07_multiplicity",
			   NumOfSegBAC_E07+1, 0, NumOfSegBAC_E07+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createFBH
// -------------------------------------------------------------------------
TList* HistMaker::createFBH( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kFBH);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }
  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kFBH, 0, kTDC, 0);
    for(Int_t i=0; i<NumOfSegFBH*2; ++i){
      if( i<NumOfSegFBH ){
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dU", // 1 origin
						 nameDetector, nameSubDir, i+1),
			       1024, 0, 1024,
			       "TDC [ch]", ""));
      } else {
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dD", // 1 origin
						 nameDetector, nameSubDir, i+1-NumOfSegFBH),
			       1024, 0, 1024,
			       "TDC [ch]", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TOT---------------------------------------------------------
  {
    const char* nameSubDir = "TOT";
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kFBH, 0, kADC, 0);
    for(Int_t i=0; i<NumOfSegFBH*2; ++i){
      if(i<NumOfSegFBH){
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dU", // 1 origin
						 nameDetector, nameSubDir, i+1),
			       200, -50, 150,
			       "TOT [ch]", ""));
      }else{
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dD", // 1 origin
						 nameDetector, nameSubDir, i+1-NumOfSegFBH),
			       200, -50, 150,
			       "TOT [ch]", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TDC/TOT SUM -----------------------------------------------------
  {
    // TDC
    top_dir->Add(createTH1(getUniqueID(kFBH, 0, kTDC, NumOfSegFBH*2+1),
			   Form("%s_TDC", nameDetector),
			   1024, 0, 1024,
			   "TDC [ch]", ""));
    // TOT
    top_dir->Add(createTH1(getUniqueID(kFBH, 0, kADC, NumOfSegFBH*2+1),
			   Form("%s_TOT", nameDetector),
			   200, -50, 150,
			   "TOT [ch]", ""));
  }
  // TDC-2D --------------------------------------------
  {
    Int_t target_id = getUniqueID(kFBH, 0, kTDC2D, 0);
    top_dir->Add(createTH2(++target_id, "FBH_TDC2D", // 1 origin
			   NumOfSegFBH*2, 0, NumOfSegFBH*2,
			   1024, 0, 1024,
			   "Segment", "TDC [ch]"));
  }
  // TOT-2D --------------------------------------------
  {
    Int_t target_id = getUniqueID(kFBH, 0, kADC2D, 0);
    top_dir->Add(createTH2(++target_id, "FBH_TOT2D", // 1 origin
			   NumOfSegFBH*2, 0, NumOfSegFBH*2,
			   200, -50, 150,
			   "Segment", "TOT [ch]"));
  }
  // Hit parttern --------------------------------------------
  {
    Int_t target_id = getUniqueID(kFBH, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "FBH_HitPat", // 1 origin
			   NumOfSegFBH, 0, NumOfSegFBH,
			   "Segment", ""));
  }
  // Multiplicity -----------------------------------------------
  {
    const char* title = "FBH_multiplicity";
    Int_t target_id = getUniqueID(kFBH, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   NumOfSegFBH, 0, NumOfSegFBH,
			   "Multiplicity", ""));
  }
  return top_dir;
}

// -------------------------------------------------------------------------
// createSSDT
// -------------------------------------------------------------------------
TList* HistMaker::createSSDT( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kSSDT);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  { // TDC --------------------------------------------------------
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSDT, 0, kTDC, 0);
    for(Int_t seg=0; seg<NumOfSegSSDT; ++seg){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%d", nameDetector, nameSubDir, seg+1),
			     0x1000, 0, 0x1000,
			     "Segment", ""));
    }
    top_dir->Add(sub_dir);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSSD0
// -------------------------------------------------------------------------
TList* HistMaker::createSSD0( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kSSD0);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* nameLayer[NumOfLayersSSD0] = { "x0", "y0" };

  // ADC --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD0, 0, kADC2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD0; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0, 0, NumOfSegSSD0, 0x3ff, 0, 0x3ff,
			     "Segment", "Peak Height  "));
    }
    top_dir->Add(sub_dir);
  }

  // TDC --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD0, 0, kTDC2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD0; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0, 0, NumOfSegSSD0, NumOfSamplesSSD+1, 0, NumOfSamplesSSD+1,
			     "Segment", "Peak Position  "));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD0, 0, kHitPat, 0);
    for(Int_t l=0; l<NumOfLayersSSD0; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0, 0, NumOfSegSSD0,
			     "Segment", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0, 0, NumOfSegSSD0,
			     "Segment", ""));
    }
    target_id = getUniqueID(kSSD0, 0, kHitPat2D, 0);
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[1], nameLayer[0]),
			   NumOfSegSSD0/6, 0, NumOfSegSSD0, NumOfSegSSD0/6, 0, NumOfSegSSD0,
			   nameLayer[0], nameLayer[1]));
    // sub_dir->Add(createTH2(++target_id, Form("%s_C%s_%s%%%s", nameDetector, nameSubDir,
    // 					     nameLayer[1], nameLayer[0]),
    // 			   NumOfSegSSD0/6, 0, NumOfSegSSD0, NumOfSegSSD0/6, 0, NumOfSegSSD0,
    // 			   nameLayer[0], nameLayer[1]));
    top_dir->Add(sub_dir);
  }

  { // Multiplicity -----------------------------------------------
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD0, 0, kMulti, 0);
    for(Int_t l=0; l<NumOfLayersSSD0; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0+1, 0, NumOfSegSSD0+1,
			     "Multiplicity", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0+1, 0, NumOfSegSSD0+1,
			     "CMultiplicity", ""));
    }
    top_dir->Add(sub_dir);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSSD1
// -------------------------------------------------------------------------
TList* HistMaker::createSSD1( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kSSD1);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* nameLayer[NumOfLayersSSD1] = { "y0", "x0", "y1", "x1" };

  // ADC --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kADC2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1, 0, NumOfSegSSD1, 0x3ff, 0, 0x3ff,
			     "Segment", "Peak Height  "));
    }
    top_dir->Add(sub_dir);
  }

  // TDC --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kTDC2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1, 0, NumOfSegSSD1, NumOfSamplesSSD+1, 0, NumOfSamplesSSD+1,
			     "Segment", "Peak Position  "));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kHitPat, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1, 0, NumOfSegSSD1,
			     "Segment", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1, 0, NumOfSegSSD1,
			     "Segment", ""));
    }
    target_id = getUniqueID(kSSD1, 0, kHitPat2D, 0);
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[0], nameLayer[1]),
			   NumOfSegSSD1/12, 0, NumOfSegSSD1, NumOfSegSSD1/12, 0, NumOfSegSSD1,
			   nameLayer[1], nameLayer[0]));
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[2], nameLayer[3]),
			   NumOfSegSSD1/12, 0, NumOfSegSSD1, NumOfSegSSD1/12, 0, NumOfSegSSD1,
			   nameLayer[3], nameLayer[2]));
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kMulti, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     100, 0., 100.,
			     "Multiplicity", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     100, 0., 100.,
			     "CMultiplicity", ""));
    }
    top_dir->Add(sub_dir);
  }

  // DeltaE --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kDeltaE);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kDeltaE, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     200, 0., 40000., "DeltaE  "));
    }
    top_dir->Add(sub_dir);
  }

  // DeltaE2D --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kDeltaE2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kDeltaE2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1/4, 0, NumOfSegSSD1, 200, 0., 40000.,
			     "Segment", "DeltaE  "));
    }
    top_dir->Add(sub_dir);
  }

  // Time --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kCTime);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kCTime, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     200., 0, 200., "Peak Time [ns] "));
    }
    top_dir->Add(sub_dir);
  }

  // Time2D --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kCTime2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kCTime2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1/4, 0, NumOfSegSSD1, 200., 0, 200.,
			     "Segment", "Peak Time [ns] "));
    }
    top_dir->Add(sub_dir);
  }

  // Chisqr --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kChisqr);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD1, 0, kChisqr, 0);
    for(Int_t l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     500., 0, 500., "Chisqr  "));
    }
    top_dir->Add(sub_dir);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSSD2
// -------------------------------------------------------------------------
TList* HistMaker::createSSD2( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kSSD2);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* nameLayer[NumOfLayersSSD2] = { "x0", "y0", "x1", "y1" };

  // ADC --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kADC2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2, 0, NumOfSegSSD2, 0x3ff, 0, 0x3ff,
			     "Segment", "Peak Height  "));
    }
    top_dir->Add(sub_dir);
  }

  // TDC --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kTDC2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2, 0, NumOfSegSSD2, NumOfSamplesSSD+1, 0, NumOfSamplesSSD+1,
			     "Segment", "Peak Position  "));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kHitPat, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2, 0, NumOfSegSSD2,
			     "Segment", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2, 0, NumOfSegSSD2,
			     "Segment", ""));
    }
    target_id = getUniqueID(kSSD2, 0, kHitPat2D, 0);
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[1], nameLayer[0]),
			   NumOfSegSSD2/12, 0, NumOfSegSSD2, NumOfSegSSD2/12, 0, NumOfSegSSD2,
			   nameLayer[0], nameLayer[1]));
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[3], nameLayer[2]),
			   NumOfSegSSD2/12, 0, NumOfSegSSD2, NumOfSegSSD2/12, 0, NumOfSegSSD2,
			   nameLayer[2], nameLayer[3]));
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kMulti, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     100, 0., 100.,
			     "Multiplicity", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     100, 0., 100.,
			     "CMultiplicity", ""));
    }
    top_dir->Add(sub_dir);
  }

  // DeltaE --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kDeltaE);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kDeltaE, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     200, 0., 40000., "DeltaE  "));
    }
    top_dir->Add(sub_dir);
  }

  // DeltaE2D --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kDeltaE2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kDeltaE2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2/4, 0, NumOfSegSSD2, 200, 0., 40000.,
			     "Segment", "DeltaE  "));
    }
    top_dir->Add(sub_dir);
  }

  // Time --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kCTime);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kCTime, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     200., 0, 200., "Peak Time [ns] "));
    }
    top_dir->Add(sub_dir);
  }

  // Time2D --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kCTime2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kCTime2D, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2/4, 0, NumOfSegSSD2, 200., 0, 200.,
			     "Segment", "Peak Time [ns] "));
    }
    top_dir->Add(sub_dir);
  }

  // Chisqr --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kChisqr);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSSD2, 0, kChisqr, 0);
    for(Int_t l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     500., 0, 500., "Chisqr  "));
    }
    top_dir->Add(sub_dir);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createPVAC
// -------------------------------------------------------------------------
TList* HistMaker::createPVAC( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kPVAC);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegPVAC] = { "PVAC" };

  // ADC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPVAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegPVAC; ++i){
      sub_dir->Add(createTH1( ++target_id,
			      Form("%s_%s", name_acs[i], nameSubDir),
			      0x1000, 0, 0x1000,
			      "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // ADC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPVAC, 0, kADCwTDC, 0);
    for(Int_t i = 0; i<NumOfSegPVAC; ++i){
      sub_dir->Add(createTH1( ++target_id,
			      Form("%s_%s", name_acs[i], nameSubDir),
			      0x1000, 0, 0x1000,
			      "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPVAC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegPVAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kPVAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "PVAC_hit_pattern",
			   NumOfSegPVAC, 0, NumOfSegPVAC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kPVAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "PVAC_multiplicity",
			   NumOfSegPVAC+1, 0, NumOfSegPVAC+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createFAC
// -------------------------------------------------------------------------
TList* HistMaker::createFAC( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kFAC);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegFAC] = { "FAC" };

  // ADC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kFAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegFAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // ADC ---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kFAC, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegFAC; ++i ){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kFAC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegFAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kFAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "FAC_hit_pattern",
			   NumOfSegFAC, 0, NumOfSegFAC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kFAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "FAC_multiplicity",
			   NumOfSegFAC+1, 0, NumOfSegFAC+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSAC1
// -------------------------------------------------------------------------
TList* HistMaker::createSAC1( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kSAC1);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegSAC1] = { "SAC1" };

  // ADC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSAC1, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegSAC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // ADC w/TDC ---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSAC1, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegSAC1; ++i ){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSAC1, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegSAC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kSAC1, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "SAC1_hit_pattern",
			   NumOfSegSAC1, 0, NumOfSegSAC1,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kSAC1, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "SAC1_multiplicity",
			   NumOfSegSAC1+1-4, 0, NumOfSegSAC1+1-4,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSCH
// -------------------------------------------------------------------------
TList* HistMaker::createSCH( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSCH);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }
  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSCH, 0, kTDC, 0);
    for(Int_t i=0; i<NumOfSegSCH; ++i){
      sub_dir->Add(createTH1(++target_id, Form("%s_%s_%d", // 1 origin
					       nameDetector, nameSubDir, i+1),
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }
  // TOT---------------------------------------------------------
  {
    const char* nameSubDir = "TOT";
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t target_id = getUniqueID(kSCH, 0, kADC, 0);
    for(Int_t i=0; i<NumOfSegSCH; ++i){
      sub_dir->Add(createTH1(++target_id, Form("%s_%s_%d", // 1 origin
					       nameDetector, nameSubDir, i+1),
			     200, -50, 150,
			     "TOT [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }
  // TDC/TOT SUM -----------------------------------------------------
  {
    // TDC
    top_dir->Add(createTH1(getUniqueID(kSCH, 0, kTDC, NumOfSegSCH+1),
			   Form("%s_TDC", nameDetector),
			   1024, 0, 1024,
			   "TDC [ch]", ""));
    // TOT
    top_dir->Add(createTH1(getUniqueID(kSCH, 0, kADC, NumOfSegSCH+1),
			   Form("%s_TOT", nameDetector),
			   200, -50, 150,
			   "TOT [ch]", ""));
  }
  // TDC-2D --------------------------------------------
  {
    Int_t target_id = getUniqueID(kSCH, 0, kTDC2D, 0);
    top_dir->Add(createTH2(++target_id, "SCH_TDC_2D", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   1024, 0, 1024,
			   "Segment", "TDC [ch]"));
  }
  // TOT-2D --------------------------------------------
  {
    Int_t target_id = getUniqueID(kSCH, 0, kADC2D, 0);
    top_dir->Add(createTH2(++target_id, "SCH_TOT_2D", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   200, -50, 150,
			   "Segment", "TOT [ch]"));
  }
  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kSCH, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "SCH_HitPat", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   "Segment", ""));
  }
  // Multiplicity -----------------------------------------------
  {
    const char* title = "SCH_multiplicity";
    Int_t target_id = getUniqueID(kSCH, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   20, 0, 20,
			   "Multiplicity", ""));
  }
  return top_dir;
}

// -------------------------------------------------------------------------
// createKFAC
// -------------------------------------------------------------------------
TList* HistMaker::createKFAC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kKFAC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kKFAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegKFAC; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    // ADC2D
    target_id = getUniqueID(kKFAC, 0, kADC2D, 0);
    sub_dir->Add(createTH2(++target_id, "KFAC_ADC_2D",
			   0x1000, 0, 0x1000, 0x1000, 0, 0x1000,
			   "ADC [ch]", "ADC [ch]"));

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createEMC
// -------------------------------------------------------------------------
TList* HistMaker::createEMC( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kEMC);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  { // XY ---------------------------------------------------------
    //Int_t serial_id = getUniqueID(kEMC, 0, kSerial, 0);
    Int_t xpos_id   = getUniqueID(kEMC, 0, kXpos,   0);
    Int_t ypos_id   = getUniqueID(kEMC, 0, kYpos,   0);
    Int_t xypos_id  = getUniqueID(kEMC, 0, kXYpos,  0);
    //Int_t time_id   = getUniqueID(kEMC, 0, kTime,   0);

    for(Int_t i = 0; i<NumOfSegEMC; ++i){
      top_dir->Add(createTH1( ++xpos_id,
			      Form("%s_Xpos", strDet.Data()),
			      0x20, -0x100, 0x100,
			      "Xpos [mm]", ""));
      top_dir->Add(createTH1( ++ypos_id,
			      Form("%s_Ypos", strDet.Data()),
			      0x20, -0x100, 0x100,
			      "Ypos [mm]", ""));
      top_dir->Add(createTH2( ++xypos_id,
			      Form("%s_XYpos", strDet.Data()),
			      0x80, -0x100, 0x100, 0x80, -0x100, 0x100,
			      "Xpos [mm]", "Ypos [mm]"));
    }
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createKIC
// -------------------------------------------------------------------------
TList* HistMaker::createKIC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kKIC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_kics[] = { "U", "D", "L", "R" };

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kKIC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegKIC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_kics[i]);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kKIC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegKIC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_kics[i]);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "KIC_hit_pattern";
    Int_t target_id = getUniqueID(kKIC, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegKIC, 0, NumOfSegKIC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "KIC_multiplicity";
    Int_t target_id = getUniqueID(kKIC, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegKIC+1, 0, NumOfSegKIC+1,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSDC1
// -------------------------------------------------------------------------
TList* HistMaker::createSDC1( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSDC1);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"v0", "v1", "x0", "x1", "u0", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC1, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersSDC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kSDC1, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfLayersSDC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC1, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersSDC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireSDC1, 0, NumOfWireSDC1,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kSDC1, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersSDC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     20, 0., 20.,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC1, 0, kMulti, NumOfLayersSDC1);
    for(Int_t i = 0; i<NumOfLayersSDC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     20, 0., 20.,
			     "Multiplicity", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSDC2
// -------------------------------------------------------------------------
TList* HistMaker::createSDC2( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSDC2);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[NumOfLayersSDC2] = { "x0", "x1", "y0", "y1" };
  //  const char* name_layer[NumOfLayersSDC3] = { "y0", "y1", "x0", "x1" };

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC2, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kSDC2, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }

    // TDCwTOTCUT -------------------------------------------------
    // Make histogram and add it
    target_id = getUniqueID(kSDC2, 0, kTDC, 10);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kSDC2, 0, kTDC2D, 10);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_C%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSDC2, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     500, 0, 500,
			     "TOT [ch]", ""));
    }

    // CTOT---------------------------------------------------------
    target_id = getUniqueID(kSDC2, 0, kADC, 10);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     500, 0, 500,
			     "TOT [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC2, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireSDC2, 0, NumOfWireSDC2,
			     "wire", ""));
    }

    // HitPatwTOTCUT -----------------------------------------------
    // Make histogram and add it
    target_id = getUniqueID(kSDC2, 0, kHitPat, 10);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireSDC2, 0, NumOfWireSDC2,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kSDC2, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC2, 0, kMulti, NumOfLayersSDC2);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }

    // MultiplicitywTOTCUT ----------------------------------------
    // Make histogram and add it
    // without TDC gate
    target_id = getUniqueID(kSDC2, 0, kMulti, 10);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC2, 0, kMulti, NumOfLayersSDC2+10);
    for(Int_t i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }


    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createHDC
// -------------------------------------------------------------------------
TList* HistMaker::createHDC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kHDC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"u0", "u1", "x0", "x1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kHDC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kHDC, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kHDC, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireHDC, 0, NumOfWireHDC,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kHDC, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kHDC, 0, kMulti, NumOfLayersHDC);
    for(Int_t i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSP0
// -------------------------------------------------------------------------
TList* HistMaker::createSP0( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSP0);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // Declaration of sub-detector directory
  for(Int_t sd = 0; sd<NumOfLayersSP0; ++sd){
    const char* nameSubDetector = Form("SP0_L%d", sd+1);
    TList *subdet_dir = new TList;
    subdet_dir->SetName(nameSubDetector);

    // ADC---------------------------------------------------------
    {
      // Declaration of the sub-directory
      TString strSubDir  = CONV_STRING(kADC);
      const char* nameSubDir = strSubDir.Data();
      TList *sub_dir = new TList;
      sub_dir->SetName(nameSubDir);

      // Make histogram and add it
      Int_t target_id = getUniqueID(kSP0, kSP0_L1+sd, kADC, 0);
      for(Int_t i = 0; i<NumOfSegSP0*2; ++i){
	const char* title = NULL;
	if(i < NumOfSegSP0){
	  Int_t seg = i+1; // 1 origin
	  title = Form("%s_%s_%dU", nameSubDetector, nameSubDir, seg);
	}else{
	  Int_t seg = i+1-NumOfSegSP0; // 1 origin
	  title = Form("%s_%s_%dD", nameSubDetector, nameSubDir, seg);
	}

	sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			       0x1000, 0, 0x1000,
			       "ADC [ch]", ""));
      }

      // insert sub directory
      subdet_dir->Add(sub_dir);
    }

    // TDC---------------------------------------------------------
    {
      // Declaration of the sub-directory
      TString strSubDir  = CONV_STRING(kTDC);
      const char* nameSubDir = strSubDir.Data();
      TList *sub_dir = new TList;
      sub_dir->SetName(nameSubDir);

      // Make histogram and add it
      Int_t target_id = getUniqueID(kSP0, kSP0_L1+sd, kTDC, 0);
      for(Int_t i = 0; i<NumOfSegSP0*2; ++i){
	const char* title = NULL;
	if(i < NumOfSegSP0){
	  Int_t seg = i+1; // 1 origin
	  title = Form("%s_%s_%dU", nameSubDetector, nameSubDir, seg);
	}else{
	  Int_t seg = i+1-NumOfSegSP0; // 1 origin
	  title = Form("%s_%s_%dD", nameSubDetector, nameSubDir, seg);
	}

	sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			       0x1000, 0, 0x1000,
			       "TDC [ch]", ""));
      }

      // insert sub directory
      subdet_dir->Add(sub_dir);
    }

    // HitPat-------------------------------------------------------
    {
      // Make histogram and add it
      Int_t target_id = getUniqueID(kSP0, kSP0_L1+sd, kHitPat, 0);
      for(Int_t i = 0; i<2; ++i){
	const char* title = NULL;
	if(i == 0){
	  title = Form("%s_HitPat_U", nameSubDetector);
	}else{
	  title = Form("%s_HitPat_D", nameSubDetector);
	}

	subdet_dir->Add(createTH1(target_id + i+1, title, // 1 origin
				  NumOfSegSP0, 0, NumOfSegSP0,
				  "Segment", ""));
      }
    }

    top_dir->Add(subdet_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

//// -------------------------------------------------------------------------
//// createSDC3
//// -------------------------------------------------------------------------
//TList* HistMaker::createSDC3( Bool_t flag_ps )
//{
//  // Determine the detector name
//  TString strDet = CONV_STRING(kSDC3);
//  // name list of crearted detector
//  name_created_detectors_.push_back(strDet);
//  if(flag_ps){
//    // name list which are displayed in Ps tab
//    name_ps_files_.push_back(strDet);
//  }
//
//  // Declaration of the directory
//  // Just type conversion from TString to char*
//  const char* nameDetector = strDet.Data();
//  TList *top_dir = new TList;
//  top_dir->SetName(nameDetector);
//
//  // layer configuration
//  const char* name_layer[NumOfLayersSDC3] = { "y0", "y1", "x0", "x1" };
//
//  // TDC---------------------------------------------------------
//  {
//    // Declaration of the sub-directory
//    TString strSubDir  = CONV_STRING(kTDC);
//    const char* nameSubDir = strSubDir.Data();
//    TList *sub_dir = new TList;
//    sub_dir->SetName(nameSubDir);
//
//    // Make histogram and add it
//    Int_t target_id = getUniqueID(kSDC3, 0, kTDC, 0);
//    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
//      const char* title = NULL;
//      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
//      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     2000, 0, 2000,
//			     "TDC [ch]", ""));
//    }
//    target_id = getUniqueID(kSDC3, 0, kTDC2D, 0);
//    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
//      const char* title = NULL;
//      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
//      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     2000, 0, 2000,
//			     "TDC [ch]", ""));
//    }
//
//    // insert sub directory
//    top_dir->Add(sub_dir);
//  }
//
//  // HitPat------------------------------------------------------
//  {
//    // Declaration of the sub-directory
//    TString strSubDir  = CONV_STRING(kHitPat);
//    const char* nameSubDir = strSubDir.Data();
//    TList *sub_dir = new TList;
//    sub_dir->SetName(nameSubDir);
//
//    // Make histogram and add it
//    Int_t target_id = getUniqueID(kSDC3, 0, kHitPat, 0);
//    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
//      Int_t nwire;
//      if( i==0 || i==1 ) nwire = NumOfWireSDC3Y;
//      if( i==2 || i==3 ) nwire = NumOfWireSDC3X;
//      const char* title = NULL;
//      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
//      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     nwire, 0, nwire,
//			     "wire", ""));
//    }
//
//    // insert sub directory
//    top_dir->Add(sub_dir);
//  }
//
//  // Multiplicity -----------------------------------------------
//  {
//    // Declaration of the sub-directory
//    TString strSubDir  = CONV_STRING(kMulti);
//    const char* nameSubDir = strSubDir.Data();
//    TList *sub_dir = new TList;
//    sub_dir->SetName(nameSubDir);
//
//    // Make histogram and add it
//    // without TDC gate
//    Int_t target_id = getUniqueID(kSDC3, 0, kMulti, 0);
//    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
//      // Int_t nwire;
//      // if( i==0 || i==1 ) nwire = NumOfWireSDC3Y;
//      // if( i==2 || i==3 ) nwire = NumOfWireSDC3X;
//      const char* title = NULL;
//      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
//      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
////			     20, 0, 20,
//			     100, 0, 100,
//			     "Multiplicity", ""));
//    }
//
//    // with TDC gate
//    target_id = getUniqueID(kSDC3, 0, kMulti, NumOfLayersSDC3);
//    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
//      const char* title = NULL;
//      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
//      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
////			     20, 0, 20,
//			     100, 0, 100,
//			     "Multiplicity", ""));
//    }
//
//    // insert sub directory
//    top_dir->Add(sub_dir);
//  }
//
//  // Return the TList pointer which is added into TGFileBrowser
//  return top_dir;
//}

// -------------------------------------------------------------------------
// createSDC3
// -------------------------------------------------------------------------
TList* HistMaker::createSDC3( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSDC3);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[NumOfLayersSDC3] = { "y0", "y1", "x0", "x1" };

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC3, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kSDC3, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }

    // TDCwTOTCUT -------------------------------------------------
    // Make histogram and add it
    target_id = getUniqueID(kSDC3, 0, kTDC, 10);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kSDC3, 0, kTDC2D, 10);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_C%s1st_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSDC3, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     800, 0, 800,
			     "TOT [ch]", ""));
    }
    // CTOT---------------------------------------------------------
    target_id = getUniqueID(kSDC3, 0, kADC, 10);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     800, 0, 800,
			     "TOT [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }


  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    Int_t nwire;

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC3, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      if( i==0 || i==1 ) nwire = NumOfWireSDC3Y;
      if( i==2 || i==3 ) nwire = NumOfWireSDC3X;
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     nwire, 0, nwire,
			     "wire", ""));
    }

    // HitPatwTOTCUT -----------------------------------------------
    // Make histogram and add it
    target_id = getUniqueID(kSDC3, 0, kHitPat, 10);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      if( i==0 || i==1 ) nwire = NumOfWireSDC3Y;
      if( i==2 || i==3 ) nwire = NumOfWireSDC3X;
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     nwire, 0, nwire,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kSDC3, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC3, 0, kMulti, NumOfLayersSDC3);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }

    // MultiplicitywTOTCUT ----------------------------------------
    // Make histogram and add it
    // without TDC gate
    target_id = getUniqueID(kSDC3, 0, kMulti, 10);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC3, 0, kMulti, NumOfLayersSDC3+10);
    for(Int_t i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_C%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     20, 0, 20,
			     128, 0, 128,
			     "Multiplicity", ""));
    }


    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSDC4
// -------------------------------------------------------------------------
TList* HistMaker::createSDC4( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSDC4);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"v0", "x0", "u0", "v1", "x1", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC4, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfLayersSDC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1600, 0, 1600,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSDC4, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfLayersSDC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireSDC4uv, 0, NumOfWireSDC4uv,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    Int_t target_id = getUniqueID(kSDC4, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfLayersSDC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC4, 0, kMulti, NumOfLayersSDC4);
    for(Int_t i = 0; i<NumOfLayersSDC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_wTDC", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createTOF
// -------------------------------------------------------------------------
TList* HistMaker::createTOF( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kTOF);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kTOF, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegTOF*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegTOF){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegTOF; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADCwTDC ---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kTOF, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegTOF*2; ++i ){
      const char* title = NULL;
      if( i<NumOfSegTOF ){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegTOF; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kTOF, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegTOF*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegTOF){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	Int_t seg = i+1-NumOfSegTOF; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "TOF_hit_pattern";
    Int_t target_id = getUniqueID(kTOF, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "TOF_multiplicity";
    Int_t target_id = getUniqueID(kTOF, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}


// -------------------------------------------------------------------------
// createTOF_HT
// -------------------------------------------------------------------------
TList* HistMaker::createTOF_HT( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kTOF_HT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  const char* name_ch[] = { "(1,11)", "(2,20)", "(3,18)", "(4,21)", "(5,9)",
                            "(6,22)", "(7,23)", "(8,24)", "(10,19)",
                            "12", "13", "14", "15", "16", "17"};


  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  //  // ADC---------------------------------------------------------
  //  {
  //    // Declaration of the sub-directory
  //    TString strSubDir  = CONV_STRING(kADC);
  //    const char* nameSubDir = strSubDir.Data();
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //
  //    // Make histogram and add it
  //    Int_t target_id = getUniqueID(kTOF_HT, 0, kADC, 0);
  //    for(Int_t i = 0; i<NumOfSegTOF_HT*2; ++i){
  //      const char* title = NULL;
  //      if(i < NumOfSegTOF_HT){
  //	Int_t seg = i+1; // 1 origin
  //	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
  //      }else{
  //	Int_t seg = i+1-NumOfSegTOF_HT; // 1 origin
  //	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
  //      }
  //
  //      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  //			     0x1000, 0, 0x1000,
  //			     "ADC [ch]", ""));
  //    }
  //
  //    // insert sub directory
  //    top_dir->Add(sub_dir);
  //  }
  //
  //  // ADCwTDC ---------------------------------------------------------
  //  {
  //    // Declaration of the sub-directory
  //    TString strSubDir  = CONV_STRING(kADCwTDC);
  //    const char* nameSubDir = strSubDir.Data();
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //
  //    // Make histogram and add it
  //    Int_t target_id = getUniqueID(kTOF_HT, 0, kADCwTDC, 0);
  //    for( Int_t i=0; i<NumOfSegTOF_HT*2; ++i ){
  //      const char* title = NULL;
  //      if( i<NumOfSegTOF_HT ){
  //	Int_t seg = i+1; // 1 origin
  //	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
  //      }else{
  //	Int_t seg = i+1-NumOfSegTOF_HT; // 1 origin
  //	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
  //      }
  //
  //      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  //			     0x1000, 0, 0x1000,
  //			     "ADC [ch]", ""));
  //    }
  //
  //    // insert sub directory
  //    top_dir->Add(sub_dir);
  //  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    // Make histogram and add it
    Int_t target_id = getUniqueID(kTOF_HT, 0, kTDC, 0);
    //    for(Int_t i = 0; i<NumOfSegTOF_HT*2; ++i){
    for(Int_t i = 0; i<NumOfSegTOF_HT; ++i){
      const char* title = NULL;
      if(i < NumOfSegTOF_HT){
	//	Int_t seg = i+1; // 1 origin
	//	title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
	//	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg;
	title = Form("%s_%s_%s", nameDetector, nameSubDir, name_ch[i]);
	//      }else{
	//	Int_t seg = i+1-NumOfSegTOF_HT; // 1 origin
	//	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "TOF_HT_hit_pattern";
    Int_t target_id = getUniqueID(kTOF_HT, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF_HT, 0, NumOfSegTOF_HT,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "TOF_HT_multiplicity";
    Int_t target_id = getUniqueID(kTOF_HT, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF_HT, 0, NumOfSegTOF_HT,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createTOFMT
// -------------------------------------------------------------------------
TList* HistMaker::createTOFMT( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kTOFMT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kTOFMT, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegTOF; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "TOFMT_hit_pattern";
    Int_t target_id = getUniqueID(kTOFMT, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "TOFMT_multiplicity";
    Int_t target_id = getUniqueID(kTOFMT, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createLAC
// -------------------------------------------------------------------------
TList* HistMaker::createLAC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kLAC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kLAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegLAC; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kLAC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegLAC; ++i){
      const char* title = NULL;
      if(i < NumOfSegLAC){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "LAC_hit_pattern";
    Int_t target_id = getUniqueID(kLAC, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegLAC, 0, NumOfSegLAC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "LAC_multiplicity";
    Int_t target_id = getUniqueID(kLAC, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegLAC, 0, NumOfSegLAC,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createLC
// -------------------------------------------------------------------------
TList* HistMaker::createLC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kLC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  //  // ADC---------------------------------------------------------
  //  {
  //    // Declaration of the sub-directory
  //    TString strSubDir  = CONV_STRING(kADC);
  //    const char* nameSubDir = strSubDir.Data();
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //
  //    // Make histogram and add it
  //    Int_t target_id = getUniqueID(kLC, 0, kADC, 0);
  //    for(Int_t i = 0; i<NumOfSegLC*2; ++i){
  //      const char* title = NULL;
  //      if(i < NumOfSegLC){
  //	Int_t seg = i+1; // 1 origin
  //	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
  //      }else{
  //	Int_t seg = i+1-NumOfSegLC; // 1 origin
  //	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
  //      }
  //
  //      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  //			     0x1000, 0, 0x1000,
  //			     "ADC [ch]", ""));
  //    }
  //
  //    // insert sub directory
  //    top_dir->Add(sub_dir);
  //  }
  //
  //  // ADCwTDC ---------------------------------------------------------
  //  {
  //    // Declaration of the sub-directory
  //    TString strSubDir  = CONV_STRING(kADCwTDC);
  //    const char* nameSubDir = strSubDir.Data();
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //
  //    // Make histogram and add it
  //    Int_t target_id = getUniqueID(kLC, 0, kADCwTDC, 0);
  //    for( Int_t i=0; i<NumOfSegLC*2; ++i ){
  //      const char* title = NULL;
  //      if( i<NumOfSegLC ){
  //	Int_t seg = i+1; // 1 origin
  //	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
  //      }else{
  //	Int_t seg = i+1-NumOfSegLC; // 1 origin
  //	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
  //      }
  //
  //      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  //			     0x1000, 0, 0x1000,
  //			     "ADC [ch]", ""));
  //    }
  //
  //    // insert sub directory
  //    top_dir->Add(sub_dir);
  //  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kLC, 0, kTDC, 0);
    //    for(Int_t i = 0; i<NumOfSegLC*2; ++i){
    for(Int_t i = 0; i<NumOfSegLC; ++i){
      const char* title = NULL;
      if(i < NumOfSegLC){
	//	Int_t seg = i+1; // 1 origin
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
	//      }else{
	//	Int_t seg = i+1-NumOfSegLC; // 1 origin
	//	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "LC_hit_pattern";
    Int_t target_id = getUniqueID(kLC, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegLC, 0, NumOfSegLC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "LC_multiplicity";
    Int_t target_id = getUniqueID(kLC, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegLC, 0, NumOfSegLC,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createMsT
// -------------------------------------------------------------------------
TList* HistMaker::createMsT( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kMsT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // HR-TDC ---------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("MsT_TOF_TDC");
    Int_t target_id = getUniqueID(kMsT, 0, kTDC, 0);
    for(Int_t seg=0; seg<NumOfSegTOF; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_TOF_TDC_%d", nameDetector, seg+1),
			     50000, 0, 2000000,
			     "TDC", ""));
    }
    top_dir->Add(sub_dir);
  }

  // HR-TDC (accept) ---------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("MsT_TOF_TDC_Acc");
    Int_t target_id = getUniqueID(kMsT, 0, kTDC, NumOfSegTOF);
    for(Int_t seg=0; seg<NumOfSegTOF; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_TOF_TDC_%d_Acc", nameDetector, seg+1),
			     10000, 0, 400000,
			     "TDC", ""));
    }
    top_dir->Add(sub_dir);
  }

  // LR-TDC ---------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("MsT_SCH_TDC");
    Int_t target_id = getUniqueID(kMsT, 0, kTDC, NumOfSegTOF*2);
    for(Int_t seg=0; seg<NumOfSegSCH; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_SCH_TDC_%d", nameDetector, seg+1),
			     50, 0, 50,
			     "TDC", ""));
    }
    top_dir->Add(sub_dir);
  }

  // Timing Counter TDC 2D -------------------------------------------------------
  {
    Int_t target_id = getUniqueID(kMsT, 0, kTDC2D, 0);
    top_dir->Add(createTH2(++target_id, Form("%s_TOF_TDC_2D", nameDetector),
			   NumOfSegTOF, 0, NumOfSegTOF, 50000, 0, 2000000,
			   "Segment", ""));
  }

  // HitPattern  ----------------------------------------------------
  {
    Int_t target_id = getUniqueID(kMsT, 0, kHitPat, 0);
    top_dir->Add(createTH1(target_id, Form("%s_TOF_HitPat", nameDetector),
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Segment", ""));
    top_dir->Add(createTH1(target_id + 1, Form("%s_SCH_HitPat", nameDetector),
			   NumOfSegSCH, 0, NumOfSegSCH,
			   "Segment", ""));
    top_dir->Add(createTH1(target_id + 2, "MsT_Flag",
			   10, 0., 10., ""));
  }

  // Hit parttern 2D Flag --------------------------------------------
  {
    Int_t target_id = getUniqueID(kMsT, 0, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id, "MsT_Flag_2D", // 1 origin
			   4, 0., 4., 4, 0., 4.,
			   "MsT Flag", "Soft Flag  "));
    GHist::get(target_id)->SetMarkerSize(6);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createMtx3D
// -------------------------------------------------------------------------
TList* HistMaker::createMtx3D( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kMtx3D);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC TOF ---------------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("Mtx_TOF_TDC");
    Int_t target_id = getUniqueID(kMtx3D, kHulTOF, kTDC, 0);
    for(Int_t seg=0; seg<NumOfSegTOF; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_TOF_TDC_%d", nameDetector, seg+1),
			     0xff, 0, 0xff,
			     "TDC [5ns/bin]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC FBH ---------------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("Mtx_FBH_TDC");
    Int_t target_id = getUniqueID(kMtx3D, kHulFBH, kTDC, 0);
    for(Int_t seg=0; seg<NumOfSegClusteredFBH; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_FBH_TDC_%d", nameDetector, seg+1),
			     0xff, 0, 0xff,
			     "TDC [5ns/bin]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC SCH ---------------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("Mtx_SCH_TDC");
    Int_t target_id = getUniqueID(kMtx3D, kHulSCH, kTDC, 0);
    for(Int_t seg=0; seg<NumOfSegSCH; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_SCH_TDC_%d", nameDetector, seg+1),
			     0xff, 0, 0xff,
			     "TDC [5ns/bin]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // Hit parttern 2D (TOFxFBH) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kMtx3D, kHulTOFxFBH, kHitPat2D, 0);
    top_dir->Add(createTH2(++target_id, "Mtx_TOFxFBH", // 1 origin
			   NumOfSegClusteredFBH, 0, NumOfSegClusteredFBH,
			   NumOfSegTOF,          0, NumOfSegTOF,
			   "Clustered FBH seg", "TOF seg"));
    GHist::get(target_id)->SetMarkerSize(6);
  }

  // Hit parttern 2D (TOFxSCH) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kMtx3D, kHulTOFxSCH, kHitPat2D, 0);
    top_dir->Add(createTH2(++target_id, "Mtx_TOFxSCH", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "SCH seg", "TOF seg"));
    GHist::get(target_id)->SetMarkerSize(6);
  }

  // Hit parttern 2D (FBHxSCH) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kMtx3D, kHulFBHxSCH, kHitPat2D, 0);
    top_dir->Add(createTH2(++target_id, "Mtx_FBHxSCH", // 1 origin
			   NumOfSegSCH,          0, NumOfSegSCH,
			   NumOfSegClusteredFBH, 0, NumOfSegClusteredFBH,
			   "SCH seg","Clustered FBH seg"));
    GHist::get(target_id)->SetMarkerSize(6);
  }

  // Hit parttern 2D (TOFxSCH FBH required) ----------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("Mtx2D_TOF_FBH");
    Int_t target_id = getUniqueID(kMtx3D, kHul2DHitPat, kHitPat2D, 0);
    sub_dir->Add(createTH2(++target_id, "Mtx2D_TOFxSCH", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "SCH seg", "TOF seg"));
    GHist::get(target_id)->SetMarkerSize(6);
    top_dir->Add(sub_dir);
  }

  // Hit parttern 3D (TOFxSCH FBH required) ----------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("Mtx3D_TOF_FBH");
    Int_t target_id = getUniqueID(kMtx3D, kHul3DHitPat, kHitPat2D, 0);
    for(Int_t i = 0; i<NumOfSegClusteredFBH; ++i){
      sub_dir->Add(createTH2(++target_id, Form("Mtx3D_TOFxSCH_FBH%d",i), // 1 origin
			     NumOfSegSCH, 0, NumOfSegSCH,
			     NumOfSegTOF, 0, NumOfSegTOF,
			     "SCH seg", "TOF seg"));
      GHist::get(target_id)->SetMarkerSize(6);
    }// for(i)
    top_dir->Add(sub_dir);
  }

  // Hit parttern 2D (Flag) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kMtx3D, kHul2D, kHitPat2D, 0);
    top_dir->Add(createTH2(++target_id, "Mtx_2D_FLAG", // 1 origin
			   3, 0., 3., 3, 0., 3.,
			   "Trigger Flag", "HUL Accept"));
    GHist::get(target_id)->SetMarkerSize(6);
  }

  // Hit parttern 3D (Flag) --------------------------------------------
  {
    Int_t target_id = getUniqueID(kMtx3D, kHul3D, kHitPat2D, 0);
    top_dir->Add(createTH2(++target_id, "Mtx_3D_FLAG", // 1 origin
			   3, 0., 3., 3, 0., 3.,
			   "Trigger Flag", "HUL Accept"));
    GHist::get(target_id)->SetMarkerSize(6);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createTriggerFlag
// -------------------------------------------------------------------------
TList* HistMaker::createTriggerFlag( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kTriggerFlag);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    // Make histogram and add it
    Int_t target_id = getUniqueID(kTriggerFlag, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegTFlag; ++i){
      const char* title = NULL;
      title = Form("%s_%d", nameDetector, i+1);
      top_dir->Add(createTH1(++target_id, title, // 1 origin
			     400, 0, 4000,
			     "TDC [ch]", ""));
    }
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "Trigger_Entry";
    Int_t target_id = getUniqueID(kTriggerFlag, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   NumOfSegTFlag+1, 0., NumOfSegTFlag+1,
			   "Trigger flag", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createTriggerFlag_E07
// -------------------------------------------------------------------------
TList* HistMaker::createTriggerFlag_E07( Bool_t flag_ps )
{
  // Determine the detector name
  //TString strDet = CONV_STRING(kTriggerFlag);
  TString strDet = "TriggerFlag_E07";
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    // Make histogram and add it
    Int_t target_id = getUniqueID(kTriggerFlag, 1, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegTFlag; ++i){
      const char* title = NULL;
      title = Form("%s_%d", nameDetector, i+1);
      top_dir->Add(createTH1(++target_id, title, // 1 origin
			     400, 0, 4000,
			     "TDC [ch]", ""));
    }
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "Trigger_Entry_E07";
    Int_t target_id = getUniqueID(kTriggerFlag, 1, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   20, 0, 20,
			   "Trigger flag", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createCorrelation
// -------------------------------------------------------------------------
TList* HistMaker::createCorrelation( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kCorrelation);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  {
    Int_t target_id = getUniqueID(kCorrelation, 0, 0, 0);
    // BH1 vs BFT -----------------------------------------------
    top_dir->Add(createTH2( ++target_id, "BH1_BFT", // 1 origin
			    NumOfSegBH1, 0, NumOfSegBH1,
			    NumOfSegBFT, 0, NumOfSegBFT,
			    "BFT seg", "BH1 seg" ) );

    // BH2 vs BH1 -----------------------------------------------
    top_dir->Add(createTH2(++target_id, "BH2_BH1", // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   NumOfSegBH2, 0, NumOfSegBH2,
			   "BH1 seg", "BH2 seg"));

    // SCH vs TOF -----------------------------------------------
    top_dir->Add(createTH2(++target_id, "SCH_TOF", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "SCH seg", "TOF seg"));

    // BC4 vs BC3 ----------------------------------------------
    top_dir->Add(createTH2(++target_id, "BC4x1_BC3x0", // 1 origin
			   NumOfWireBC3, 0, NumOfWireBC3,
			   NumOfWireBC4, 0, NumOfWireBC4,
			   "BC3 wire", "BC4 wire"));

    // SDC2 vs SDC1 --------------------------------------------
    top_dir->Add(createTH2(++target_id, "SDC2x1_SDC1x0", // 1 origin
			   NumOfWireSDC1, 0, NumOfWireSDC1,
			   NumOfWireSDC2, 0, NumOfWireSDC2,
			   "SDC1 wire", "SDC2 wire"));

    // TOF vs SDC3 --------------------------------------------
    top_dir->Add(createTH2(++target_id, "TOF_SDC3x0", // 1 origin
			   NumOfWireSDC3X, 0, NumOfWireSDC3X,
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "SDC3 wire", "TOF seg"));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createCorrelatio_CATCH
// -------------------------------------------------------------------------
TList* HistMaker::createCorrelation_catch( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kCorrelation_catch);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // BGO vs CFT -----------------------------------------------
  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_Layer[] = { "UV1" ,"PHI1", "UV2", "PHI2",
                               "UV3" ,"PHI3", "UV4", "PHI4",
                               "UV_ALL" ,"PHI_ALL"};

  {
    Int_t target_id = getUniqueID(kCorrelation_catch, 0, 0, 0);
    for(Int_t l=0; l<NumOfLayersCFT - 4; ++l){
      const char* title = NULL;
      title = Form("BGO_CFT_%s", name_Layer[l*2+1] );
      top_dir->Add(createTH2(++target_id, title, // 1 origin
			   NumOfSegCFT[l*2+1], 0, NumOfSegCFT[l*2+1],
			   NumOfSegBGO, 0, NumOfSegBGO,
			   "CFT seg", "BGO seg"));
    }
  }

//  {
//    Int_t target_id = getUniqueID(kCorrelation_catch, 0, 0, 4);
//      const char* title = "BGO_PiID";
//      top_dir->Add(createTH2(++target_id, title, // 1 origin
//			   NumOfSegBGO, 0, NumOfSegBGO,
//			   NumOfSegPiID, 0, NumOfSegPiID,
//			   "BGO seg", "PiID seg"));
//  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createDAQ
// -------------------------------------------------------------------------
TList* HistMaker::createDAQ( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kDAQ);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);


  // DAQ infomation --------------------------------------------------
  {
    // Event builder infomation
    Int_t target_id = getUniqueID(kDAQ, kEB, kHitPat, 0);
    top_dir->Add(createTH1(target_id + 1, "Data size EB", // 1 origin
			   5000, 0, 10000,
			   "Data size [words]", ""));

    // Node information
    target_id = getUniqueID(kDAQ, kVME, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size VME nodes", // 1 origin
			   5, 0, 5,
			   100, 0, 1200,
			   "VME node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kCLite, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size CLite nodes", // 1 origin
			   15, 0, 15,
			   200, 0, 400,
			   "CLite node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kEASIROC, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size EASIROC nodes", // 1 origin
			   100, 0, 100,
			   100, 0, 300,
			   "EASIROC node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kHUL, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size HUL nodes", // 1 origin
			   20, 0, 20,
			   200, 0, 400,
			   "HUL node ID", "Data size [words]"));

//    target_id = getUniqueID(kDAQ, kCAMAC, kHitPat2D, 0);
//    top_dir->Add(createTH2(target_id + 1, "Data size CAMAC nodes", // 1 origin
//			   3, 0, 3,
//			   100, 0, 200,
//			   "CAMAC node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kOpt, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size OptLink nodes", // 1 origin
			   2, 0, 2,
			   500, 0, 1000,
			   "Opt node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kMiscNode, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size Misc nodes", // 1 origin
			   5, 0, 5,
			   100, 0, 200,
			   "Misc node ID", "Data size [words]"));
  }

  // {
  //   // TKO box information
  //   // Declaration of the sub-directory
  //   TString strSubDir  = CONV_STRING(kTKO);
  //   const char* nameSubDir = strSubDir.Data();
  //   TList *sub_dir = new TList;
  //   sub_dir->SetName(nameSubDir);

  //   Int_t target_id = getUniqueID(kDAQ, kTKO, kHitPat2D, 0);
  //   for(Int_t box = 0; box<6; ++box){
  //     const char* title = NULL;
  //     title = Form("TKO box%d", box);
  //     sub_dir->Add(createTH2(target_id + box+1, title, // 1 origin
  // 			     24, 0, 24,
  // 			     40, 0, 40,
  // 			     "TKO MA", "N of decoded hits"));

  //     top_dir->Add(sub_dir);
  //   }
  // }

  return top_dir;
}

//_____________________________________________________________________
TList*
HistMaker::createDCEff( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING( kDCEff );
  // name list of crearted detector
  name_created_detectors_.push_back( strDet );
  if( flag_ps ){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back( strDet );
  }

  return NULL;
}

// -------------------------------------------------------------------------
// Old functions
// -------------------------------------------------------------------------
TList* HistMaker::createBAC_SAC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBAC_SAC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ACs configuration
  const char* name_acs[] = {"BAC0a", "BAC0b", "BAC1", "BAC2", "SAC1", "SAC2"};

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBAC_SAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegBAC_SAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBAC_SAC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBAC_SAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     //			     0x1000, 0, 0x1000,
			     1500, 0, 1500,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

TList* HistMaker::createSFV_SAC3( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSFV_SAC3);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const Int_t NofLoop = 7;
  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSFV_SAC3, 0, kADC, 0);
    for(Int_t i = 0; i<NofLoop; ++i){
      const char* title = NULL;
      if(i<NofLoop-1){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%d", "SFV", nameSubDir, seg);
      }else{
	title = Form("%s_%s", "SAC3", nameSubDir);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kSFV_SAC3, 0, kTDC, 0);
    for(Int_t i = 0; i<NofLoop; ++i){
      const char* title = NULL;
      if(i<NofLoop-1){
	Int_t seg = i+1; // 1 origin
	title = Form("%s_%s_%d", "SFV", nameSubDir, seg);
      }else{
	title = Form("%s_%s", "SAC3", nameSubDir);
      }

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    const char* title = "SFV_SAC3_hit_pattern";
    Int_t target_id = getUniqueID(kSFV_SAC3, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NofLoop, 0, NofLoop,
			   "Segment", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

TList* HistMaker::createGe( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kGe);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kGe, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x2000, 0, 0x2000,
			     "ADC [ch]", ""));
    }

    // Sum histogram
    sub_dir->Add(createTH1(++target_id + NumOfSegGe, "Ge_ADC_Sum", // 1 origin
			   0x2000, 0, 0x2000,
			   "ADC [ch]", ""));

    // Sum histogram (Energy calibrated)
    sub_dir->Add(createTH1(++target_id + NumOfSegGe, "Ge_ADC_Sum_EnergyCalibrated", // 1 origin
			   8000, 0, 8000,
			   "Ge energy [keV]", ""));

    // 2D histogram
    target_id = getUniqueID(kGe, 0, kADC2D, 0);
    sub_dir->Add(createTH2(++target_id, "Ge_ADC_2D", // 1 origin
			   NumOfSegGe, 0, NumOfSegGe,
			   0x2000, 0, 0x2000,
			   "Ge segment", "ADC [ch]"));

    // HitPat
    target_id = getUniqueID(kGe, 0, kHitPat, 0);
    sub_dir->Add(createTH1(++target_id, "Ge_HitPat (using ADC)", // 1 origin
			   NumOfSegGe, 0, NumOfSegGe,
			   "Ge segment", ""));

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // CRM---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kCRM);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kGe, 0, kCRM, 0);
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     2000, 0, 10000,
			     "CRM [ch]", ""));
    }

    // 2D histogram
    target_id = getUniqueID(kGe, 0, kCRM2D, 0);
    sub_dir->Add(createTH2(++target_id, "Ge_CRM_2D", // 1 origin
			   NumOfSegGe, 0, NumOfSegGe,
			   2000, 0, 10000,
			   "Ge segment", "CRM [ch]"));

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TFA---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTFA);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kGe, 0, kTFA, 0);
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     2000, 0, 10000,
			     "TFA [ch]", ""));
    }

    // 2D histogram
    target_id = getUniqueID(kGe, 0, kTFA2D, 0);
    sub_dir->Add(createTH2(++target_id, "Ge_TFA_2D", // 1 origin
			   NumOfSegGe, 0, NumOfSegGe,
			   2000, 0, 10000,
			   "Ge segment", "TFA [ch]"));

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // PUR---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kPUR);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kGe, 0, kPUR, 0);
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     2000, 0, 130000,
			     "PUR [ch]", ""));
    }

    // 2D histogram
    target_id = getUniqueID(kGe, 0, kPUR2D, 0);
    sub_dir->Add(createTH2(++target_id, "Ge_PUR_2D", // 1 origin
			   NumOfSegGe, 0, NumOfSegGe,
			   2000, 0, 130000,
			   "Ge segment", "PUR [ch]"));

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // RST---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kRST);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kGe, 0, kRST, 0);
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     100, 0, 130000,
			     "RST [ch]", ""));
    }

    // 2D histogram
    target_id = getUniqueID(kGe, 0, kRST2D, 0);
    sub_dir->Add(createTH2(++target_id, "Ge_RST_2D", // 1 origin
			   NumOfSegGe, 0, NumOfSegGe,
			   100, 0, 130000,
			   "Ge segment", "RST [ch]"));

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multi -------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kGe, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_MultiCRM_%d", nameDetector, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     16, 0, 16,
			     "Multi CRM [ch]", ""));
    }

    target_id = getUniqueID(kGe, 0, kMulti, 0) + NumOfSegGe;
    for(Int_t i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_MultiTFA_%d", nameDetector, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     16, 0, 16,
			     "Multi TFA [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

TList* HistMaker::createPWO( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kPWO);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPWO, 0, kADC2D, 0);
    for(Int_t i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      Int_t box = i+1; // 1 origin
      title = Form("%s_%s2d_Box%d", nameDetector, nameSubDir, box);
      sub_dir->Add(createTH2(target_id + i+1, title, // 1 origin
			     NumOfUnitPWO[i], 0, NumOfUnitPWO[i],
			     0x1000, 0, 0x1000,
			     "PWO segment", "ADC [ch]"));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // sum hist
    Int_t target_id = getUniqueID(kPWO, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      Int_t box = i+1; // 1 origin
      title = Form("%s_%ssum_Box%d", nameDetector, nameSubDir, box);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  			     0x1000, 0, 0x1000,
  			     "TDC [ch]", ""));
    }

    target_id = getUniqueID(kPWO, 0, kTDC2D, 0);
    for(Int_t i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      Int_t box = i+1; // 1 origin
      title = Form("%s_%s2d_Box%d", nameDetector, nameSubDir, box);
      sub_dir->Add(createTH2(target_id + i+1, title, // 1 origin
  			     NumOfUnitPWO[i], 0, NumOfUnitPWO[i],
  			     0x1000, 0, 0x1000,
  			     "PWO segment", "TDC [ch]"));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPWO, 0, kHitPat, 0);
    for(Int_t i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      Int_t box = i+1; // 1 origin
      title = Form("%s_%s_Box%d", nameDetector, nameSubDir, box);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  			     NumOfUnitPWO[i], 0, NumOfUnitPWO[i],
  			     "PWO segment", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity---------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPWO, 0, kMulti, 0);
    for(Int_t i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      Int_t box = i+1; // 1 origin
      title = Form("%s_%s_Box%d", nameDetector, nameSubDir, box);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  			     NumOfUnitPWO[i], 0, NumOfUnitPWO[i],
  			     "Multiplicity", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

TList* HistMaker::createPWO_E05( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kPWO);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const Int_t box_id = 7; // PBox 8 (E-6)

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPWO, 0, kADC, 0);
    for(Int_t i=0; i<NumOfUnitPWO[box_id]; ++i){
      sub_dir->Add(createTH1(++target_id, Form("%s_%s_%d",
					       nameDetector, nameSubDir, i+1),
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kPWO, 0, kTDC, 0);
    for(Int_t i=0; i<NumOfUnitPWO[box_id]; ++i){
      sub_dir->Add(createTH1(++target_id, Form("%s_%s_%d",
					       nameDetector, nameSubDir, i+1),
			     0x800, 0, 0x800,
			     "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.Data();

    Int_t target_id = getUniqueID(kPWO, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, Form("%s_%s", nameDetector, nameSubDir),
			   NumOfUnitPWO[box_id], 0, NumOfUnitPWO[box_id],
			   "Segment", ""));
  }

  // Multiplicity---------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.Data();

    Int_t target_id = getUniqueID(kPWO, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, Form("%s_%s", nameDetector, nameSubDir),
			   NumOfUnitPWO[box_id]+1, 0, NumOfUnitPWO[box_id]+1,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser

  return top_dir;
}

// -------------------------------------------------------------------------
// createVMEEASIROC
// -------------------------------------------------------------------------
TList* HistMaker::createVMEEASIROC( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kVMEEASIROC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC-2D -------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kVMEEASIROC, 0, kTDC2D  , 0);
    const char* sub_name = "TDC";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersVMEEASIROC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%d", nameDetector, sub_name, i + 1);
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegVMEEASIROC, 0, NumOfSegVMEEASIROC,
			     1024, 0, 1024,
			     "ch", "TDC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC-2D HighGain ----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHighGain_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kVMEEASIROC, 0, kHighGain, 10);
    const char* sub_name = "HighGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersVMEEASIROC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%d", nameDetector, sub_name, i + 1);
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegVMEEASIROC, 0, NumOfSegVMEEASIROC,
			     4096, 0, 4096,
			     "ch", "ADC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC-2D LowGain -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kLowGain_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kVMEEASIROC, 0, kLowGain, 10);
    const char* sub_name = "LowGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersVMEEASIROC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%d", nameDetector, sub_name, i + 1);
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegVMEEASIROC, 0, NumOfSegVMEEASIROC,
			     4096, 0, 4096,
			     "ch", "ADC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createCFT
// -------------------------------------------------------------------------
TList* HistMaker::createCFT( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kCFT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  const char* name_Layer[] = { "UV1" ,"PHI1", "UV2", "PHI2",
                               "UV3" ,"PHI3", "UV4", "PHI4",
                               "UV_ALL" ,"PHI_ALL"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kTDC, 0);
    const char* sub_name = "TDC";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      1024, 0, 1024,
			      "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC-2D -------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kTDC2D  , 0);
    const char* sub_name = "TDC";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_2D", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     1024, 0, 1024,
			     "Fiber", "TDC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kADC, 0);
    const char* sub_name = "TOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      200, -50, 150,
			      "TOT [ch]", ""));
    }

  // CTOT---------------------------------------------------------
    target_id = getUniqueID(kCFT, 0, kADC, 10);
    sub_name = "CTOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      200, -50, 150,
			      "TOT [ch]", ""));

    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT-2D --------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTOT2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kADC2D, 0);
    const char* sub_name = "TOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     200, -50, 150,
			     "Fiber", "TOT [ch]"));
    }

  // CTOT-2D (after cut) --------------------------------------------
    target_id = getUniqueID(kCFT, 0, kADC2D, 10);
    sub_name = "CTOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     200, -50, 150,
			     "Fiber", "TOT [ch]"));

    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC HighGain------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHighGain);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kHighGain, 0);
    const char* sub_name = "HighGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      4096, 0, 4096,
			      "ADC [ch]", ""));
    }

  // ADC HighGain Cut -------------------------------------------
      target_id = getUniqueID(kCFT, 0, kHighGain, 20);
      sub_name = "CHighGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      4096, 0, 4096,
			      "ADC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC Pedestal -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kPedestal);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kPede, 0);
    const char* sub_name = "Pedestal";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      1024, -200, 824,
			      "ADC [ch]", ""));

    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC LowGain ------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kLowGain);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kLowGain , 0);
    const char* sub_name = "LowGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      4096, 0, 4096,
			      "ADC [ch]", ""));
    }

  // ADC LowGain Cut --------------------------------------------
    target_id = getUniqueID(kCFT, 0, kLowGain , 20);
    sub_name = "CLowGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      4096, 0, 4096,
			      "ADC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC-2D HighGain ----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHighGain_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kHighGain, 10);
    const char* sub_name = "HighGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     4096, 0, 4096,
			     "Fiber", "ADC [ch]"));
    }
  // ADC-2D HighGain Cut ------------------------------------------
    target_id = getUniqueID(kCFT, 0, kHighGain, 30);
    sub_name = "CHighGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     4096, 0, 4096,
			     "Fiber", "ADC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC-2D Pedestal ----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kPedestal_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kPede    , 10);
    const char* sub_name = "Pedestal";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     1024,-200, 824,
			     "Fiber", "ADC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC-2D LowGain -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kLowGain_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kLowGain, 10);
    const char* sub_name = "LowGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     4096, 0, 4096,
			     "Fiber", "ADC [ch]"));
    }
  // ADC-2D LowGain Cut -------------------------------------------
    target_id = getUniqueID(kCFT, 0, kLowGain, 30);
    sub_name = "CLowGain";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_2D_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     4096, 0, 4096,
			     "Fiber", "ADC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC HighGain * TOT -----------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHighGainXTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kHighGain, 40);
    const char* sub_name = "HighGainXTOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2( ++target_id, title , // 1 origin
			      4096, 0, 4096,
			      200, -50, 150,
			      "HighGain [ch]", "TOT [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC LowGain * TOT -----------------------------------------
  {
    TString strSubDir  = CONV_STRING(kLowGainXTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kLowGain, 40);
    const char* sub_name = "LowGainXTOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2( ++target_id, title , // 1 origin
			      4096, 0, 4096,
			      200, -50, 150,
			      "LowGain [ch]", "TOT [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }


  // Hit parttern -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHitPattern);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kCFT, 0, kHitPat, 0);
    const char* sub_name = "HitPat";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     "Fiber", ""));

    }
  // Hit parttern (after cut) -----------------------------------
    target_id = getUniqueID(kCFT, 0, kHitPat, 10);
    sub_name = "CHitPat";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     NumOfSegCFT[i], 0, NumOfSegCFT[i],
			     "Fiber", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kMultiplicity);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    //    const char* title = "CFT_multiplicity";
    Int_t target_id = getUniqueID(kCFT, 0, kMulti, 0);
    const char* sub_name = "multiplicity";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT + 2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     30, 0, 30,
			     "Multiplicity", ""));
    }
  // Multiplicity (after cut)------------------------------------
    //    const char* title = "CFT_CMulti";
    target_id = getUniqueID(kCFT, 0, kMulti, 10);
    sub_name = "CMulti";
    // Add to the top directory
    for(Int_t i=0; i<NumOfLayersCFT + 2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     30, 0, 30,
			     "Multiplicity", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBGO
// -------------------------------------------------------------------------
TList* HistMaker::createBGO( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kBGO);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // FlashADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kFADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kBGO, 0, kFADC, 0);
    for(Int_t i = 0; i<NumOfSegBGO; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);

      sub_dir->Add(createTH2(target_id + i+1, title, // 1 origin
			     200, 0, 200,
			     19000, -1000, 18000,
			     "Sampling", "ADC [ch]"));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }
  
  // FlashADCwTDC ----------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kFADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kBGO, 0, kADCwTDC, 0);
    for(Int_t i = 0; i<NumOfSegBGO; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);

      sub_dir->Add(createTH2(target_id + i+1, title, // 1 origin
			     200, 0, 200,
			     19000, -1000, 18000,
			     "Sampling", "ADC [ch]"));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }
  
  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBGO, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBGO; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // FADC TRGTiming & Clk ---------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = "Trigger_Timing";
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    const char* name_Layer[] = { "TrgTiming1", "TrgTiming2", "TrgTiming3", "clk" };

    // Make histogram and add it
    Int_t target_id = getUniqueID(kBGO, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegBGO_T; ++i){
      const char* title = NULL;
      title = Form("%s_%s", nameDetector, name_Layer[i]);
      sub_dir->Add(createTH1(target_id + NumOfSegBGO + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kBGO, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BGO_hit_pattern", // 1 origin
			   NumOfSegBGO, 0, NumOfSegBGO,
			   "Segment", ""));

    top_dir->Add(createTH1(++target_id, "BGO_chit_pattern", // 1 origin
			   NumOfSegBGO, 0, NumOfSegBGO,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kBGO, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "BGO_multiplicity", // 1 origin
			   NumOfSegBGO, 0, NumOfSegBGO,
			   "Multiplicity", ""));

    top_dir->Add(createTH1(++target_id, "BGO_cmultiplicity", // 1 origin
			   NumOfSegBGO, 0, NumOfSegBGO,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createPiID
// -------------------------------------------------------------------------
TList* HistMaker::createPiID( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kPiID);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kPiID, 0, kHighGain, 0);
    for(Int_t i = 0; i<NumOfSegPiID; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_HighGain_%d", nameDetector, seg);

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     0x1000, 0, 0x1000,
			     1000, 0, 2000,
			     "ADC [ch]", ""));
    }

    target_id = getUniqueID(kPiID, 0, kLowGain, 0);
    for(Int_t i = 0; i<NumOfSegPiID; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_LowGain_%d", nameDetector, seg);

      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     0x1000, 0, 0x1000,
			     1000, 0, 2000,
			     "ADC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // ADC w/TDC ---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kADCwTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    Int_t target_id = getUniqueID(kPiID, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfSegPiID; ++i ){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_HighGainwTDC_%d", nameDetector, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     0x1000, 0, 0x1000,
			     1000, 0, 2000,
			     "ADC [ch]", ""));
    }

    target_id = getUniqueID(kPiID, 0, kADCwTDC, NumOfSegPiID);
    for( Int_t i=0; i<NumOfSegPiID; ++i ){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_LowGainwTDC_%d", nameDetector, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     0x1000, 0, 0x1000,
			     1000, 0, 2000,
			     "ADC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    Int_t target_id = getUniqueID(kPiID, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfSegPiID; ++i){
      const char* title = NULL;
      Int_t seg = i+1; // 1 origin
      title = Form("%s_%s_%d", nameDetector, nameSubDir, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
//			     0x1000, 0, 0x1000,
			     1000, 0, 1000,
			     "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kPiID, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "PiID_hit_pattern", // 1 origin
			   NumOfSegPiID, 0, NumOfSegPiID,
			   "Segment", ""));

    top_dir->Add(createTH1(++target_id, "PiID_chit_pattern", // 1 origin
			   NumOfSegPiID, 0, NumOfSegPiID,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    Int_t target_id = getUniqueID(kPiID, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, "PiID_multiplicity", // 1 origin
			   NumOfSegPiID, 0, NumOfSegPiID,
			   "Multiplicity", ""));

    top_dir->Add(createTH1(++target_id, "PiID_cmultiplicity", // 1 origin
			   NumOfSegPiID, 0, NumOfSegPiID,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSFT
// -------------------------------------------------------------------------
TList* HistMaker::createSFT( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kSFT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  const char* name_Layer[] = { "U", "V", "X", "XP" };
  const char* name_Layer_1[] = { "V", "U", "X" };
  //  const Int_t NumOfSegSFT;

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kTDC, 0);
    const char* sub_name = "TDC";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1( ++target_id, title , // 1 origin
			      1024, 0, 1024,
			      "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC (after cut) --------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kCTDC);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kTDC, 10);
    const char* sub_name = "CTDC";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT---------------------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kADC, 0);
    const char* sub_name = "TOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     200, -50, 150,
			     "TOT [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT (after cut) --------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kCTOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kADC, 10);
    const char* sub_name = "CTOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     200, -50, 150,
			     "TOT [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kHitPattern);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kHitPat, 0);
    const char* sub_name = "HitPat";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     NumOfSegSFT[i], 0, NumOfSegSFT[i],
			     "Segment", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Hit parttern (after cut) -----------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kCHitPattern);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kHitPat, 10);
    const char* sub_name = "CHitPat";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     NumOfSegSFT[i], 0, NumOfSegSFT[i],
			     "Segment", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kMultiplicity);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    //    const char* title = "SFT_multiplicity";
    Int_t target_id = getUniqueID(kSFT, 0, kMulti, 0);
    const char* sub_name = "multiplicity";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT-1; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer_1[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     30, 0, 30,
			     "Multiplicity", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity (after cut)------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kCMultiplicity);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    //    const char* title = "SFT_CMulti";
    Int_t target_id = getUniqueID(kSFT, 0, kMulti, 10);
    const char* sub_name = "CMulti";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT-1; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, sub_name, name_Layer_1[i] );
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     30, 0, 30,
			     "Multiplicity", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TDC-2D (after cut) --------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTDC_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kTDC2D, 0);
    const char* sub_name = "CTDC";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_2D", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegSFT[i], 0, NumOfSegSFT[i],
			     1024, 0, 1024,
			     "Segment", "TDC [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // TOT-2D (after cut) --------------------------------------------
  {
    // Declaration of the sub-directory
    TString strSubDir  = CONV_STRING(kTOT_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSFT, 0, kADC2D, 0);
    const char* sub_name = "CTOT";
    // Add to the top directory
    for(Int_t i=0; i<NumOfPlaneSFT; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s_2D", nameDetector, sub_name, name_Layer[i] );
      sub_dir->Add(createTH2(++target_id, title, // 1 origin
			     NumOfSegSFT[i], 0, NumOfSegSFT[i],
			     200, -50, 150,
			     "Segment", "TOT [ch]"));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }
  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSAC_for_E40
// -------------------------------------------------------------------------
TList* HistMaker::createSAC( Bool_t flag_ps )
{
  TString strDet = CONV_STRING(kSAC);       // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab

  const char* nameDetector = strDet.Data(); // Declaration of the directory
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  const char* Room[]={"Room1","Room2","Room3","Room4"};


  // ADC ---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName( nameSubDir );

    Int_t target_id = getUniqueID(kSAC, 0, kADC, 0);
    for(Int_t i = 0; i<NumOfRoomsSAC; ++i){
      const char* title = NULL;
      title = Form("SAC_%s_%s", Room[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // ADC w/TDC ---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kADCwTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName( nameSubDir );

    Int_t target_id = getUniqueID(kSAC, 0, kADCwTDC, 0);
    for( Int_t i=0; i<NumOfRoomsSAC; ++i ){
      const char* title = NULL;
      title = Form("SAC_%s_%s", Room[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  // TDC---------------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    Int_t target_id = getUniqueID(kSAC, 0, kTDC, 0);
    for(Int_t i = 0; i<NumOfRoomsSAC; ++i){
      const char* title = NULL;
      title = Form("SAC_%s_%s", Room[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    Int_t target_id = getUniqueID(kSAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "SAC_hit_pattern",
			   NumOfRoomsSAC, 0, NumOfRoomsSAC,
			   "Room", ""));
  }

  { // Multiplicity -----------------------------------------------
    Int_t target_id = getUniqueID(kSAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "SAC_multiplicity",
			   NumOfRoomsSAC+1, 0, NumOfRoomsSAC+1,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}



// -------------------------------------------------------------------------
// createFBT1
// -------------------------------------------------------------------------
TList* HistMaker::createFBT1( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kFBT1);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }
  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  const char* UD[] ={"U","D"};

  //  // TDC---------------------------------------------------------
  //  {
  //    TString strSubDir  = CONV_STRING(kTDC);
  //    const char* nameSubDir = strSubDir.Data();
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //    for(Int_t l=0; l<NumOfLayersFBT; ++l){
  //       for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
  //       Int_t target_id = getUniqueID(kFBT1, l, kTDC, v*FBTOffset);
  //          for(Int_t i=0; i<NumOfSegFBT1; ++i){
  //	        sub_dir->Add(createTH1(++target_id , Form("%s_Layer%d_%s_%d%s",
  //			       nameDetector,l+1, nameSubDir, i+1 ,UD[v]),   //1 origine
  //			       1024, 0, 1024,
  //			       "TDC [ch]", ""));
  //          }
  //       }
  //    }
  //    top_dir->Add(sub_dir);
  //  }
  //  // TOT---------------------------------------------------------
  //  {
  //    const char* nameSubDir = "TOT";
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //    for(Int_t l=0; l<NumOfLayersFBT; ++l){
  //    for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
  //       Int_t target_id = getUniqueID(kFBT1, l, kADC, v*FBTOffset);
  //    for(Int_t i=0; i<NumOfSegFBT1; ++i){
  //	        sub_dir->Add(createTH1(++target_id , Form("%s_Layer%d_%s_%d%s",
  //			       nameDetector,l+1, nameSubDir, i+1 ,UD[v]),   //1 origine
  //			       200, -50, 150,
  //			       "TOT [ch]", ""));
  //    }
  //    }
  //    }
  //    top_dir->Add(sub_dir);
  //  }
  // TDC/TOT SUM -----------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC_TOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
	// TDC
	sub_dir->Add(createTH1(getUniqueID(kFBT1, l, kTDC, NumOfSegFBT1+1+ v*FBTOffset) ,
			       Form("%s_Layer%d_TDC_%s", nameDetector, l+1, UD[v]),
			       1024, 0, 1024,
			       "TDC [ch]", ""));
	// TOT
	sub_dir->Add(createTH1(getUniqueID(kFBT1, l, kADC, NumOfSegFBT1+1+ v*FBTOffset) ,
			       Form("%s_Layer%d_TOT_%s", nameDetector, l+1, UD[v]),
			       200, -50, 150,
			       "TOT [ch]", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TDC-2D --------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT1, l, kTDC2D, v*FBTOffset);
        sub_dir->Add(createTH2(++target_id,
                               Form("FBT1_Layer%d_TDC2D_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT1, 0, NumOfSegFBT1,
			       1024, 0, 1024,
			       "Segment", "TDC [ch]"));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TOT-2D --------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTOT_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT1, l, kADC2D, v*FBTOffset);
        sub_dir->Add(createTH2(++target_id,
			       Form("FBT1_Layer%d_TOT2D_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT1, 0, NumOfSegFBT1,
			       200, -50, 150,
			       "Segment", "TOT [ch]"));
      }
    }
    top_dir->Add(sub_dir);
  }
  // Hit parttern --------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHitParttern);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT1, l, kHitPat, v*FBTOffset);
        sub_dir->Add(createTH1(++target_id,
                               Form("FBT1_Layer%d_HitPat_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT1, 0, NumOfSegFBT1,
			       "Segment", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // Multiplicity -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kMultiplicity);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT1, l, kMulti, v*FBTOffset);
        sub_dir->Add(createTH1(++target_id,
                               Form("FBT1_Layer%d_multiplicity_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT1, 0, NumOfSegFBT1,
			       "Multiplicity", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  return top_dir;
}


// -------------------------------------------------------------------------
// createFBT2
// -------------------------------------------------------------------------
TList* HistMaker::createFBT2( Bool_t flag_ps )
{
  // Determine the detector name
  TString strDet = CONV_STRING(kFBT2);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }
  // Declaration of the directory
  // Just type conversion from TString to char*
  const char* nameDetector = strDet.Data();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  const char* UD[] ={"U","D"};

  //  // TDC---------------------------------------------------------
  //  {
  //    TString strSubDir  = CONV_STRING(kTDC);
  //    const char* nameSubDir = strSubDir.Data();
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //    for(Int_t l=0; l<NumOfLayersFBT; ++l){
  //       for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
  //       Int_t target_id = getUniqueID(kFBT2, l, kTDC, v*FBTOffset);
  //          for(Int_t i=0; i<NumOfSegFBT2; ++i){
  //	        sub_dir->Add(createTH1(++target_id , Form("%s_Layer%d_%s_%d%s",
  //			       nameDetector,l+1, nameSubDir, i+1 ,UD[v]),   //1 origine
  //			       1024, 0, 1024,
  //			       "TDC [ch]", ""));
  //          }
  //       }
  //    }
  //    top_dir->Add(sub_dir);
  //  }
  //  // TOT---------------------------------------------------------
  //  {
  //    const char* nameSubDir = "TOT";
  //    TList *sub_dir = new TList;
  //    sub_dir->SetName(nameSubDir);
  //    for(Int_t l=0; l<NumOfLayersFBT; ++l){
  //    for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
  //       Int_t target_id = getUniqueID(kFBT2, l, kADC, v*FBTOffset);
  //    for(Int_t i=0; i<NumOfSegFBT2; ++i){
  //	        sub_dir->Add(createTH1(++target_id , Form("%s_Layer%d_%s_%d%s",
  //			       nameDetector,l+1, nameSubDir, i+1 ,UD[v]),   //1 origine
  //			       200, -50, 150,
  //			       "TOT [ch]", ""));
  //    }
  //    }
  //    }
  //    top_dir->Add(sub_dir);
  //  }
  // TDC/TOT SUM -----------------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC_TOT);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        // TDC
        sub_dir->Add(createTH1(getUniqueID(kFBT2, l, kTDC, NumOfSegFBT2+1+ v*FBTOffset) ,
			       Form("%s_Layer%d_TDC_%s", nameDetector, l+1, UD[v]),
			       1024, 0, 1024,
			       "TDC [ch]", ""));
        // TOT
        sub_dir->Add(createTH1(getUniqueID(kFBT2, l, kADC, NumOfSegFBT2+1+ v*FBTOffset) ,
			       Form("%s_Layer%d_TOT_%s", nameDetector, l+1, UD[v]),
			       200, -50, 150,
			       "TOT [ch]", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TDC-2D --------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTDC_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT2, l, kTDC2D, v*FBTOffset);
        sub_dir->Add(createTH2(++target_id,
                               Form("FBT2_Layer%d_TDC2D_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT2, 0, NumOfSegFBT2,
			       1024, 0, 1024,
			       "Segment", "TDC [ch]"));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TOT-2D --------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kTOT_2D);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT2, l, kADC2D, v*FBTOffset);
        sub_dir->Add(createTH2(++target_id,
			       Form("FBT2_Layer%d_TOT2D_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT2, 0, NumOfSegFBT2,
			       200, -50, 150,
			       "Segment", "TOT [ch]"));
      }
    }
    top_dir->Add(sub_dir);
  }
  // Hit parttern --------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kHitParttern);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT2, l, kHitPat, v*FBTOffset);
        sub_dir->Add(createTH1(++target_id,
                               Form("FBT2_Layer%d_HitPat_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT2, 0, NumOfSegFBT2,
			       "Segment", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // Multiplicity -----------------------------------------------
  {
    TString strSubDir  = CONV_STRING(kMultiplicity);
    const char* nameSubDir = strSubDir.Data();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    for(Int_t l=0; l<NumOfLayersFBT; ++l){
      for(Int_t v=0; v<NumOfUDStructureFBT; ++v){
        Int_t target_id = getUniqueID(kFBT2, l, kMulti, v*FBTOffset);
        sub_dir->Add(createTH1(++target_id,
                               Form("FBT2_Layer%d_multiplicity_%s",l+1, UD[v]), // 1 origin
			       NumOfSegFBT2, 0, NumOfSegFBT2,
			       "Multiplicity", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  return top_dir;
}
