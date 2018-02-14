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

  hddaq::cout << "#D " << FUNC_NAME
	      << " argument list" << std::endl;
  std::copy(argv.begin(), argv.end(),
	    std::ostream_iterator<std::string>(hddaq::cout, "\n"));
  hddaq::cout << std::endl;

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

  hddaq::cout << " config file = " << confFile << std::endl;
  TString dir = hddaq::dirname( confFile );
  dir += "/";
  hddaq::cout << " dir = " << dir << std::endl;
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
	  TString key   = param[0];
	  TString value = param[1];
	  key.ReplaceAll(":","");
	  key.ReplaceAll(";","");
	  key.ReplaceAll(" ","");
	  if ( value[0] != '/' )
	    value = hddaq::realpath( std::string(dir+value) );

	  hddaq::cout << " key = " << key
		    << " value = " << value << std::endl;
	  m_key_map[key] = value;
	}
    }

  // initialize unpacker system
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  g_unpacker.set_config_file( std::string(m_key_map["UNPACKER"]),
			      std::string(m_key_map["DIGIT"]),
			      std::string(m_key_map["CMAP"]) );
  g_unpacker.set_istream(dataSrc);

  hddaq::cout << std::endl;

  // Initialize of ConfMan and Unpacker were done
  flag_.set(kIsGood);
}
