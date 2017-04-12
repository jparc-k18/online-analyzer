/*
  GeAdcCalibMan.cc
*/

#include "GeAdcCalibMan.hh"
#include "ConfMan.hh"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cstdlib>

// initialize GeAdcCalibMan ----------------------------------------------
void ConfMan::initializeGeAdcCalibMan()
{
  if(name_file_["GEADC:"] != ""){
    GeAdcCalibMan& gGeAdcCalib = GeAdcCalibMan::GetInstance();
    flag_[kIsGood] = gGeAdcCalib.Initialize(name_file_["GEADC:"]);
  }else{
    std::cout << "#E ConfMan"
	      << " File path does not exist in " << name_file_["GEADC:"] 
	      << std::endl;
    flag_.reset(kIsGood);
  }
}
// initialize GeAdcCalibMan ----------------------------------------------

#define frand() (-0.5+(double) rand() / RAND_MAX)

const int BufSize = 200;

//Adc1,Adc2,Adc3,Adc4,Adc5
struct GeAdcCalibMap {
  GeAdcCalibMap( double q0, double q1, double q2, double q3, double q4 )
    : p0(q0), p1(q1), p2(q2), p3(q3), p4(q4)
  {}
  double p0, p1, p2, p3, p4; 
};

GeAdcCalibMan::GeAdcCalibMan()
{
  
}

GeAdcCalibMan::~GeAdcCalibMan()
{
  clearElements();
}

void GeAdcCalibMan::clearElements( void )
{
  std::map <unsigned int, GeAdcCalibMap *>::iterator itr;
  for( itr=Cont_.begin(); itr!=Cont_.end(); ++itr ){
    delete itr->second;
    itr->second = 0;
  }
  Cont_.clear();
}

inline unsigned int SortKey( int seg )
{
  return seg;
}


bool GeAdcCalibMan::Initialize( const std::string& filename)
{
  static const std::string funcname = "[GeAdcCalibMan::Initialize]";

  ParamFileName_ = filename;

  int seg;
  double p0, p1, p2 ,p3, p4;
  FILE *fp;
  char buf[BufSize];

  if((fp=fopen(ParamFileName_.c_str(),"r"))==0){
    std::cerr << funcname << ": file open fail" << std::endl;
    exit(-1);
  }

  while( fgets( buf, BufSize, fp ) ){
    if( buf[0]!='#' ){
      if( sscanf( buf, "%d %lf %lf %lf %lf %lf",
		  &seg, &p0, &p1, &p2, &p3, &p4 )==6 ){
	unsigned int key = SortKey( seg );
	GeAdcCalibMap *p=new GeAdcCalibMap( p0, p1, p2, p3, p4 );
	if(p){
	  if( Cont_[key] ) delete Cont_[key];
	  Cont_[key]=p;
	}
	else{
	  std::cerr << funcname << ": new fail. "
		    << " Seg=" << std::setw(3) << std::dec << seg 
		    << std::endl;
	}
      }
      else{
	std::cerr << funcname << ": Bad format => "
		  << std::string(buf) << std::endl;
      }
    }
  }
  fclose(fp);

  std::cout << funcname << ": Initialization finished" << std::endl;
  return true;
}

GeAdcCalibMap * GeAdcCalibMan::getMap( int seg ) const
{
  unsigned int key=SortKey( seg );
  return Cont_[key];
}  

double GeAdcCalibMan::CalibAdc( int seg, int adc, double &a_ ) 
{
  static const std::string funcname = "[GeAdcCalibMan::CalibAdc]";
  GeAdcCalibMap *p=getMap( seg );
  if(p){
    double adcCH=adc+frand();
    if( adc>0 ){
      a_=(p->p0)
	+(p->p1)*adcCH
	+(p->p2)*adcCH*adcCH
	+(p->p3)*adcCH*adcCH*adcCH
	+(p->p4)*adcCH*adcCH*adcCH*adcCH;
    }
    return true;
  }
  else{
    std::cerr << funcname << ": No record. "
	      << " Seg=" << std::setw(3) << std::dec << seg
    	      << std::endl;
    return -1;
  }
}
