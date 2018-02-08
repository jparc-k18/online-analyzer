// -*- C++ -*-

#ifndef HODO_PARAM_MAN_H
#define HODO_PARAM_MAN_H

#include <string>
#include <map>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class HodoTParam //Hodo TDC to Time
{
public:
  inline HodoTParam( Double_t offset, Double_t gain )
  : Offset(offset), Gain(gain)
  {}

  ~HodoTParam() {}
private:
  HodoTParam();
  HodoTParam( const HodoTParam & );
  HodoTParam & operator = ( const HodoTParam & );

private:
  Double_t Offset, Gain;
public:
  Double_t offset( void ) const { return Offset; }
  Double_t gain( void )   const { return Gain; }
  Double_t time( Int_t tdc ) const
  { return ((Double_t)tdc-Offset)*Gain; }
  inline Int_t tdc( Double_t time ) const
  { return (Int_t)(time/Gain+Offset); }
};

//______________________________________________________________________________
class HodoAParam //Hodo ADC to DeltaE
{
public:
  inline HodoAParam( Double_t pedestal, Double_t gain )
  : Pedestal(pedestal), Gain(gain)
  {}
  ~HodoAParam() {}
private:
  HodoAParam();
  HodoAParam( const HodoAParam & );
  HodoAParam & operator = ( const HodoAParam & );
private:
  Double_t Pedestal, Gain;
public:
  Double_t pedestal( void ) const { return Pedestal; }
  Double_t gain( void )     const { return Gain; }
  Double_t de( Int_t adc ) const
  { return ((Double_t)adc-Pedestal)/(Gain-Pedestal); }
  Int_t adc( Double_t de ) const
  { return (Int_t)(Gain*de+Pedestal*(1.-de)); }
};

//______________________________________________________________________________
class HodoParamMan : public virtual TObject //HodoParam Main Class
{
public:
  static HodoParamMan& GetInstance( void );
  ~HodoParamMan( void );

private:
  HodoParamMan( void );
  HodoParamMan( const HodoParamMan& );
  HodoParamMan& operator =( const HodoParamMan& );

private:
  typedef std::map <Int_t, HodoTParam*> TContainer;
  typedef std::map <Int_t, HodoAParam*> AContainer;
  typedef std::map <Int_t, HodoTParam*>::const_iterator TIterator;
  typedef std::map <Int_t, HodoAParam*>::const_iterator AIterator;

  TString    m_file_name;
  TContainer TPContainer;
  AContainer APContainer;

public:
  void   SetFileName( const TString& filename ) { m_file_name = filename; }
  Bool_t Initialize( void );
  Bool_t GetTime( Int_t cid, Int_t plid, Int_t seg, Int_t ud, Int_t tdc, Double_t &time ) const;
  Bool_t GetDe( Int_t cid, Int_t plid, Int_t seg, Int_t ud, Int_t adc, Double_t &de ) const;
  Bool_t GetTdc(Int_t cid, Int_t plid, Int_t seg, Int_t ud, Double_t time, Int_t &tdc ) const;
  Bool_t GetAdc(Int_t cid, Int_t plid, Int_t seg, Int_t ud, Double_t de, Int_t &adc ) const;

private:
  HodoTParam* GetTmap( Int_t cid, Int_t plid, Int_t seg, Int_t ud ) const;
  HodoAParam* GetAmap( Int_t cid, Int_t plid, Int_t seg, Int_t ud ) const;
  void        clearACont( void );
  void        clearTCont( void );

  ClassDef(HodoParamMan,0);
};

//______________________________________________________________________________
inline HodoParamMan&
HodoParamMan::GetInstance( void )
{
  static HodoParamMan g_instance;
  return g_instance;
}

#endif
