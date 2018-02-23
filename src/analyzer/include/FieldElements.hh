// -*- C++ -*-

#ifndef FIELD_ELEMENTS_HH
#define FIELD_ELEMENTS_HH

#include <TObject.h>

#include "DCGeomRecord.hh"
#include "ThreeVector.hh"

//______________________________________________________________________________
class FieldElements : public TObject
{
public:
  FieldElements( const TString& name, const ThreeVector& pos,
                 Double_t ta, Double_t ra1, Double_t ra2 );
  virtual ~FieldElements( void );

private:
  enum FERegion { kFERSurface, kFERInside, kFEROutside };
  DCGeomRecord m_geom_record;

public:
  static FERegion FERSurface( void ) { return kFERSurface; }
  static FERegion FERInside( void )  { return kFERInside;  }
  static FERegion FEROutside( void ) { return kFEROutside; }
  ThreeVector     Local2GlobalPos( const ThreeVector &in ) const;
  ThreeVector     Local2GlobalDir( const ThreeVector &in ) const;
  ThreeVector     Global2LocalPos( const ThreeVector &in ) const;
  ThreeVector     Global2LocalDir( const ThreeVector &in ) const;

  virtual ThreeVector GetField( const ThreeVector &gPos ) const = 0;
  virtual Bool_t      ExistField( const ThreeVector &gPos ) const = 0;
  virtual FERegion    CheckRegion( const ThreeVector &gPos,
				   Double_t Tolerance ) const = 0;

  ClassDef(FieldElements,0);
};

#endif
