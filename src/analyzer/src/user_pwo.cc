// Author: Tomonori Takahashi

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>

#include "Controller.hh"

#include "user_analyzer.hh"
#include "UnpackerManager.hh"
#include "filesystem_util.hh"
#include "ConfMan.hh"
#include "UserParamMan.hh"
#include "HistMaker.hh"
#include "DetectorID.hh"
#include "PsMaker.hh"
#include "GuiPs.hh"
#include "MacroBuilder.hh"

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  std::vector<TH1*> hptr_array;
  
  static const int NofPWO = 214;

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::getInstance();
  gConfMan.initialize(argv);
  if(!gConfMan.isGood()){return -1;}
  // unpacker and all the parameter managers are initialized at this stage

  // Make tabs
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();
  TGFileBrowser *tab_hist  = gCon.makeFileBrowser("Hist");
  TGFileBrowser *tab_macro = gCon.makeFileBrowser("Macro");

  // Add macros to the Macro tab
  //  tab_macro->Add(hoge());
  tab_macro->Add(clear_canvas());

  // Add histograms to the Hist tab
  HistMaker& gHist = HistMaker::getInstance();
  // PWO ADC
  {
    TList *dir_adc = new TList();
    dir_adc->SetName("ADC");  
    
    int base_id = gHist.getUniqueID(kMisc, 0, kADC, 0);
    for(int i = 0; i<NofPWO; ++i){
      // Hist index is 1 origin
      const char* title = Form("PWO_ADC_%d", i+1);
      dir_adc->Add(gHist.createTH1(base_id +i+1, title, 0x1000, 0, 0x1000));
    }

    tab_hist->Add(dir_adc);
  }

  // PWO TDC
  {
    TList *dir_tdc = new TList();
    dir_tdc->SetName("TDC");  
    
    int base_id = gHist.getUniqueID(kMisc, 0, kTDC, 0);
    for(int i = 0; i<NofPWO; ++i){
      // Hist index is 1 origin
      const char* title = Form("PWO_TDC_%d", i+1);
      dir_tdc->Add(gHist.createTH1(base_id +i+1, title, 0x1000, 0, 0x1000));
    }

    tab_hist->Add(dir_tdc);
  }

  // Set histogram pointers to the vector sequentially.
  // This vector contains both TH1 and TH2.
  // Then you need to do down cast when you use TH2.
  if(0 != gHist.setHistPtr(hptr_array)){return -1;}

  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  hptr_array.clear();
  return 0;
}

//____________________________________________________________________________
int
process_event()
{
  static UnpackerManager& gUnpacker = GUnpacker::get_instance();
  static HistMaker&       gHist     = HistMaker::getInstance();

  // PWO -----------------------------------------------------------
  {
    static const int pwo_adc = gHist.getSequentialID(kMisc, 0, kADC, 1);
    static const int pwo_tdc = gHist.getSequentialID(kMisc, 0, kTDC, 1);

    for(int i = 0; i<NofPWO; ++i){
      // do something....
      // do something....
      // do something....
      int adc_val =0;
      int tdc_val =0;
      hptr_array[pwo_adc + i]->Fill(adc_val);
      hptr_array[pwo_tdc + i]->Fill(tdc_val);
    }
  }

  return 0;
}

}
