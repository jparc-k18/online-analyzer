#ifndef HISTMAKER_HH
#define HISTMAKER_HH

#include<vector>
#include<map>
#include<bitset>
#include<string>

#include<TROOT.h>

enum DetectorType {
  kDetectorZero,
  // Detector unique ID in the beam line
  kBH1, kBFT, kBC3, kBC4, kBH2, kBAC, kFBH,
  kSSDT, kSSD1, kSSD2,
  kPVAC, kFAC, kSCH, kEMC, kMsT, kMtx3D,
  // Detector unique ID in the SKS system
  kSDC1, kSDC2, kSDC3, kTOF,
  // Old detectors
  kBMW, kBAC_SAC, kSFV_SAC3, kGe, kPWO, kSP0,
  kBH2_E07, kBAC_E07, kSSD0, kSAC1, kKFAC,
  kKIC, kHDC, kSDC4, kTOFMT, kLAC, kLC,
  // Others
  kTriggerFlag, kDAQ, kCorrelation, kMisc,
  kTimeStamp, kDCEff,
  sizeDetectorType,
  factorDetectorType = 10000000
};

enum SubDetectorType {
  kSubDetectorZero,
  // Detector unique sub ID in Counters
  kSP0_L1, kSP0_L2, kSP0_L3, kSP0_L4,
  kSP0_L5, kSP0_L6, kSP0_L7, kSP0_L8,
  // Sub Detector ID in HUL
  kHulTOF,     kHulFBH,     kHulSCH,
  kHulTOFxFBH, kHulTOFxSCH, kHulFBHxSCH,
  kHul2DHitPat, kHul3DHitPat,
  kHul2D, kHul3D,
  // DAQ unique sub ID
  kEB, kTKO, kVME, kCLite, kEASIROC, kCAMAC,
  kMiscNode,
  sizeSubDetectorType,
  factorSubDetectorType = 100000
};

enum DataType{
  kDataTypeZero,
  // Usual data type
  kADC,   kTDC,   kHitPat,   kMulti,
  kADC2D, kTDC2D, kHitPat2D, kMulti2D,
  kADCwTDC,
  // Extra data type for EMC
  kSerial, kXpos, kYpos, kXYpos, kTime,
  // Extra data type for Ge detector
  kCRM,   kTFA,   kPUR,   kRST,
  kCRM2D, kTFA2D, kPUR2D, kRST2D,
  sizeDataType,
  factorDataType = 1000
};

std::string getStr_FromEnum(const char* c);

class TH1;
class TH2;
class TList;

class HistMaker {
  // Declaration of the private parameters ---------------------------------
  // histogram unique and sequential ID
  int current_hist_id_;
  std::map<int, int>         idmap_seq_from_unique_;
  std::map<int, int>         idmap_unique_from_seq_;
  std::map<std::string, int> idmap_seq_from_name_;

  // data set of string which means the created detector name
  std::vector<std::string>      name_created_detectors_;
  std::vector<std::string>      name_ps_files_;

  // return value of insert
  typedef std::pair<std::map<int, int>::iterator, bool> TypeRetInsert;

public:
  // Public functions ------------------------------------------------------
  virtual ~HistMaker( void );
  static HistMaker& getInstance( void );
  static void getListOfDetectors(std::vector<std::string>& vec);
  static void getListOfPsFiles(std::vector<std::string>& vec);

  static int  getNofHist();

  static int  getUniqueID(int detector_type, int subdetector_type,
			 int data_type, int channel=1 );
  static int  getUniqueID(int sequential_id);

  static int  getSequentialID(int detector_type, int subdetector_type,
			     int data_type, int channel=1 );
  static int  getSequentialID(int unique_id);
  static int  getSequentialID(const char* name_hist);

  int  setHistPtr(std::vector<TH1*>& vec);

  TH1* createTH1(int unique_id, const char* title,
		 int nbinx, int xmin, int xmax,
		 const char* xtitle="", const char* ytitle=""
		 );

  TH2* createTH2(int unique_id, const char* title,
  		 int nbinx, int xmin, int xmax,
  		 int nbiny, int ymin, int ymax,
  		 const char* xtitle="", const char* ytitle=""
  		 );

