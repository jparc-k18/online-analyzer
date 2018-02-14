// -*- C++ -*-

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>

#include "ConfMan.hh"
#include "DetectorID.hh"
#include "EMCParamMan.hh"
#include "FuncName.hh"

ClassImp(EMCParamMan);

//______________________________________________________________________________
EMCParamMan::EMCParamMan( void )
  : m_nspill(0)
{
}

//______________________________________________________________________________
EMCParamMan::~EMCParamMan( void )
{
}

//______________________________________________________________________________
bool
EMCParamMan::Initialize( const TString& filename )
{
  std::ifstream ifs( filename );
  if( !ifs.is_open() ){
    std::cerr << "#E " << FUNC_NAME << " "
	      << "no such parameter file : " << filename << std::endl;
    std::exit( EXIT_FAILURE );
  }

  m_nspill = 0;
  m_spill.clear();
  m_x.clear();
  m_y.clear();

  TString line;
  while( ifs.good() && line.ReadLine(ifs) ){
    if( line[0] == '#' ) continue;

    TString param[3];
    std::istringstream iss( line.Data() );
    iss >> param[0] >> param[1] >> param[2];

    double spill = param[0].Atof();
    double x     = param[1].Atof();
    double y     = param[2].Atof();
    m_spill.push_back( spill );
    m_x.push_back( x );
    m_y.push_back( y );
    m_nspill = spill;
  }
  std::cout << std::endl;

#if 0
  {
    std::cout << "#D " << FUNC_NAME << " "
	      << "EMC Parameter" << std::endl;
    for( int i=0; i<m_nspill; ++i ){
      std::cout << m_spill[i] << "\t" << m_x[i] << "\t" << m_y[i] << std::endl;
    }
  }
#endif

  std::cout << "#D " << FUNC_NAME << " "
	    << "Initialized"
	    << std::endl;

  return true;
}

//______________________________________________________________________________
int
EMCParamMan::Pos2Spill( double x, double y )
{
  for( int i=0; i<m_nspill; ++i ){
    if( TMath::Abs( m_x[i] - x ) < 1000. &&
	TMath::Abs( m_y[i] - y ) < 1000. ){
      return m_spill[i];
    }
  }
  return -1;
}
