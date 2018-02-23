// -*- C++ -*-

#ifndef FIELD_MAN_HH
#define FIELD_MAN_HH

#include <string>
#include <vector>

#include <TObject.h>

#include "ThreeVector.hh"

class KuramaFieldMap;
class FieldElements;

//______________________________________________________________________________
class FieldMan : public TObject
{
public:
  static FieldMan& GetInstance( void );
  ~FieldMan( void );

private:
  FieldMan( void );
  FieldMan( const FieldMan& );
  FieldMan & operator =( const FieldMan& );

private:
  typedef std::vector<FieldElements*> FEContainer;
  typedef FEContainer::const_iterator FEIterator;

  Bool_t          m_is_ready;
  TString         m_file_name;
  KuramaFieldMap* m_kurama_map;
  FEContainer     m_element_list;

public:
  Bool_t      Initialize( void );
  Bool_t      Initialize( const TString& file_name );
  Bool_t      IsReady( void ) const { return m_is_ready; }
  ThreeVector GetField( const ThreeVector& position ) const;
  ThreeVector GetdBdX( const ThreeVector& position ) const;
  ThreeVector GetdBdY( const ThreeVector& position ) const;
  ThreeVector GetdBdZ( const ThreeVector& position ) const;
  void        ClearElementsList( void );
  void        AddElement( FieldElements* element );
  void        SetFileName( const TString& file_name ) { m_file_name = file_name; }
  Double_t    StepSize( const ThreeVector& position,
			Double_t default_step_size,
			Double_t min_step_size ) const;

  ClassDef(FieldMan,0);
};

//______________________________________________________________________________
inline FieldMan&
FieldMan::GetInstance( void )
{
  static FieldMan g_instance;
  return g_instance;
}

#endif