  TList* createTimeStamp(bool flag_ps=true);
  TList* createBH1(bool flag_ps=true);
  TList* createBFT(bool flag_ps=true);
  TList* createBC3(bool flag_ps=true);
  TList* createBC4(bool flag_ps=true);
  TList* createBH2(bool flag_ps=true);
  TList* createBAC(bool flag_ps=true);
  TList* createFBH(bool flag_ps=true);
  TList* createSSDT(bool flag_ps=true);
  TList* createSSD1(bool flag_ps=true);
  TList* createSSD2(bool flag_ps=true);
  TList* createPVAC(bool flag_ps=true);
  TList* createFAC(bool flag_ps=true);
  TList* createSCH(bool flag_ps=true);
  TList* createEMC(bool flag_ps=true);
  TList* createSDC1(bool flag_ps=true);
  TList* createSDC2(bool flag_ps=true);
  TList* createSDC3(bool flag_ps=true);
  TList* createTOF(bool flag_ps=true);
  TList* createMsT(bool flag_ps=true);
  TList* createMtx3D(bool flag_ps=false);
  TList* createTriggerFlag(bool flag_ps=true);
  TList* createCorrelation(bool flag_ps=true);
  TList* createDAQ(bool flag_ps=true);
  TList* createDCEff(bool flag_ps=true);

  // Old functions
  TList* createBMW(bool flag_ps=true);
  TList* createBAC_SAC(bool flag_ps=true);
  TList* createSFV_SAC3(bool flag_ps=true);
  TList* createGe(bool flag_ps=true);
  TList* createPWO(bool flag_ps=true);
  TList* createBH2_E07(bool flag_ps=true);
  TList* createBAC_E07(bool flag_ps=true);
  TList* createSSD0(bool flag_ps=true);
  TList* createSAC1(bool flag_ps=true);
  TList* createKFAC(bool flag_ps=true);
  TList* createKIC(bool flag_ps=true);
  TList* createHDC(bool flag_ps=true);
  TList* createSP0(bool flag_ps=true);
  TList* createSDC4(bool flag_ps=true);
  TList* createTOFMT(bool flag_ps=true);
  TList* createLAC(bool flag_ps=true);
  TList* createLC(bool flag_ps=true);
  TList* createTriggerFlag_E07(bool flag_ps=true);
  TList* createPWO_E05(bool flag_ps=true);

private:
  HistMaker();
  HistMaker(const HistMaker& object);
  HistMaker& operator=(const HistMaker& object);

  ClassDef(HistMaker, 0)
};

// getInstance -----------------------------------------------------------
inline HistMaker& HistMaker::getInstance()
{
  static HistMaker object;
  return object;
}

// getNofHist ------------------------------------------------------------
inline int HistMaker::getNofHist()
{
  HistMaker& g= HistMaker::getInstance();
  return g.idmap_unique_from_seq_.size();
}

// getUniqueID -----------------------------------------------------------
inline int HistMaker::getUniqueID(int detector_type, int subdetector_type,
				  int data_type, int channel)
{
  int unique_id = 0;
  unique_id += detector_type*factorDetectorType;
  unique_id += subdetector_type*factorSubDetectorType;
  unique_id += data_type*factorDataType;
  unique_id += channel;
  return unique_id;
}

// getUniqueID -----------------------------------------------------------
inline int HistMaker::getUniqueID(int sequential_id)
{
  HistMaker& g= HistMaker::getInstance();
  return g.idmap_unique_from_seq_[sequential_id];
}

// getSequentialID -------------------------------------------------------
inline int HistMaker::getSequentialID(int detector_type, int subdetector_type,
				      int data_type, int channel)
{
  HistMaker& g= HistMaker::getInstance();
  int unique_id = g.getUniqueID(detector_type, subdetector_type, data_type, channel);
  return g.idmap_seq_from_unique_[unique_id];
}

// getSequentialID -------------------------------------------------------
inline int HistMaker::getSequentialID(int unique_id)
{
  HistMaker& g= HistMaker::getInstance();
  return g.idmap_seq_from_unique_[unique_id];
}

// getSequentialID -------------------------------------------------------
inline int HistMaker::getSequentialID(const char* name)
{
  HistMaker& g= HistMaker::getInstance();
  std::string nameStr = name;
  return g.idmap_seq_from_name_[nameStr];
}


#endif
