// -*- C++ -*-

#ifndef DC_DRIFT_PARAM_MAN_HH
#define DC_DRIFT_PARAM_MAN_HH

#include <map>
#include <string>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
struct DCDriftParamRecord
{
public:
  Int_t type, np;
  std::vector<Double_t> p;
};

//______________________________________________________________________________
class DCDriftParamMan : public TObject
{
public:
  static DCDriftParamMan& GetInstance( void );
  ~DCDriftParamMan( void );

private:
  DCDriftParamMan( void );
  DCDriftParamMan( const DCDriftParamMan& );
  DCDriftParamMan& operator =( const DCDriftParamMan& );

private:
  TString                               m_file_name;
  std::map<UInt_t, DCDriftParamRecord*> m_map;

public:
  Bool_t Initialize( void );
  void   SetFileName( const TString& file_name) { m_file_name = file_name; }
  Bool_t calcDrift( Int_t PlaneId, Double_t WireId, Double_t ctime,
		    Double_t & dt, Double_t & dl ) const;

private:
  DCDriftParamRecord* getParameter( Int_t PlaneId, Double_t WireId ) const;
  void clearElements( void );

  static Double_t DriftLength1( Double_t dt, Double_t vel );
  static Double_t DriftLength2( Double_t dt, Double_t p1, Double_t p2, Double_t p3,
			      Double_t st, Double_t p5, Double_t p6 );
  static Double_t DriftLength3( Double_t dt, Double_t p1, Double_t p2, Int_t PlaneId);
  static Double_t DriftLength4( Double_t dt, Double_t p1, Double_t p2, Double_t p3);
  static Double_t DriftLength5( Double_t dt, Double_t p1, Double_t p2, Double_t p3, Double_t p4, Double_t p5);
  static Double_t DriftLength6( Int_t PlaneId, Double_t dt, Double_t p1, Double_t p2 ,Double_t p3, Double_t p4, Double_t p5);

  ClassDef(DCDriftParamMan,0);
};

//______________________________________________________________________________
inline DCDriftParamMan&
DCDriftParamMan::GetInstance( void )
{
  static DCDriftParamMan g_instance;
  return g_instance;
}

#endif
