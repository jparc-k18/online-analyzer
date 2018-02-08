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
void
ConfMan::InitializeMsTParamMan( void )
{
  if( ( name_file_["MASS:"] != "" ) ){
    MsTParamMan& gMsTParam = MsTParamMan::GetInstance();
    flag_[kIsGood] = gMsTParam.Initialize( name_file_["MASS:"] );
  }else{
    std::cout << "#E ConfMan::"
	      << " File path does not exist in " << name_file_["MASS:"]
	      << std::endl;
    flag_.reset(kIsGood);
  }
}

//______________________________________________________________________________
MsTParamMan::MsTParamMan( void )
  : TObject()
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
      // std::cerr << "#E " << FUNC_NAME << " "
      // 		<< "No such parameter file : " << filename << std::endl;
      std::exit( EXIT_FAILURE );
    }

    const Int_t NumOfSegDetA = NumOfSegTOF+2;
    const Int_t NumOfSegDetB = NumOfSegSCH;

    m_low_threshold.resize( NumOfSegDetA );
    m_high_threshold.resize( NumOfSegDetA );
    for( Int_t i=0; i<NumOfSegDetA; ++i ){
      m_low_threshold[i].resize( NumOfSegDetB );
      m_high_threshold[i].resize( NumOfSegDetB );
    }

    std::string line;
    Int_t tofseg = 0;
    Int_t LorH   = 0;
    while( !ifs.eof() && std::getline( ifs, line ) ){
      if( line.empty() ) continue;

      std::string param[NumOfSegDetB];
      std::istringstream iss( line );

      for( Int_t i=0; i<NumOfSegDetB; ++i ){
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

      if( param[NumOfSegDetB-1].empty() ) continue;
      for( Int_t i=0; i<NumOfSegDetB; ++i ){
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

#if 1
    // Low
    std::cout << "Low Threshold" << std::endl;
    for( Int_t i=0; i<NumOfSegDetA; ++i ){
      std::cout << std::setw(2) << i << " :";
      for( Int_t j=0; j<NumOfSegDetB; ++j ){
	std::cout << " " << m_low_threshold[i][j];
      }
      std::cout << std::endl;
    }
    // High
    std::cout << "High Threshold" << std::endl;
    for( Int_t i=0; i<NumOfSegDetA; ++i ){
      std::cout << std::setw(2) << i << " :";
      for( Int_t j=0; j<NumOfSegDetB; ++j ){
	std::cout << " " << m_high_threshold[i][j];
      }
      std::cout << std::endl;
    }
#endif
  }

  // std::cout << "#D " << FUNC_NAME << " "
  // 	    << "Initialized"
  // 	    << std::endl;

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
