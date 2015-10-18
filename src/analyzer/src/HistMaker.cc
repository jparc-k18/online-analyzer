#include<iostream>
#include<cstdlib>
#include<string>
#include<algorithm>

#include"DetectorID.hh"
#include"HistHelper.hh"
#include"HistMaker.hh"

#include<TH1.h>
#include<TH2.h>
#include<TList.h>
#include<TDirectory.h>
#include<TString.h>

ClassImp(HistMaker)

// getStr_FromEnum ----------------------------------------------------------
// The method to get std::string from enum value 
#define CONV_STRING(x) getStr_FromEnum(#x)
std::string getStr_FromEnum(const char* c){
  std::string str = c;
  return str.substr(1);
}

static const std::string MyName = "HistMaker::";

// Constructor -------------------------------------------------------------
HistMaker::HistMaker():
  current_hist_id_(0)
{

}

// Destructor --------------------------------------------------------------
HistMaker::~HistMaker()
{

}

// -------------------------------------------------------------------------
// getListOfDetectors
// -------------------------------------------------------------------------
void HistMaker::getListOfDetectors(std::vector<std::string>& vec)
{
  HistMaker& g = HistMaker::getInstance();
  std::copy(g.name_created_detectors_.begin(),
	    g.name_created_detectors_.end(),
	    back_inserter(vec)
	    );
  
}

// -------------------------------------------------------------------------
// getListOfPsFiles
// -------------------------------------------------------------------------
void HistMaker::getListOfPsFiles(std::vector<std::string>& vec)
{
  HistMaker& g = HistMaker::getInstance();
  std::copy(g.name_ps_files_.begin(),
	    g.name_ps_files_.end(),
	    back_inserter(vec)
	    );
  
}

