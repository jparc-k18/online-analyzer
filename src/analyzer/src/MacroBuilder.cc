// -*- C++ -*-

#include "MacroBuilder.hh"

#include <string>

#include <TMacro.h>
#include <TString.h>

#include "Main.hh"

namespace analyzer
{

namespace macro
{

//____________________________________________________________________________
TObject*
Get( TString name )
{
  std::string process = Main::getInstance().getArgv().at(0);
  Int_t n = process.find("bin");
  TString path = process.substr(0, n)+"src/analyzer/macro/";

  if( name.Contains(".C") )
    name.ReplaceAll(".C","");

  return new TMacro(path+name+".C");
}

}

}
