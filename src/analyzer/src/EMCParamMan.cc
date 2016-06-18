/*
 */

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>

#include "EMCParamMan.hh"
#include "ConfMan.hh"
#include "DetectorID.hh"

static const std::string class_name("EMCParamMan");

// initialize EMCParamMan --------------------------------------------------
void
ConfMan::initializeEMCParamMan( void )
{
  if( ( name_file_["EMC:"] != "" ) ){
    EMCParamMan& gEMCParam = EMCParamMan::GetInstance();
    flag_[kIsGood] = gEMCParam.Initialize( name_file_["EMC:"] );
  }else{
    std::cout << "#E ConfMan::initializeEMCParamMan()"
	      << " File path does not exist in " << name_file_["EMC:"]
	      << std::endl;
    flag_.reset( kIsGood );
  }
}
// initialize EMCParamMan --------------------------------------------------

//_____________________________________________________________________
EMCParamMan::EMCParamMan( void )
  : m_nspill(0)
{
}

//_____________________________________________________________________
EMCParamMan::~EMCParamMan( void )
{
}

//_____________________________________________________________________
bool
EMCParamMan::Initialize( const std::string& filename )
{
  static const std::string func_name("["+class_name+"::"+__func__+"()]");

  std::ifstream ifs( filename.c_str() );
  if( !ifs.is_open() ){
    std::cerr << "#E " << func_name << " "
	      << "no such parameter file : " << filename << std::endl;
    std::exit( EXIT_FAILURE );
  }

  m_nspill = 0;
  m_spill.clear();
  m_x.clear();
  m_y.clear();

  std::string line;
  while( !ifs.eof() && std::getline( ifs, line ) ){
    if( line.empty() ) continue;

    std::string param[3];
    std::istringstream iss( line );
    iss >> param[0] >> param[1] >> param[2];

    if( param[0].at(0) == '#' ) continue;
    
    double spill = std::strtod( param[0].c_str(), NULL );
    double x     = std::strtod( param[1].c_str(), NULL );
    double y     = std::strtod( param[2].c_str(), NULL );
    m_spill.push_back( spill );
    m_x.push_back( x );
    m_y.push_back( y );
    m_nspill++;
  }
  std::cout << std::endl;

#if 0
  {
    std::cout << "#D " << func_name << " "
	      << "EMC Parameter" << std::endl;
    for( int i=0; i<m_nspill; ++i ){
      std::cout << m_spill[i] << "\t" << m_x[i] << "\t" << m_y[i] << std::endl;
    }
  }
#endif

  std::cout << "#D " << func_name << " "
	    << "Initialized"
	    << std::endl;

  return true;
}

//_____________________________________________________________________
int
EMCParamMan::Pos2Spill( double x, double y )
{
  for( int i=0; i<m_nspill; ++i ){
    if( abs( m_x[i] - x ) < 1000. &&
	abs( m_y[i] - y ) < 1000. ){
      return m_spill[i];
    }
  }
  return -1;
}
