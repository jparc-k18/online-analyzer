
// Author: Tomonori Takahashi

//#include <unistd.h>
//#include "UnpackerManager.hh"


#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>

#include <DAQNode.hh>
#include <filesystem_util.hh>
#include <Unpacker.hh>
#include <UnpackerManager.hh>

#include "Controller.hh"
#include "user_analyzer.hh"

#include "ConfMan.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "DetectorID.hh"
#include "GuiPs.hh"
#include "HistMaker.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "MacroBuilder.hh"
#include "MatrixParamMan.hh"
#include "MsTParamMan.hh"
#include "ProcInfo.hh"
#include "PsMaker.hh"
#include "SsdAnalyzer.hh"
#include "UserParamMan.hh"


namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  if( !gConfMan.IsGood() ) return -1;
  //  GUnpacker::get_instance().set_decode_mode(false);

  // Make tabs
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();
  TGFileBrowser *tab_hist  = gCon.makeFileBrowser("Hist");
  TGFileBrowser *tab_macro = gCon.makeFileBrowser("Macro");

  // Add macros to the Macro tab
  //tab_macro->Add(macro::Get("dispGe"));
  tab_macro->Add(macro::Get("dispGeAdc"));
  tab_macro->Add(macro::Get("dispGeTdc"));
  tab_macro->Add(macro::Get("dispGeAdc_60Co_1170"));
  tab_macro->Add(macro::Get("dispGeTdc_60Co_1330"));
  tab_macro->Add(macro::Get("dispGeAdc_60Co"));

  // Add histograms to the Hist tab
  HistMaker& gHist = HistMaker::getInstance();
  tab_hist->Add(gHist.createGe());
  tab_hist->Add(gHist.createBGO());


  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  std::cout << "#D skeleton::process_end End" << std::endl;
  return 0;
}

//____________________________________________________________________________
int
process_event()
{
  sleep(1);
  std::cout << "#D skeleton::process_event" << std::endl;

  return 0;
}

}
