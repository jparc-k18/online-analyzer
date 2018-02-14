// -*- C++ -*-

#ifndef HODO_PHC_MAN_H
#define HODO_PHC_MAN_H

#include <map>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class HodoPHCParam : public TObject
{
public:
  HodoPHCParam( int type, int np, double *parlist );
  ~HodoPHCParam();
private:
  HodoPHCParam( const HodoPHCParam &right );
  HodoPHCParam & operator = ( const HodoPHCParam & );
private:
  int Type, NPar;
  double *ParList;
public:
  double DoPHC( double time, double de );
  double DoRPHC( double time, double de );
private:
  double type1Correction( double time, double de );
  double type2Correction( double time, double de ); // For fiber
  double type1RCorrection( double time, double de );
};

//______________________________________________________________________________
class HodoPHCMan : public TObject
{
public:
  ~HodoPHCMan( void );
  static HodoPHCMan& GetInstance( void );

private:
  HodoPHCMan( void );
  HodoPHCMan( const HodoPHCMan& );
  HodoPHCMan& operator =( const HodoPHCMan& );

private:
  typedef std::map <int, HodoPHCParam *> PHCPContainer;
  typedef PHCPContainer::const_iterator  PHCPIterator;
  Bool_t        m_is_initialized;
  TString       m_file_name;
  PHCPContainer m_map;

public:
  void   SetFileName( const TString& file_name ) { m_file_name = file_name; }
  Bool_t Initialize( void );
  Bool_t Initialize( const TString& file_name ) { SetFileName(file_name); return Initialize(); }
  Bool_t IsReady( void ) const { return m_is_initialized; }
  Bool_t DoCorrection( int cid, int plid, int seg, int ud, double time,
                     double de, double &ctime ) const;
  Bool_t DoRCorrection( int cid, int plid, int seg, int ud, double time,
                      double de, double &ctime ) const;
private:
  HodoPHCParam * GetMap( int cid, int plid, int seg, int ud ) const;
  void clearMap( void );

  ClassDef(HodoPHCMan,0);
};

//______________________________________________________________________________
inline HodoPHCMan&
HodoPHCMan::GetInstance( void )
{
  static HodoPHCMan g_instance;
  return g_instance;
}

#endif
