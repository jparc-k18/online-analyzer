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

// analyzer
#include "Main.hh"

// for batch application
#include "Sigwait.hh"
#include "user_analyzer.hh"

// for gui application
#include "Updater.hh"
#include "Controller.hh"

using namespace hddaq;
using namespace hddaq::gui;
using namespace analyzer;

TROOT theROOT("main", "analyzer application");

int main(int argc, char* argv[])
{
  gROOT->Reset();

  std::vector<std::string> argV(argv, argv+argc);
  Main& g_main = Main::getInstance();

  std::vector<std::string>::iterator batchOpt
    = std::find(argV.begin(), argV.end(), "-b");
  if (batchOpt!=argV.end())
    {
      std::cout << "#D batch mode selected" << std::endl;
      g_main.setBatchMode(true);
      argV.erase(std::remove(argV.begin(), argV.end(), *batchOpt));
    }

  std::vector<std::string>::iterator forceOpt
    = std::find(argV.begin(), argV.end(), "-f");
  if (forceOpt!=argV.end())
    {
      std::cout << "#D overwrite root file (if provieded)" << std::endl;
      g_main.setForceOverwrite(true);
      argV.erase(std::remove(argV.begin(), argV.end(), *forceOpt));
    }

  std::copy(argV.begin(), argV.end(),
	    std::ostream_iterator<std::string>(std::cout, "\n "));
  std::cout << std::endl;
  if (g_main.isBatch())
    {
      Sigwait& g_sigwait = Sigwait::getInstance();
      g_sigwait.run();
      g_main.initialize(argV);

      g_main.run();
      closeTFile();
      g_sigwait.kill();
    }
  else
    {
      // It is safe to call constructor of singleton object
      // (getInstance() method) before creating threads.
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

      g_main.initialize(argV);

      // ______ construct GUI ______
      Controller::getInstance();

      ::usleep(2000);

      g_updater.start();

      g_main.start();

      cint.Run();

      g_updater.join();
      g_main.join();
    }

  return 0;
}
