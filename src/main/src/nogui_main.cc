// Author: Tomonori Takahashi

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <TROOT.h>

#include "Main.hh"
#include "Sigwait.hh"
#include "user_analyzer.hh"

using namespace analyzer;

TROOT theROOT("noGui", "analysis without GUI controller");

int main(int argc, char* argv[])
{
  std::vector<std::string> argV(argv, argv+argc);

  Main& g_main = Main::getInstance();
  g_main.initialize(argV);
  Sigwait& g_sigwait = Sigwait::getInstance();
  g_sigwait.run();

  g_main.setBatchMode(true);
  g_main.run();
  closeTFile();
  g_sigwait.kill();
  return 0;
}
