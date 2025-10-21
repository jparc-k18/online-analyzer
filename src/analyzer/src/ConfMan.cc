// -*- C++ -*-

#include "ConfMan.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#include <lexical_cast.hh>
#include <filesystem_util.hh>
#include <replace_string.hh>
#include <std_ostream.hh>

// #include "BH1Filter.hh"
#include "BH1Match.hh"
#include "BH2Filter.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "DCDriftParamMan.hh"
#include "EventDisplay.hh"
#include "FieldMan.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "K18TransMatrix.hh"
#include "MatrixParamMan.hh"
#include "MsTParamMan.hh"
#include "UnpackerManager.hh"
#include "UserParamMan.hh"

namespace
{
  using namespace hddaq::unpacker;
  const std::string& class_name("ConfMan");
  const std::string& kConfFile("CONF");
  std::string sConfDir;
  UnpackerManager&      gUnpacker = GUnpacker::get_instance();
  const MatrixParamMan& gMatrix   = MatrixParamMan::GetInstance();
  const UserParamMan&   gUser     = UserParamMan::GetInstance();
}

ClassImp( ConfMan );

//_____________________________________________________________________________
ConfMan::ConfMan( void )
  : m_is_ready( false )
{
  flag_.reset();
}

//_____________________________________________________________________________
ConfMan::~ConfMan( void )
{
}

//_____________________________________________________________________________
Bool_t
ConfMan::Contains( const TString& key ) const
{
  return m_key_map.find( key ) != m_key_map.end();
}

//_____________________________________________________________________________
bool
ConfMan::Initialize( void )
{
  static const std::string func_name("["+class_name+"::"+__func__+"()]");

  if( m_is_ready ){
    hddaq::cerr << "#W " << func_name
		<< " already initialied" << std::endl;
    return false;
  }

  std::ifstream ifs( m_file[kConfFile].c_str() );
  if( !ifs.is_open() ){
    hddaq::cerr << "#E " << func_name
		<< " cannot open file : " << m_file[kConfFile] << std::endl;
    return false;
  }

  hddaq::cout << "#D " << func_name << std::endl;

  sConfDir = hddaq::dirname(m_file[kConfFile]);

  std::string line;
  while( ifs.good() && std::getline( ifs, line ) ){
    if( line.empty() || line[0]=='#' ) continue;

    hddaq::replace_all(line, ",",  ""); // remove ,
    hddaq::replace_all(line, ":",  ""); // remove :
    hddaq::replace_all(line, "\"", ""); // remove "

    std::istringstream iss( line );
    std::istream_iterator<std::string> begin( iss );
    std::istream_iterator<std::string> end;
    std::vector<std::string> v( begin, end );
    if( v.size()<2 ) continue;

    std::string key = v[0];
    hddaq::cout << " key = "   << std::setw(10) << std::left << key
		<< " value = " << std::setw(30) << std::left << v[1]
		<< std::endl;

    m_file[key]   = FilePath(v[1]);
    m_string[key] = v[1];
    m_double[key] = hddaq::lexical_cast<double>(v[1]);
    m_int[key]    = hddaq::lexical_cast<int>(v[1]);
    m_bool[key]   = hddaq::lexical_cast<bool>(v[1]);
  }

  // if ( !InitializeParameterFiles() || !InitializeHistograms() )
  //  return false;

  if( gMatrix.IsReady() ){
    gMatrix.Print2D1();
    gMatrix.Print2D2();
  }
  if( gUser.IsReady() )
    gUser.Print();

  m_is_ready = true;
  return true;
}

//_____________________________________________________________________________
bool
ConfMan::Initialize( const std::string& file_name )
{
  m_file[kConfFile] = file_name;
  return Initialize();
}

//_____________________________________________________________________________
bool
ConfMan::Initialize( const std::vector<std::string>& argv )
{
  using namespace hddaq;
  using namespace hddaq::unpacker;
  static const std::string func_name("["+class_name+"::"+__func__+"()]");

  hddaq::cout << "#D " << func_name
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
  while( conf.good() ){
    std::string l;
    std::getline(conf, l);
    std::istringstream iss(l);
    std::istream_iterator<std::string> issBegin(iss);
    std::istream_iterator<std::string> issEnd;
    std::vector<std::string> param(issBegin, issEnd);
    if( param.empty() )
      continue;
    if( param[0].empty() )
      continue;
    if( param[0]=='#' )
      continue;
    if( param.size()==2 ){
      TString key   = param[0];
      TString value = param[1];
      key.ReplaceAll(":","");
      key.ReplaceAll(";","");
      key.ReplaceAll(" ","");
      if( value[0] != '/' && !value.IsFloat() )
	value = hddaq::realpath( std::string(dir+value) );

      hddaq::cout << " key = "   << std::setw(12) << std::left << key
		  << " value = " << value << std::endl;

      m_key_map[key]    = value;
      m_int_map[key]    = value.Atoi();
      m_double_map[key] = value.Atof();

      std::string value_str=value.Data();
      std::string key_str=key.Data();

      m_file[key_str]   = FilePath(value_str);
      m_string[key_str] = value;
      m_double[key_str] = hddaq::lexical_cast<double>(value);
      m_int[key_str]    = hddaq::lexical_cast<int>(value);
      m_bool[key_str]   = hddaq::lexical_cast<bool>(value);

    }
  }

  // initialize unpacker system
  TString key_unpacker = Contains( "UNPACK" ) ? "UNPACK" : "UNPACKER";
  gUnpacker.set_config_file( std::string(m_key_map[key_unpacker]),
			     std::string(m_key_map["DIGIT"]),
			     std::string(m_key_map["CMAP"]) );
  gUnpacker.set_istream( dataSrc );

  hddaq::cout << std::endl;

  // Initialize of ConfMan and Unpacker were done
  flag_.set(kIsGood);

  return true;
}

//_____________________________________________________________________________
bool
ConfMan::InitializeUnpacker( void )
{
  gUnpacker.set_config_file( m_file["UNPACK"],
			     m_file["DIGIT"],
			     m_file["CMAP"] );
  return true;
}

//_____________________________________________________________________________
bool
ConfMan::Finalize( void )
{
  //return FinalizeProcess();
  return true;
}

//_____________________________________________________________________________
std::string
ConfMan::FilePath( const std::string& src ) const
{
  std::ifstream tmp( src.c_str() );
  if ( tmp.good() )
    return src;
  else
    return sConfDir + "/" + src;
}
