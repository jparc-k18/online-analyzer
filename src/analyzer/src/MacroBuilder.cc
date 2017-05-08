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
inline TString
GetMacroPath( void )
{
  std::string process = Main::getInstance().getArgv().at(0);
  Int_t n = process.find("bin");
  return process.substr(0, n)+"src/analyzer/macro/";
}

//____________________________________________________________________________
TObject*
Get( TString key )
{
  if( key.Contains(".C") )
    key.ReplaceAll(".C","");
  TMacro *m = new TMacro(GetMacroPath()+key+".C");
  m->SetName(key);
  return m;
}

}

}
