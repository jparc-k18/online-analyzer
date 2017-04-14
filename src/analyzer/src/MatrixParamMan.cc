/*
 */

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>

#include "MatrixParamMan.hh"
#include "ConfMan.hh"
#include "DetectorID.hh"

ClassImp(MatrixParamMan)

static const std::string class_name("MatrixParamMan");

//______________________________________________________________________________
void
ConfMan::initializeMatrixParamMan( void )
{
  if( ( name_file_["MATRIX2D:"] != "" ) && ( name_file_["MATRIX3D:"] != "" ) ){
    MatrixParamMan& gMatrixParam = MatrixParamMan::GetInstance();
    flag_[kIsGood] = gMatrixParam.Initialize( name_file_["MATRIX2D:"],
					      name_file_["MATRIX3D:"] );
  }else{
    std::cout << "#E ConfMan::"
	      << " File path does not exist in " << name_file_["MATRIX2D:"]
	      << " or " << name_file_["MATRIX3D:"] << std::endl;
    flag_.reset(kIsGood);
  }
}

//_____________________________________________________________________
MatrixParamMan::MatrixParamMan( void )
{
}

//_____________________________________________________________________
MatrixParamMan::~MatrixParamMan( void )
{
}

//_____________________________________________________________________
bool
MatrixParamMan::Initialize( const std::string& filename_2d,
			    const std::string& filename_3d )
{
  static const std::string func_name("["+class_name+"::"+__func__+"()]");

  {
    std::ifstream ifs2d( filename_2d.c_str() );
    if( !ifs2d.is_open() ){
      std::cerr << "#E " << func_name << " "
		<< "No such parameter file : " << filename_2d << std::endl;
      std::exit( EXIT_FAILURE );
    }

    m_enable_2d.resize( NumOfSegTOF );
    for( int i=0; i<NumOfSegTOF; ++i ){
      m_enable_2d.at(i).resize( NumOfSegSCH );
    }

    std::string line;
    int tofseg = 0;
    while( !ifs2d.eof() && std::getline( ifs2d, line ) ){
      if( line.empty() ) continue;

      std::string param[2];
      std::istringstream iss( line );
      iss >> param[0] >> param[1];

      if( param[0].at(0) == '#' ) continue;

      if( param[0].substr(2).at(0)=='0' )
	param[0] = param[0].substr(3);
      else
	param[0] = param[0].substr(2);

      int channel = std::strtol( param[0].c_str(), NULL, 0 );
      int enable  = std::strtol( param[1].substr(0,1).c_str(), NULL, 0 );

      m_enable_2d[tofseg][channel] = enable;
      if( channel==NumOfSegSCH-1 ) ++tofseg;
    }

#if 1
    std::cout << "#D " << func_name << " "
              << "2D Parameter" << std::endl;
    for( int i=NumOfSegTOF-1; i>=0; --i ){
      std::cout << std::setw(2) << i << " :";
      for( int j=0; j<NumOfSegSCH; ++j ){
	std::cout << m_enable_2d[i][j];
      }
      std::cout << std::endl;
    }
#endif
  }

  {
    std::ifstream ifs3d( filename_3d.c_str() );
    if( !ifs3d.is_open() ){
      std::cerr << "#E " << func_name << " "
		<< "No such parameter file : " << filename_3d << std::endl;
      std::exit( EXIT_FAILURE );
    }

    m_enable_3d.resize( NumOfSegTOF );
    for( int i=0; i<NumOfSegTOF; ++i ){
      m_enable_3d[i].resize( NumOfSegSCH );
      for( int j=0; j<NumOfSegSCH; ++j ){
	m_enable_3d[i][j].resize( NumOfSegClusteredFBH );
      }
    }

    std::string line;
    int tofseg = 0;
    while( !ifs3d.eof() && std::getline( ifs3d, line ) ){
      if( line.empty() ) continue;

      std::string param[2];
      std::istringstream iss( line );
      iss >> param[0] >> param[1];

      if( param[0].at(0) == '#' ) continue;

      if( param[0].substr(2).at(0)=='0' )
	param[0] = param[0].substr(3);
      else
	param[0] = param[0].substr(2);

      int channel = std::strtol( param[0].c_str(), NULL, 0 );
      int enable[NumOfSegClusteredFBH] = {};
      for( int i=0; i<NumOfSegClusteredFBH; ++i ){
	enable[i] = std::strtol( param[1].substr(i,1).c_str(), NULL, 0 );
	m_enable_3d[tofseg][channel][i] = enable[i];
      }

      if( channel==NumOfSegSCH-1 ) ++tofseg;
    }

#if 0
    std::cout << "#D " << func_name << " "
              << "3D Parameter" << std::endl;
    for( int k=0; k<NumOfSegClusteredFBH; ++k ){
      std::cout << "Clustered FBH : " << k << std::endl;
      for( int i=NumOfSegTOF-1; i>=0; --i ){
	std::cout << std::setw(2) << i << " :";
	for( int j=0; j<NumOfSegSCH; ++j ){
	  std::cout << m_enable_3d[i][j][k];
	}
	std::cout << std::endl;
      }
    }
#endif
  }

  std::cout << "#D " << func_name << " "
	    << "Initialized"
	    << std::endl;

  return true;
}

//_____________________________________________________________________
bool
MatrixParamMan::IsAccept( int detA, int detB )
{
  int enable = m_enable_2d.at( detA ).at( detB );
  return ( enable == 1 );
}

//_____________________________________________________________________
bool
MatrixParamMan::IsAccept( int detA, int detB, int detC )
{
  int enable = m_enable_3d.at( detA ).at( detB ).at( detC );
  return ( enable == 1 );
}
