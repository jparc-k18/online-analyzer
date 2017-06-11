// Author: Tomonori Takahashi

// std c++
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// ROOT
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>

// gui test
#include "Main.hh"
#include "Sigwait.hh"
#include "Updater.hh"
#include "Controller.hh"

using namespace hddaq;
using namespace hddaq::gui;
using namespace analyzer;

TROOT the_ROOT("gui_main", "application with GUI");

int main(int argc, char* argv[])
{
  gROOT->SetStyle("Classic");
  std::vector<std::string> argV(argv, argv+argc);

  // It is safe to call constructor of singleton object
  // (getInstance() method) before creating threads.

  Sigwait& g_sigwait = Sigwait::getInstance();
  g_sigwait.run(); 

  Main& g_main = Main::getInstance();

  Updater& g_updater = Updater::getInstance();

  // ______ setup CINT ________
  // The TRint or TApplication object
  // must be constructed before other GUI applications.
  const std::string cint_pwd(".");
  int argc_cint = 2;
  char* argv_cint[argc_cint];
  argv_cint[0] = argv[0];
  argv_cint[1] = const_cast<char*>(cint_pwd.c_str());
  TRint cint("cint", &argc_cint, argv_cint, 0, 0, kTRUE);
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigInterrupt);

  g_main.initialize(argV);
  gSystem->Sleep(200);

  // ______ construct GUI ______
  Controller::getInstance();
  gSystem->Sleep(200);

  g_main.setBatchMode(false);

  g_main.start();
  gSystem->Sleep(200);

  g_updater.start();
  gSystem->Sleep(200);

  cint.Run();

  g_updater.join();
  gSystem->Sleep(200);

  g_main.join();
  gSystem->Sleep(200);

  return 0;
}
