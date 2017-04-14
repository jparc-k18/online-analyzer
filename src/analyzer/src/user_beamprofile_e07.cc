// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TString.h>

#include "Controller.hh"
#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "ConfMan.hh"
#include "DCAnalyzer.hh"
#include "DCGeomMan.hh"
#include "DetectorID.hh"
#include "HistMaker.hh"
#include "MacroBuilder.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  std::vector<TH1*> hptr_array;
  
  enum HistName
    {
      FF_200, FF_500, FF_680, FF_1000, FF_1200,
      size_HistName
    };
  static const double FF_plus[] =
    {
      200., 500., 680, 1000., 1200.
    };

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);
  gConfMan.initializeDCGeomMan();
  gConfMan.initializeDCTdcCalibMan();
  gConfMan.initializeDCDriftParamMan();
  gConfMan.initializeUserParamMan();
  if(!gConfMan.isGood()){return -1;}
  // unpacker and all the parameter managers are initialized at this stage

  // Make tabs
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();
  TGFileBrowser *tab_hist  = gCon.makeFileBrowser("Hist");
  TGFileBrowser *tab_macro = gCon.makeFileBrowser("Macro");

  // Add macros to the Macro tab
  tab_macro->Add(clear_all_canvas());
  tab_macro->Add(clear_canvas());
  tab_macro->Add(split22());
  tab_macro->Add(split32());
  tab_macro->Add(split33());
  tab_macro->Add(dispBeamProfile_e07());
  tab_macro->Add(dispBcOutFF());
  tab_macro->Add(dispSSD1Profile());

  // Add histograms to the Hist tab
  HistMaker& gHist = HistMaker::getInstance();
  //BcOut
  {
    TList *sub_dir = new TList;
    const char* nameSubDir = "BcOut";
    sub_dir->SetName(nameSubDir);
    int unique_id = gHist.getUniqueID(kMisc, 0, kHitPat);
    // Profile X
    for(int i = 0; i<size_HistName; ++i){
      char* title = Form("%s FF %d_X", nameSubDir, (int)FF_plus[i]);
      sub_dir->Add(gHist.createTH1(unique_id++, title,
				    400,-200,200,
				    "x position [mm]", ""));
    }
    // Profile Y
    for(int i = 0; i<size_HistName; ++i){
      char* title = Form("%s FF %d_Y", nameSubDir, (int)FF_plus[i]);
      sub_dir->Add(gHist.createTH1(unique_id++, title,
				    200,-100,100,
				    "y position [mm]", ""));
    }
    tab_hist->Add(sub_dir);
    // Profile XY
    for(int i = 0; i<size_HistName; ++i){
      char* title = Form("%s FF %d_XY", nameSubDir, (int)FF_plus[i]);
      sub_dir->Add(gHist.createTH2(unique_id++, title,
				   400,-200,200, 200,-100,100,
				   "x position [mm]", "y position [mm]"));
    }
    tab_hist->Add(sub_dir);
  }

  //SSD1
  {
    TList *sub_dir = new TList;
    const char* nameSubDir = "SSD1";
    sub_dir->SetName(nameSubDir);
    int unique_id = gHist.getUniqueID(kMisc, 0, kHitPat2D);
    const char* nameLayer[] = { "Y0", "X0", "Y1", "X1" };
    for(int l=0; l<NumOfLayersSSD1; ++l){
      char* title = Form("%s_HitPos_%s", nameSubDir, nameLayer[l]);
      sub_dir->Add(gHist.createTH1(unique_id++, title,
				   200,-100,100,
				   "Position [mm]", ""));
    }
    tab_hist->Add(sub_dir);
  }

  // Set histogram pointers to the vector sequentially.
  // This vector contains both TH1 and TH2.
  // Then you need to do down cast when you use TH2.
  if(0 != gHist.setHistPtr(hptr_array)){return -1;}

  gStyle->SetOptStat(1110);
  gStyle->SetTitleW(.4);
  gStyle->SetTitleH(.1);
  gStyle->SetStatW(.42);
  gStyle->SetStatH(.3);

  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  return 0;
}