// -------------------------------------------------------------------------
// setHistPtr
// -------------------------------------------------------------------------
int HistMaker::setHistPtr(std::vector<TH1*>& vec)
{
  static const std::string MyFunc = "setHistPtr ";
  
  vec.resize(current_hist_id_);
  for(int i = 0; i<current_hist_id_; ++i){
    int unique_id = getUniqueID(i);
    vec[i] = GHist::get(unique_id);
    if(vec[i] == NULL){
      std::cerr << "#E: " << MyName << MyFunc
		<< "Pointer is NULL\n"
		<< " Unique ID    : " << unique_id << "\n"
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
TH1* HistMaker::createTH1(int unique_id, const char* title,
			  int nbinx, int xmin, int xmax,
			  const char* xtitle, const char* ytitle
			  )
{
  static const std::string MyFunc = "createTH1 ";

  // Add information to dictionaries which will be used to find the histogram
  int sequential_id = current_hist_id_++;
  idmap_seq_from_unique_[unique_id]     = sequential_id;
  idmap_seq_from_name_[title]           = sequential_id;
  idmap_unique_from_seq_[sequential_id] = unique_id;

  // create histogram using the static method of HistHelper class
  TH1 *h = GHist::I1(unique_id, title, nbinx, xmin, xmax);
  if(!h){
    std::cerr << "#E: " << MyName << MyFunc
	      << "Fail to create TH1"
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
TH2* HistMaker::createTH2(int unique_id, const char* title,
			  int nbinx, int xmin, int xmax,
			  int nbiny, int ymin, int ymax,
			  const char* xtitle, const char* ytitle
			  )
{
  static const std::string MyFunc = "createTH2 ";

  // Add information to dictionaries which will be used to find the histogram
  int sequential_id = current_hist_id_++;
  idmap_seq_from_unique_[unique_id]     = sequential_id;
  idmap_seq_from_name_[title]           = sequential_id;
  idmap_unique_from_seq_[sequential_id] = unique_id;

  // create histogram using the static method of HistHelper class
  TH2 *h = GHist::I2(unique_id, title,
		  nbinx, xmin, xmax,
		  nbiny, ymin, ymax);
  if(!h){
    std::cerr << "#E: " << MyName << MyFunc
	      << "Fail to create TH2"
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
// createBH1 
// -------------------------------------------------------------------------
TList* HistMaker::createBH1(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBH1);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // Make unique ID
    int target_id = getUniqueID(kBH1, 0, kADC, 0);
    for(int i = 0; i<NumOfSegBH1*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH1){
	int seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegBH1; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBH1, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegBH1*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH1){
	int seg = i+1; // 1 origin 
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegBH1; // 1 origin 
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
    const char* title = "BH1_hit_pattern";
    int target_id = getUniqueID(kBH1, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "BH1_multiplicity";
    int target_id = getUniqueID(kBH1, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   "Multiplicity", ""));
  }
  
  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createBFT
// -------------------------------------------------------------------------
TList* HistMaker::createBFT(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBFT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    int target_id = getUniqueID(kBFT, 0, kTDC, 0);
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
    int target_id = getUniqueID(kBFT, 0, kTDC, 10);
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
    int target_id = getUniqueID(kBFT, 0, kADC, 0);
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
    int target_id = getUniqueID(kBFT, 0, kADC, 10);
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
    int target_id = getUniqueID(kBFT, 0, kHitPat, 0);
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
    int target_id = getUniqueID(kBFT, 0, kHitPat, 10);
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
    int target_id = getUniqueID(kBFT, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   30, 0, 30,
			   "Multiplicity", ""));
  }

  // Multiplicity (after cut)------------------------------------
  {
    const char* title = "BFT_CMulti";
    int target_id = getUniqueID(kBFT, 0, kMulti, 10);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   30, 0, 30,
			   "Multiplicity", ""));
  }
  
  // TDC-2D (after cut) --------------------------------------------
  {
    int target_id = getUniqueID(kBFT, 0, kTDC2D, 0);
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
    int target_id = getUniqueID(kBFT, 0, kADC2D, 0);
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
TList* HistMaker::createBC3(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBC3);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"x0", "x1", "v0", "v1", "u0", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBC3, 0, kTDC, 0);
    for(int i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kBC3, 0, kTDC2D, 0);
    for(int i = 0; i<NumOfLayersBC3; ++i){
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBC3, 0, kHitPat, 0);
    for(int i = 0; i<NumOfLayersBC3; ++i){
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    int target_id = getUniqueID(kBC3, 0, kMulti, 0);
    for(int i = 0; i<NumOfLayersBC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kBC3, 0, kMulti, NumOfLayersBC3);
    for(int i = 0; i<NumOfLayersBC3; ++i){
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
TList* HistMaker::createBC4(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBC4);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"u0", "u1", "v0", "v1", "x0", "x1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBC4, 0, kTDC, 0);
    for(int i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kBC4, 0, kTDC2D, 0);
    for(int i = 0; i<NumOfLayersBC4; ++i){
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBC4, 0, kHitPat, 0);
    for(int i = 0; i<NumOfLayersBC4; ++i){
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    int target_id = getUniqueID(kBC4, 0, kMulti, 0);
    for(int i = 0; i<NumOfLayersBC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kBC4, 0, kMulti, NumOfLayersBC4);
    for(int i = 0; i<NumOfLayersBC4; ++i){
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
TList* HistMaker::createBMW(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBMW);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    int target_id = getUniqueID(kBMW, 0, kADC, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, "BMW_ADC", // 1 origin
			   0x1000, 0, 0x1000,
			   "ADC [ch]", ""));
  }

  // TDC---------------------------------------------------------
  {
    int target_id = getUniqueID(kBMW, 0, kTDC, 0);
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
TList* HistMaker::createBH2(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBH2);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBH2, 0, kADC, 0);
    for(int i = 0; i<NumOfSegBH2; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBH2, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegBH2; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin
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
    const char* title = "BH2_hit_pattern";
    int target_id = getUniqueID(kBH2, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegBH2, 0, NumOfSegBH2,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "BH2_multiplicity";
    int target_id = getUniqueID(kBH2, 0, kMulti, 0);
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
TList* HistMaker::createBH2_E07(bool flag_ps)
{
  std::string strDet = CONV_STRING(kBH2_E07);   // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab
  
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  { // ADC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    
    int target_id = getUniqueID(kBH2_E07, 0, kADC, 0);
    for(int i = 0; i<NumOfSegBH2_E07*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH2_E07){
	int seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegBH2_E07; // 1 origin
	title = Form("%s_%s_%dD", nameDetector, nameSubDir, seg);
      }
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kBH2_E07, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegBH2_E07*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegBH2_E07){
	int seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegBH2_E07; // 1 origin
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
    int target_id = getUniqueID(kBH2_E07, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   NumOfSegBH2_E07, 0, NumOfSegBH2_E07,
			   "Segment", ""));
  }
  
  { // Multiplicity -----------------------------------------------
    const char* title = "BH2_E07_multiplicity";
    int target_id = getUniqueID(kBH2_E07, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   NumOfSegBH2_E07+1, 0, NumOfSegBH2_E07+1,
			   "Multiplicity", ""));
  }
  
  return top_dir;
}

// -------------------------------------------------------------------------
// createBAC
// -------------------------------------------------------------------------
TList* HistMaker::createBAC(bool flag_ps)
{
  std::string strDet = CONV_STRING(kBAC);       // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab
  
  const char* nameDetector = strDet.c_str(); // Declaration of the directory
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[] = { "BAC1", "BAC2" };

  { // ADC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kBAC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegBAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kBAC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegBAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }
  
  { // Hit parttern -----------------------------------------------
    int target_id = getUniqueID(kBAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "BAC_hit_pattern",
			   NumOfSegBAC, 0, NumOfSegBAC,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    int target_id = getUniqueID(kBAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "BAC_multiplicity",
			   NumOfSegBAC+1, 0, NumOfSegBAC+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createBAC_E07
// -------------------------------------------------------------------------
TList* HistMaker::createBAC_E07(bool flag_ps)
{
  std::string strDet = CONV_STRING(kBAC_E07);   // Determine the detector name
  name_created_detectors_.push_back(strDet);    // name list of crearted detector
  if(flag_ps) name_ps_files_.push_back(strDet); // name list which are displayed in Ps tab
  
  const char* nameDetector = strDet.c_str(); // Declaration of the directory
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegBAC_E07] = {
    "BAC1-SUM", "BAC2-SUM", "BAC1-1", "BAC1-2", "BAC2-1", "BAC2-2"
  };
  
  { // ADC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kBAC_E07, 0, kADC, 0);
    for(int i = 0; i<NumOfSegBAC_E07; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1( ++target_id, title, // 1 origin
			      0x1000, 0, 0x1000,
			      "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }
  
  { // TDC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC); // Declaration of the sub-directory
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kBAC_E07, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegBAC_E07; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }
  
  { // Hit parttern -----------------------------------------------
    int target_id = getUniqueID(kBAC_E07, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "BAC_E07_hit_pattern",
			   NumOfSegBAC_E07, 0, NumOfSegBAC_E07,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    int target_id = getUniqueID(kBAC_E07, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "BAC_E07_multiplicity",
			   NumOfSegBAC_E07+1, 0, NumOfSegBAC_E07+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createFBH
// -------------------------------------------------------------------------
TList* HistMaker::createFBH(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kFBH);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  
  // TDC---------------------------------------------------------
  {
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kFBH, 0, kTDC, 0);
    for(int i=0; i<NumOfSegFBH*2; ++i){
      if(i<NumOfSegFBH){
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dU", // 1 origin
						 nameDetector, nameSubDir, i+1),
			       1024, 0, 1024,
			       "TDC [ch]", ""));
      }else{
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dD", // 1 origin
						 nameDetector, nameSubDir, i+1),
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
    int target_id = getUniqueID(kFBH, 0, kADC, 0);
    for(int i=0; i<NumOfSegFBH*2; ++i){
      if(i<NumOfSegFBH){
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dU", // 1 origin
						 nameDetector, nameSubDir, i+1),
			       200, -50, 150,
			       "TOT [ch]", ""));
      }else{
	sub_dir->Add(createTH1(++target_id, Form("%s_%s_%dD", // 1 origin
						 nameDetector, nameSubDir, i+1),
			       200, -50, 150,
			       "TOT [ch]", ""));
      }
    }
    top_dir->Add(sub_dir);
  }
  // TDC-2D --------------------------------------------
  {
    int target_id = getUniqueID(kFBH, 0, kTDC2D, 0);
    top_dir->Add(createTH2(++target_id, "FBH_TDC_2D", // 1 origin
			   NumOfSegFBH*2, 0, NumOfSegFBH*2,
			   1024, 0, 1024,
			   "Segment", "TDC [ch]"));
  }
  // TOT-2D --------------------------------------------
  {
    int target_id = getUniqueID(kFBH, 0, kADC2D, 0);
    top_dir->Add(createTH2(++target_id, "FBH_TOT_2D", // 1 origin
			   NumOfSegFBH*2, 0, NumOfSegFBH*2,
			   200, -50, 150,
			   "Segment", "TOT [ch]"));
  }
  // Hit parttern 2D --------------------------------------------
  {
    int target_id = getUniqueID(kFBH, 0, kHitPat2D, 0);
    top_dir->Add(createTH2(++target_id, "FBH_HitPat", // 1 origin
			   NumOfSegFBH, 0, NumOfSegFBH, NumOfSegFBH, 0, NumOfSegFBH,
			   "Segment", ""));
  }
  // Multiplicity -----------------------------------------------
  {
    const char* title = "FBH_multiplicity";
    int target_id = getUniqueID(kFBH, 0, kMulti2D, 0);
    top_dir->Add(createTH2(++target_id, title, // 1 origin
			   NumOfSegFBH, 0, NumOfSegFBH, NumOfSegFBH, 0, NumOfSegFBH,
			   "Multiplicity", ""));
  }
  return top_dir;
}

// -------------------------------------------------------------------------
// createSSDT
// -------------------------------------------------------------------------
TList* HistMaker::createSSDT(bool flag_ps)
{
  std::string strDet = CONV_STRING(kSSDT);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  { // TDC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSDT, 0, kTDC, 0);
    for(int seg=0; seg<NumOfSegSSDT; ++seg){
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
TList* HistMaker::createSSD0(bool flag_ps)
{
  std::string strDet = CONV_STRING(kSSD0);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* nameLayer[NumOfLayersSSD0] = { "x0", "y0" };

  { // ADC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD0, 0, kADC2D, 0);
    for(int l=0; l<NumOfLayersSSD0; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0, 0, NumOfSegSSD0, 0x3ff, 0, 0x3ff,
			     "Segment", "Peak Height  "));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD0, 0, kTDC2D, 0);
    for(int l=0; l<NumOfLayersSSD0; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD0, 0, NumOfSegSSD0, NumOfSamplesSSD+1, 0, NumOfSamplesSSD+1,
			     "Segment", "Peak Position  "));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD0, 0, kHitPat, 0);
    for(int l=0; l<NumOfLayersSSD0; ++l){
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD0, 0, kMulti, 0);
    for(int l=0; l<NumOfLayersSSD0; ++l){
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
TList* HistMaker::createSSD1(bool flag_ps)
{
  std::string strDet = CONV_STRING(kSSD1);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* nameLayer[NumOfLayersSSD1] = { "y0", "x0", "y1", "x1" };

  { // ADC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD1, 0, kADC2D, 0);
    for(int l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1, 0, NumOfSegSSD1, 0x3ff, 0, 0x3ff,
			     "Segment", "Peak Height  "));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD1, 0, kTDC2D, 0);
    for(int l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1, 0, NumOfSegSSD1, NumOfSamplesSSD+1, 0, NumOfSamplesSSD+1,
			     "Segment", "Peak Position  "));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD1, 0, kHitPat, 0);
    for(int l=0; l<NumOfLayersSSD1; ++l){
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
			   NumOfSegSSD1/6, 0, NumOfSegSSD1, NumOfSegSSD1/6, 0, NumOfSegSSD1,
			   nameLayer[1], nameLayer[0]));
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[2], nameLayer[3]),
			   NumOfSegSSD1/6, 0, NumOfSegSSD1, NumOfSegSSD1/6, 0, NumOfSegSSD1,
			   nameLayer[3], nameLayer[2]));
    top_dir->Add(sub_dir);
  }

  { // Multiplicity -----------------------------------------------
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD1, 0, kMulti, 0);
    for(int l=0; l<NumOfLayersSSD1; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1+1, 0, NumOfSegSSD1+1,
			     "Multiplicity", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD1+1, 0, NumOfSegSSD1+1,
			     "CMultiplicity", ""));
    }
    top_dir->Add(sub_dir);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSSD2
// -------------------------------------------------------------------------
TList* HistMaker::createSSD2(bool flag_ps)
{
  std::string strDet = CONV_STRING(kSSD2);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* nameLayer[NumOfLayersSSD2] = { "x0", "y0", "x1", "y1" };

  { // ADC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD2, 0, kADC2D, 0);
    for(int l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2, 0, NumOfSegSSD2, 0x3ff, 0, 0x3ff,
			     "Segment", "Peak Height  "));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC --------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD2, 0, kTDC2D, 0);
    for(int l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH2(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2, 0, NumOfSegSSD2, NumOfSamplesSSD+1, 0, NumOfSamplesSSD+1,
			     "Segment", "Peak Position  "));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD2, 0, kHitPat, 0);
    for(int l=0; l<NumOfLayersSSD2; ++l){
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
			   NumOfSegSSD2/6, 0, NumOfSegSSD2, NumOfSegSSD2/6, 0, NumOfSegSSD2,
			   nameLayer[0], nameLayer[1]));
    sub_dir->Add(createTH2(++target_id, Form("%s_%s_%s%%%s", nameDetector, nameSubDir,
					     nameLayer[3], nameLayer[2]),
			   NumOfSegSSD2/6, 0, NumOfSegSSD2, NumOfSegSSD2/6, 0, NumOfSegSSD2,
			   nameLayer[2], nameLayer[3]));
    top_dir->Add(sub_dir);
  }

  { // Multiplicity -----------------------------------------------
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSSD2, 0, kMulti, 0);
    for(int l=0; l<NumOfLayersSSD2; ++l){
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2+1, 0, NumOfSegSSD2+1,
			     "Multiplicity", ""));
      sub_dir->Add(createTH1(++target_id,
			     Form("%s_C%s_%s", nameDetector, nameSubDir, nameLayer[l]),
			     NumOfSegSSD2+1, 0, NumOfSegSSD2+1,
			     "CMultiplicity", ""));
    }
    top_dir->Add(sub_dir);
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createPVAC
// -------------------------------------------------------------------------
TList* HistMaker::createPVAC(bool flag_ps)
{
  std::string strDet = CONV_STRING(kPVAC);
  name_created_detectors_.push_back(strDet); 
  if(flag_ps) name_ps_files_.push_back(strDet); 
  
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegPVAC] = {
    "PVAC-SUM", "PVAC-1", "PVAC-2", "PVAC-3", "PVAC-4",
  };

  { // ADC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kPVAC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegPVAC; ++i){
      sub_dir->Add(createTH1( ++target_id,
			      Form("%s_%s", name_acs[i], nameSubDir),
			      0x1000, 0, 0x1000,
			      "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kPVAC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegPVAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }
  
  { // Hit parttern -----------------------------------------------
    int target_id = getUniqueID(kPVAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "PVAC_hit_pattern",
			   NumOfSegPVAC, 0, NumOfSegPVAC,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    int target_id = getUniqueID(kPVAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "PVAC_multiplicity",
			   NumOfSegPVAC+1, 0, NumOfSegPVAC+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createFAC
// -------------------------------------------------------------------------
TList* HistMaker::createFAC(bool flag_ps)
{
  std::string strDet = CONV_STRING(kFAC);
  name_created_detectors_.push_back(strDet); 
  if(flag_ps) name_ps_files_.push_back(strDet); 

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegFAC] = {
    "FAC-SUM", "FAC-1", "FAC-2", "FAC-3", "FAC-4", "FAC-5", "FAC-6"
  };

  { // ADC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kFAC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegFAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kFAC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegFAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    int target_id = getUniqueID(kFAC, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "FAC_hit_pattern",
			   NumOfSegFAC, 0, NumOfSegFAC,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    int target_id = getUniqueID(kFAC, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "FAC_multiplicity",
			   NumOfSegFAC+1, 0, NumOfSegFAC+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSAC1
// -------------------------------------------------------------------------
TList* HistMaker::createSAC1(bool flag_ps)
{
  std::string strDet = CONV_STRING(kSAC1);
  name_created_detectors_.push_back(strDet); 
  if(flag_ps) name_ps_files_.push_back(strDet); 

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_acs[NumOfSegSAC1] = { "SAC1" };

  { // ADC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kSAC1, 0, kADC, 0);
    for(int i = 0; i<NumOfSegSAC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(++target_id, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "ADC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // TDC---------------------------------------------------------
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kSAC1, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegSAC1; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    top_dir->Add(sub_dir);
  }

  { // Hit parttern -----------------------------------------------
    int target_id = getUniqueID(kSAC1, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "SAC1_hit_pattern",
			   NumOfSegSAC1, 0, NumOfSegSAC1,
			   "Segment", ""));
  }

  { // Multiplicity -----------------------------------------------
    int target_id = getUniqueID(kSAC1, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, "SAC1_multiplicity",
			   NumOfSegSAC1+1, 0, NumOfSegSAC1+1,
			   "Multiplicity", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createSCH
// -------------------------------------------------------------------------
TList* HistMaker::createSCH(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kSCH);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);
  
  // TDC---------------------------------------------------------
  {
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    int target_id = getUniqueID(kSCH, 0, kTDC, 0);
    for(int i=0; i<NumOfSegSCH; ++i){
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
    int target_id = getUniqueID(kSCH, 0, kADC, 0);
    for(int i=0; i<NumOfSegSCH; ++i){
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
    int target_id = getUniqueID(kSCH, 0, kTDC2D, 0);
    top_dir->Add(createTH2(++target_id, "SCH_TDC_2D", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   1024, 0, 1024,
			   "Segment", "TDC [ch]"));
  }
  // TOT-2D --------------------------------------------
  {
    int target_id = getUniqueID(kSCH, 0, kADC2D, 0);
    top_dir->Add(createTH2(++target_id, "SCH_TOT_2D", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   200, -50, 150,
			   "Segment", "TOT [ch]"));
  }
  // Hit parttern -----------------------------------------------
  {
    int target_id = getUniqueID(kSCH, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, "SCH_HitPat", // 1 origin
			   NumOfSegSCH, 0, NumOfSegSCH,
			   "Segment", ""));
  }
  // Multiplicity -----------------------------------------------
  {
    const char* title = "SCH_multiplicity";
    int target_id = getUniqueID(kSCH, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   20, 0, 20,
			   "Multiplicity", ""));
  }
  return top_dir;
}

// -------------------------------------------------------------------------
// createKFAC
// -------------------------------------------------------------------------
TList* HistMaker::createKFAC(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kKFAC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kKFAC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegKFAC; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin
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
TList* HistMaker::createEMC(bool flag_ps)
{
  std::string strDet = CONV_STRING(kEMC);
  name_created_detectors_.push_back(strDet);
  if(flag_ps) name_ps_files_.push_back(strDet);

  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  { // XY ---------------------------------------------------------
    //int serial_id = getUniqueID(kEMC, 0, kSerial, 0);
    int xpos_id   = getUniqueID(kEMC, 0, kXpos,   0);
    int ypos_id   = getUniqueID(kEMC, 0, kYpos,   0);
    int xypos_id  = getUniqueID(kEMC, 0, kXYpos,  0);
    //int time_id   = getUniqueID(kEMC, 0, kTime,   0);

    for(int i = 0; i<NumOfSegEMC; ++i){
      top_dir->Add(createTH1( ++xpos_id,
			      Form("%s_Xpos", strDet.c_str()),
			      0x100, -0x100, 0x100,
			      "Xpos [mm]", ""));
      top_dir->Add(createTH1( ++ypos_id,
			      Form("%s_Ypos", strDet.c_str()),
			      0x100, -0x100, 0x100,
			      "Ypos [mm]", ""));
      top_dir->Add(createTH2( ++xypos_id,
			      Form("%s_XYpos", strDet.c_str()),
			      0x100, -0x100, 0x100, 0x100, -0x100, 0x100,
			      "Xpos [mm]", "Ypos [mm]"));
    }
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createKIC
// -------------------------------------------------------------------------
TList* HistMaker::createKIC(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kKIC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const char* name_kics[] = { "U", "D", "L", "R" };

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kKIC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegKIC; ++i){
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kKIC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegKIC; ++i){
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
    int target_id = getUniqueID(kKIC, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegKIC, 0, NumOfSegKIC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "KIC_multiplicity";
    int target_id = getUniqueID(kKIC, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegKIC+1, 0, NumOfSegKIC+1,
			   "Multiplicity", ""));
  }
  
  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createSDC2
// -------------------------------------------------------------------------
TList* HistMaker::createSDC2(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kSDC2);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"x0", "x1", "v0", "v1", "u0", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSDC2, 0, kTDC, 0);
    for(int i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kSDC2, 0, kTDC2D, 0);
    for(int i = 0; i<NumOfLayersSDC2; ++i){
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSDC2, 0, kHitPat, 0);
    for(int i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    int target_id = getUniqueID(kSDC2, 0, kMulti, 0);
    for(int i = 0; i<NumOfLayersSDC2; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC2, 0, kMulti, NumOfLayersSDC2);
    for(int i = 0; i<NumOfLayersSDC2; ++i){
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
// createHDC
// -------------------------------------------------------------------------
TList* HistMaker::createHDC(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kHDC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"u0", "u1", "x0", "x1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kHDC, 0, kTDC, 0);
    for(int i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     1024, 0, 1024,
			     "TDC [ch]", ""));
    }
    target_id = getUniqueID(kHDC, 0, kTDC2D, 0);
    for(int i = 0; i<NumOfLayersHDC; ++i){
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kHDC, 0, kHitPat, 0);
    for(int i = 0; i<NumOfLayersHDC; ++i){
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    int target_id = getUniqueID(kHDC, 0, kMulti, 0);
    for(int i = 0; i<NumOfLayersHDC; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kHDC, 0, kMulti, NumOfLayersHDC);
    for(int i = 0; i<NumOfLayersHDC; ++i){
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
TList* HistMaker::createSP0(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kSP0);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // Declaration of sub-detector directory
  for(int sd = 0; sd<NumOfLayersSP0; ++sd){
    const char* nameSubDetector = Form("SP0_L%d", sd+1);
    TList *subdet_dir = new TList;
    subdet_dir->SetName(nameSubDetector);

    // ADC---------------------------------------------------------
    {
      // Declaration of the sub-directory
      std::string strSubDir  = CONV_STRING(kADC);
      const char* nameSubDir = strSubDir.c_str();
      TList *sub_dir = new TList;
      sub_dir->SetName(nameSubDir);

      // Make histogram and add it
      int target_id = getUniqueID(kSP0, kSP0_L1+sd, kADC, 0);
      for(int i = 0; i<NumOfSegSP0*2; ++i){
	const char* title = NULL;
	if(i < NumOfSegSP0){
	  int seg = i+1; // 1 origin
	  title = Form("%s_%s_%dU", nameSubDetector, nameSubDir, seg);
	}else{
	  int seg = i+1-NumOfSegSP0; // 1 origin
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
      std::string strSubDir  = CONV_STRING(kTDC);
      const char* nameSubDir = strSubDir.c_str();
      TList *sub_dir = new TList;
      sub_dir->SetName(nameSubDir);

      // Make histogram and add it
      int target_id = getUniqueID(kSP0, kSP0_L1+sd, kTDC, 0);
      for(int i = 0; i<NumOfSegSP0*2; ++i){
	const char* title = NULL;
	if(i < NumOfSegSP0){
	  int seg = i+1; // 1 origin 
	  title = Form("%s_%s_%dU", nameSubDetector, nameSubDir, seg);
	}else{
	  int seg = i+1-NumOfSegSP0; // 1 origin 
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
      int target_id = getUniqueID(kSP0, kSP0_L1+sd, kHitPat, 0);
      for(int i = 0; i<2; ++i){
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

// -------------------------------------------------------------------------
// createSDC3
// -------------------------------------------------------------------------
TList* HistMaker::createSDC3(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kSDC3);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"v0", "x0", "u0", "v1", "x1", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSDC3, 0, kTDC, 0);
    for(int i = 0; i<NumOfLayersSDC3; ++i){
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSDC3, 0, kHitPat, 0);
    for(int i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     NumOfWireSDC3uv, 0, NumOfWireSDC3uv,
			     "wire", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // Multiplicity -----------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    int target_id = getUniqueID(kSDC3, 0, kMulti, 0);
    for(int i = 0; i<NumOfLayersSDC3; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC3, 0, kMulti, NumOfLayersSDC3);
    for(int i = 0; i<NumOfLayersSDC3; ++i){
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
// createSDC4
// -------------------------------------------------------------------------
TList* HistMaker::createSDC4(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kSDC4);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // layer configuration
  const char* name_layer[] = {"v0", "x0", "u0", "v1", "x1", "u1"};

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSDC4, 0, kTDC, 0);
    for(int i = 0; i<NumOfLayersSDC4; ++i){
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSDC4, 0, kHitPat, 0);
    for(int i = 0; i<NumOfLayersSDC4; ++i){
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    // without TDC gate
    int target_id = getUniqueID(kSDC4, 0, kMulti, 0);
    for(int i = 0; i<NumOfLayersSDC4; ++i){
      const char* title = NULL;
      title = Form("%s_%s_%s", nameDetector, nameSubDir, name_layer[i]);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     10, 0, 10,
			     "Multiplicity", ""));
    }

    // with TDC gate
    target_id = getUniqueID(kSDC4, 0, kMulti, NumOfLayersSDC4);
    for(int i = 0; i<NumOfLayersSDC4; ++i){
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
TList* HistMaker::createTOF(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kTOF);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kTOF, 0, kADC, 0);
    for(int i = 0; i<NumOfSegTOF*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegTOF){
	int seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegTOF; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kTOF, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegTOF*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegTOF){
	int seg = i+1; // 1 origin 
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegTOF; // 1 origin 
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
    int target_id = getUniqueID(kTOF, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "TOF_multiplicity";
    int target_id = getUniqueID(kTOF, 0, kMulti, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Multiplicity", ""));
  }
  
  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createTOFMT
// -------------------------------------------------------------------------
TList* HistMaker::createTOFMT(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kTOFMT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kTOFMT, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegTOF; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
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
    int target_id = getUniqueID(kTOFMT, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "TOFMT_multiplicity";
    int target_id = getUniqueID(kTOFMT, 0, kMulti, 0);
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
TList* HistMaker::createLAC(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kLAC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kLAC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegLAC; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kLAC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegLAC; ++i){
      const char* title = NULL;
      if(i < NumOfSegLAC){
	int seg = i+1; // 1 origin 
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
    int target_id = getUniqueID(kLAC, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegLAC, 0, NumOfSegLAC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "LAC_multiplicity";
    int target_id = getUniqueID(kLAC, 0, kMulti, 0);
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
TList* HistMaker::createLC(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kLC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kLC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegLC*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegLC){
	int seg = i+1; // 1 origin
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegLC; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kLC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegLC*2; ++i){
      const char* title = NULL;
      if(i < NumOfSegLC){
	int seg = i+1; // 1 origin 
	title = Form("%s_%s_%dU", nameDetector, nameSubDir, seg);
      }else{
	int seg = i+1-NumOfSegLC; // 1 origin 
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
    const char* title = "LC_hit_pattern";
    int target_id = getUniqueID(kLC, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NumOfSegLC, 0, NumOfSegLC,
			   "Segment", ""));
  }

  // Multiplicity -----------------------------------------------
  {
    const char* title = "LC_multiplicity";
    int target_id = getUniqueID(kLC, 0, kMulti, 0);
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
TList* HistMaker::createMsT(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kMsT);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // Timing Counter TDC ---------------------------------------------------------
  {
    TList *sub_dir = new TList;
    sub_dir->SetName("MsT_TOF_TDC");
    int target_id = getUniqueID(kMsT, 0, kTDC, 0);
    for(int seg=0; seg<NumOfSegTOF; ++seg){
      sub_dir->Add(createTH1(++target_id, Form("%s_TOF_TDC_%d", nameDetector, seg+1),
			     0x800, 0, 0x800,
			     "Segment", ""));
    }
    top_dir->Add(sub_dir);
  }

  // Timing Counter TDC 2D -------------------------------------------------------
  {
    int target_id = getUniqueID(kMsT, 0, kTDC2D, 0);
    top_dir->Add(createTH2(++target_id, Form("%s_TOF_TDC_2D", nameDetector),
			   NumOfSegMsT, 0, NumOfSegMsT, 0x800, 0, 0x800,
			   "Segment", ""));
  }

  // HitPattern Counter Flag ----------------------------------------------------
  {
    int target_id = getUniqueID(kMsT, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, Form("%s_LC_FLAG", nameDetector),
			   NumOfSegMsT, 0, NumOfSegMsT,
			   "Segment", ""));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createTriggerFlag
// -------------------------------------------------------------------------
TList* HistMaker::createTriggerFlag(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kTriggerFlag);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    // Make histogram and add it
    int target_id = getUniqueID(kTriggerFlag, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegMisc; ++i){
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
    int target_id = getUniqueID(kTriggerFlag, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(++target_id, title, // 1 origin
			   20, 0, 20,
			   "Trigger flag", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

// -------------------------------------------------------------------------
// createTriggerFlag_E07
// -------------------------------------------------------------------------
TList* HistMaker::createTriggerFlag_E07(bool flag_ps)
{
  // Determine the detector name
  //std::string strDet = CONV_STRING(kTriggerFlag);
  std::string strDet = "TriggerFlag_E07";
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // TDC---------------------------------------------------------
  {
    // Make histogram and add it
    int target_id = getUniqueID(kTriggerFlag, 1, kTDC, 0);
    for(int i = 0; i<NumOfSegMisc; ++i){
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
    int target_id = getUniqueID(kTriggerFlag, 1, kHitPat, 0);
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
TList* HistMaker::createCorrelation(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kCorrelation);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }

  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str(); 
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);  

  {
    int target_id = getUniqueID(kCorrelation, 0, 0, 0);
    // BH2 vs BH1 -----------------------------------------------
    top_dir->Add(createTH2(++target_id, "BH2_BH1", // 1 origin
			   NumOfSegBH1, 0, NumOfSegBH1,
			   NumOfSegBH2, 0, NumOfSegBH2,
			   "BH1 seg", "BH2 seg"));

    // LC vs TOF -----------------------------------------------
    top_dir->Add(createTH2(++target_id, "LC_TOF", // 1 origin
			   NumOfSegTOF, 0, NumOfSegTOF,
			   NumOfSegLC,  0, NumOfSegLC,
			   "TOF seg", "LC seg"));

    // BC4 vs BC3 ----------------------------------------------
    top_dir->Add(createTH2(++target_id, "BC4x1_BC3x0", // 1 origin
			   NumOfWireBC3, 0, NumOfWireBC3,
			   NumOfWireBC4, 0, NumOfWireBC4,
			   "BC3 wire", "BC4 wire"));

    // SDC2 vs SDC1 --------------------------------------------
    top_dir->Add(createTH2(++target_id, "HDCx1_SDC2x0", // 1 origin
			   NumOfWireSDC2, 0, NumOfWireSDC2,
			   NumOfWireHDC,  0, NumOfWireHDC,
			   "SDC2 wire", "HDC wire"));
  }

  return top_dir;
}

// -------------------------------------------------------------------------
// createDAQ
// -------------------------------------------------------------------------
TList* HistMaker::createDAQ(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kDAQ);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }

  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str(); 
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);    


  // DAQ infomation --------------------------------------------------
  {
    // Event builder infomation
    int target_id = getUniqueID(kDAQ, kEB, kHitPat, 0);
    top_dir->Add(createTH1(target_id + 1, "Data size EB", // 1 origin
			   5000, 0, 5000,
			   "Data size [words]", ""));

    // Node information
    target_id = getUniqueID(kDAQ, kVME, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size VME nodes", // 1 origin
			   7, 0, 7,
			   500, 0, 1000,
			   "VME node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kCopper, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size Copper nodes", // 1 origin
			   15, 0, 15,
			   100, 0, 200,
			   "Copper node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kEASIROC, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size EASIROC nodes", // 1 origin
			   11, 0, 11,
			   50, 0, 100,
			   "EASIROC node ID", "Data size [words]"));

    target_id = getUniqueID(kDAQ, kCAMAC, kHitPat2D, 0);
    top_dir->Add(createTH2(target_id + 1, "Data size CAMAC nodes", // 1 origin
			   3, 0, 3,
			   50, 0, 100,
			   "CAMAC node ID", "Data size [words]"));
  }
  
  {
    // TKO box information
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kTKO);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kDAQ, kTKO, kHitPat2D, 0);
    for(int box = 0; box<6; ++box){
      const char* title = NULL;
      title = Form("TKO box%d", box);
      sub_dir->Add(createTH2(target_id + box+1, title, // 1 origin
			     24, 0, 24, 
			     40, 0, 40,
			     "TKO MA", "N of decoded hits"));
      
      top_dir->Add(sub_dir);
    }
  }

  return top_dir;  
}


// -------------------------------------------------------------------------
// Old functions
// -------------------------------------------------------------------------
TList* HistMaker::createBAC_SAC(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kBAC_SAC);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ACs configuration
  const char* name_acs[] = {"BAC0a", "BAC0b", "BAC1", "BAC2", "SAC1", "SAC2"};

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBAC_SAC, 0, kADC, 0);
    for(int i = 0; i<NumOfSegBAC_SAC; ++i){
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kBAC_SAC, 0, kTDC, 0);
    for(int i = 0; i<NumOfSegBAC_SAC; ++i){
      const char* title = NULL;
      title = Form("%s_%s", name_acs[i], nameSubDir);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }

    // insert sub directory
    top_dir->Add(sub_dir);
  }
  
  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

TList* HistMaker::createSFV_SAC3(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kSFV_SAC3);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const int NofLoop = 7;
  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSFV_SAC3, 0, kADC, 0);
    for(int i = 0; i<NofLoop; ++i){
      const char* title = NULL;
      if(i<NofLoop-1){
	int seg = i+1; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kSFV_SAC3, 0, kTDC, 0);
    for(int i = 0; i<NofLoop; ++i){
      const char* title = NULL;
      if(i<NofLoop-1){
	int seg = i+1; // 1 origin
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
    int target_id = getUniqueID(kSFV_SAC3, 0, kHitPat, 0);
    // Add to the top directory
    top_dir->Add(createTH1(target_id + 1, title, // 1 origin
			   NofLoop, 0, NofLoop,
			   "Segment", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}

TList* HistMaker::createGe(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kGe);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kGe, 0, kADC, 0);
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin
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
    std::string strSubDir  = CONV_STRING(kCRM);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kGe, 0, kCRM, 0);
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kTFA);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kGe, 0, kTFA, 0);
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kPUR);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kGe, 0, kPUR, 0);
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kRST);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kGe, 0, kRST, 0);
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    // Make histogram and add it
    int target_id = getUniqueID(kGe, 0, kMulti, 0);
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
      title = Form("%s_MultiCRM_%d", nameDetector, seg);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
			     16, 0, 16,
			     "Multi CRM [ch]", ""));
    }

    target_id = getUniqueID(kGe, 0, kMulti, 0) + NumOfSegGe;
    for(int i = 0; i<NumOfSegGe; ++i){
      const char* title = NULL;
      int seg = i+1; // 1 origin 
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

TList* HistMaker::createPWO(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kPWO);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet); 
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet); 
  }
  
  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kPWO, 0, kADC2D, 0);
    for(int i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      int box = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    
    // sum hist
    int target_id = getUniqueID(kPWO, 0, kTDC, 0);
    for(int i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      int box = i+1; // 1 origin 
      title = Form("%s_%ssum_Box%d", nameDetector, nameSubDir, box);
      sub_dir->Add(createTH1(target_id + i+1, title, // 1 origin
  			     0x1000, 0, 0x1000,
  			     "TDC [ch]", ""));
    }

    target_id = getUniqueID(kPWO, 0, kTDC2D, 0);
    for(int i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      int box = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    
    int target_id = getUniqueID(kPWO, 0, kHitPat, 0);
    for(int i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      int box = i+1; // 1 origin 
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
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);
    
    int target_id = getUniqueID(kPWO, 0, kMulti, 0);
    for(int i = 0; i<NumOfBoxPWO; ++i){
      const char* title = NULL;
      int box = i+1; // 1 origin 
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

TList* HistMaker::createPWO_E05(bool flag_ps)
{
  // Determine the detector name
  std::string strDet = CONV_STRING(kPWO);
  // name list of crearted detector
  name_created_detectors_.push_back(strDet);
  if(flag_ps){
    // name list which are displayed in Ps tab
    name_ps_files_.push_back(strDet);
  }

  // Declaration of the directory
  // Just type conversion from std::string to char*
  const char* nameDetector = strDet.c_str();
  TList *top_dir = new TList;
  top_dir->SetName(nameDetector);

  const int box_id = 7; // PBox 8 (E-6)

  // ADC---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kADC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kPWO, 0, kADC, 0);
    for(int i=0; i<NumOfUnitPWO[box_id]; ++i){
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
    std::string strSubDir  = CONV_STRING(kTDC);
    const char* nameSubDir = strSubDir.c_str();
    TList *sub_dir = new TList;
    sub_dir->SetName(nameSubDir);

    int target_id = getUniqueID(kPWO, 0, kTDC, 0);
    for(int i=0; i<NumOfUnitPWO[box_id]; ++i){
      sub_dir->Add(createTH1(++target_id, Form("%s_%s_%d",
					       nameDetector, nameSubDir, i+1),
			     0x1000, 0, 0x1000,
			     "TDC [ch]", ""));
    }
    // insert sub directory
    top_dir->Add(sub_dir);
  }

  // HitPat---------------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kHitPat);
    const char* nameSubDir = strSubDir.c_str();

    int target_id = getUniqueID(kPWO, 0, kHitPat, 0);
    top_dir->Add(createTH1(++target_id, Form("%s_%s", nameDetector, nameSubDir),
			   NumOfUnitPWO[box_id], 0, NumOfUnitPWO[box_id],
			   "Segment", ""));
  }

  // Multiplicity---------------------------------------------------
  {
    // Declaration of the sub-directory
    std::string strSubDir  = CONV_STRING(kMulti);
    const char* nameSubDir = strSubDir.c_str();

    int target_id = getUniqueID(kPWO, 0, kMulti, 0);
    top_dir->Add(createTH1(++target_id, Form("%s_%s", nameDetector, nameSubDir),
			   NumOfUnitPWO[box_id]+1, 0, NumOfUnitPWO[box_id]+1,
			   "Multiplicity", ""));
  }

  // Return the TList pointer which is added into TGFileBrowser
  return top_dir;
}
