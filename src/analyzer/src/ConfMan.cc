// -*- C++ -*-

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <TApplication.h>

#include <UnpackerManager.hh>
#include <Unpacker.hh>
#include <filesystem_util.hh>
#include <std_ostream.hh>

#include "ConfMan.hh"
#include "FuncName.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "DCDriftParamMan.hh"
#include "UserParamMan.hh"
#include "MatrixParamMan.hh"
#include "MsTParamMan.hh"
#include "GeAdcCalibMan.hh"

ClassImp(ConfMan);

//______________________________________________________________________________
ConfMan::ConfMan( void )
{
  flag_.reset();
}

//______________________________________________________________________________
ConfMan::~ConfMan( void )
{
}

//______________________________________________________________________________
void
ConfMan::Initialize( const std::vector<std::string>& argv )
{
  using namespace hddaq;
  using namespace hddaq::unpacker;

  std::cout << "#D " << FUNC_NAME
	    << " argument list" << std::endl;
  std::copy(argv.begin(), argv.end(),
	    std::ostream_iterator<std::string>(std::cout, "\n"));
  std::cout << std::endl;

  int nArg = argv.size();
  if (sizeArgumentList > nArg)
    {
      std::cerr << "# Usage: "
		<< basename( argv[kProcess] )
		<< " [config file] [input stream]"
		<< std::endl;
      std::exit(0);
    }

  const std::string& confFile(argv[kConfPath]);
  const std::string& dataSrc(argv[kStreamPath]);

  std::cout << " config file = " << confFile << std::endl;
  TString dir = hddaq::dirname( confFile );
  dir += "/";
  std::cout << " dir = " << dir << std::endl;
  std::ifstream conf( confFile.c_str() );
  while (conf.good())
    {
      std::string l;
      std::getline(conf, l);
      std::istringstream iss(l);
      std::istream_iterator<std::string> issBegin(iss);
      std::istream_iterator<std::string> issEnd;
      std::vector<std::string> param(issBegin, issEnd);
      if (param.empty())
	continue;
      if (param[0].empty())
	continue;
      if (param.size()==2)
	{
	  const TString& name = param[0];
	  TString value = param[1];
	  std::cout << " key = " << name
		    << " value = " << value << std::endl;
	  if ( value[0] != '/' )
	    value = hddaq::realpath( std::string(dir+value) );
	  name_file_[name] = value;
	}
    }

  // initialize unpacker system
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  g_unpacker.set_config_file( std::string(name_file_["UNPACKER:"]),
			      std::string(name_file_["DIGIT:"]),
			      std::string(name_file_["CMAP:"]) );
  g_unpacker.set_istream(dataSrc);

  // Initialize of ConfMan and Unpacker were done
  flag_.set(kIsGood);
}
