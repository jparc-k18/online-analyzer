// -*- C++ -*-

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "MsTParamMan.hh"
#include "ConfMan.hh"
#include "DetectorID.hh"
#include "FuncName.hh"

ClassImp(MsTParamMan);

//______________________________________________________________________________
MsTParamMan::MsTParamMan( void )
  : TObject(),
    m_nseg_a( NumOfSegTOF+2 ),
    m_nseg_b( NumOfSegSCH ),
    m_low_threshold(),
    m_high_threshold()
{
}

//______________________________________________________________________________
MsTParamMan::~MsTParamMan( void )
{
}

//______________________________________________________________________________
Bool_t
MsTParamMan::Initialize( const TString& filename )
{
  {
    std::ifstream ifs( filename );
    if( !ifs.is_open() ){
      hddaq::cerr << "#E " << FUNC_NAME << " "
		  << "No such parameter file : " << filename << std::endl;
      std::exit( EXIT_FAILURE );
    }

    m_low_threshold.resize( m_nseg_a );
    m_high_threshold.resize( m_nseg_a );
    for( Int_t i=0; i<m_nseg_a; ++i ){
      m_low_threshold[i].resize( m_nseg_b );
      m_high_threshold[i].resize( m_nseg_b );
    }

    std::string line;
    Int_t tofseg = 0;
    Int_t LorH   = 0;
    while( !ifs.eof() && std::getline( ifs, line ) ){
      if( line.empty() ) continue;

      std::string param[m_nseg_b];
      std::istringstream iss( line );

      for( Int_t i=0; i<m_nseg_b; ++i ){
	iss >> param[i];
      }
      if( param[0].at(0) == '#' ) continue;
      if( param[1] == "Mem_Thr_Low" || param[0]=="Low" || param[1]=="Low" ){
	tofseg =  0;
	LorH   = -1;
	continue;
      }
      if( param[1] == "Mem_Thr_Hi" || param[0]=="Hi" || param[1]=="Hi" ){
	tofseg =  0;
	LorH   =  1;
	continue;
      }

      if( param[m_nseg_b-1].empty() ) continue;
      for( Int_t i=0; i<m_nseg_b; ++i ){
	std::replace( param[i].begin(), param[i].end(), '(', ' ');
	std::replace( param[i].begin(), param[i].end(), ')', ' ');
	std::replace( param[i].begin(), param[i].end(), ',', ' ');
	if( LorH == -1 )
	  m_low_threshold[tofseg][i] = std::strtol( param[i].c_str(), NULL, 0 );
	if( LorH ==  1 )
	  m_high_threshold[tofseg][i] = std::strtol( param[i].c_str(), NULL, 0 );
      }
      ++tofseg;
    }
  }

  return true;
}

//______________________________________________________________________________
Bool_t
MsTParamMan::IsAccept( Int_t detA, Int_t detB, Int_t tdc )
{
  // for FERA
  Int_t cdetA = detA>=13 ? detA + 2 : detA;

  Int_t low  = m_low_threshold[cdetA][detB];
  Int_t high = m_high_threshold[cdetA][detB];
  return ( low < tdc && tdc < high );
}

//______________________________________________________________________________
void
MsTParamMan::Print( Option_t* ) const
{
  hddaq::cout << FUNC_NAME << std::endl;
  // Low
  hddaq::cout << "Low Threshold" << std::endl;
  for( Int_t i=0; i<m_nseg_a; ++i ){
    hddaq::cout << std::setw(2) << i << " :";
    for( Int_t j=0; j<m_nseg_b; ++j ){
      hddaq::cout << " " << m_low_threshold[i][j];
    }
    hddaq::cout << std::endl;
  }
  // High
  hddaq::cout << "High Threshold" << std::endl;
  for( Int_t i=0; i<m_nseg_a; ++i ){
    hddaq::cout << std::setw(2) << i << " :";
    for( Int_t j=0; j<m_nseg_b; ++j ){
      hddaq::cout << " " << m_high_threshold[i][j];
    }
    hddaq::cout << std::endl;
  }
}