//____________________________________________________________________________
int
process_event()
{
  static HistMaker& gHist = HistMaker::getInstance();
  static UnpackerManager& gUnpacker = GUnpacker::get_instance();
  static DCGeomMan&       gGeom     = DCGeomMan::GetInstance();

  static const double dist_FF = 1200.;

  /////////// BcOut
  {
    DCRHC BcOutAna(DetIdBcOut);
    bool BcOutTrack = BcOutAna.TrackSearch(9);

    if(BcOutTrack){
      static const int xpos_id = gHist.getSequentialID(kMisc, 0, kHitPat);
      static const int ypos_id = gHist.getSequentialID(kMisc, 0, kHitPat, size_HistName+1);
      static const int xypos_id = gHist.getSequentialID(kMisc, 0, kHitPat, size_HistName*2+1);

      for(int i = 0; i<size_HistName; ++i){
	double xpos = BcOutAna.GetPosX(dist_FF+FF_plus[i]);
	double ypos = BcOutAna.GetPosY(dist_FF+FF_plus[i]);
	hptr_array[xpos_id+i]->Fill(xpos);
	hptr_array[ypos_id+i]->Fill(ypos);
	hptr_array[xypos_id+i]->Fill(xpos, ypos);
      }
    }
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  ////////// SSD1
  {
    // data type
    static const int k_device = gUnpacker.get_device_id("SSD1");
    static const int k_adc    = gUnpacker.get_data_id("SSD1","adc");
    static const int k_flag   = gUnpacker.get_data_id("SSD1","flag");
    // sequential id
    static const int hit_id = gHist.getSequentialID(kMisc, 0, kHitPat2D);

    bool chit_flag[NumOfLayersSSD1][NumOfSegSSD1];

    for(int l=0; l<NumOfLayersSSD1; ++l){
      for(int seg=0; seg<NumOfSegSSD1; ++seg){
	chit_flag[l][seg] = false;
	// ADC
	int nhit_a = gUnpacker.get_entries(k_device, l, seg, 0, k_adc);
	if( nhit_a==0 ) continue;
	if( nhit_a != NumOfSamplesSSD ){
	  std::cerr << "#W SSD1 layer:" << l << " seg:" << seg
		    << " the number of samples is wrong : "
		    << nhit_a << "/" << NumOfSamplesSSD << std::endl;
	  continue;
	}
	int  adc[nhit_a];
	bool slope[nhit_a-1];
	int  peak_height   = -1;
	int  peak_position = -1;
	for(int m=0; m<nhit_a; ++m){
	  adc[m] = gUnpacker.get(k_device, l, seg, 0, k_adc, m);
	  adc[m] -= adc[0];
	  if( m>0 )
	    slope[m] = adc[m]>adc[m-1];
	  if( adc[m]>peak_height ){
	    peak_height   = adc[m];
	    peak_position = m;
	  }
	}
	// Zero Suppression Flag
	int  nhit_flag = gUnpacker.get_entries(k_device, l, seg, 0, k_flag);
	bool  hit_flag = false;
	if(nhit_flag != 0){
	  int flag = gUnpacker.get(k_device, l, seg, 0, k_flag);
	  if(flag==1) hit_flag = true;
	}
	chit_flag[l][seg] = hit_flag &&
	  slope[0] &&
	  slope[1] && slope[2] && !slope[4] && !slope[5] && !slope[6];
	  //&& ( peak_height > 350 );
	if( peak_height>=0 && peak_position>=1 && chit_flag[l][seg] ){
	  double wpos = gGeom.calcWirePosition( l+7, seg+1 );
	  hptr_array[hit_id +l]->Fill( wpos );
	}
      }//for(seg)
    }//for(l)
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif
  
  return 0;
}

}
