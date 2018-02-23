// -*- C++ -*-

#ifndef RUNGE_KUTTA_UTILITIES_HH
#define RUNGE_KUTTA_UTILITIES_HH

#include <vector>
#include <utility>
#include <iosfwd>

#include <TObject.h>

#include "ThreeVector.hh"

class RKFieldIntegral;
class RKDeltaFieldIntegral;
class RKTrajectoryPoint;
class RKcalcHitPoint;
class RKCordParameter;
class RKHitPointContainer;

//______________________________________________________________________________
class RungeKutta : public TObject
{
public:
  //______________________________________________________________________________
  static RKFieldIntegral
  CalcFieldIntegral( Double_t U, Double_t V, Double_t Q, const ThreeVector& B );
  //______________________________________________________________________________
  static RKFieldIntegral
  CalcFieldIntegral( Double_t U, Double_t V, Double_t Q,
		     const ThreeVector& B,
		     const ThreeVector& dBdX, const ThreeVector& dBdY );
  //______________________________________________________________________________
  static RKDeltaFieldIntegral
  CalcDeltaFieldIntegral( const RKTrajectoryPoint& prevPoint,
			  const RKFieldIntegral& intg );
  //______________________________________________________________________________
  static RKDeltaFieldIntegral
  CalcDeltaFieldIntegral( const RKTrajectoryPoint& prevPoint,
			  const RKFieldIntegral& intg,
			  const RKDeltaFieldIntegral& dIntg1,
			  const RKDeltaFieldIntegral& dIntg2, Double_t StepSize );
  //______________________________________________________________________________
  static Bool_t
  CheckCrossing( Int_t lnum, const RKTrajectoryPoint& startPoint,
		 const RKTrajectoryPoint& endPoint, RKcalcHitPoint& crossPoint );
  //______________________________________________________________________________
  static Int_t
  Trace( const RKCordParameter& initial, RKHitPointContainer& hitContainer );
  //______________________________________________________________________________
  static RKTrajectoryPoint
  TraceOneStep( Double_t StepSize, const RKTrajectoryPoint& prevPoint );
  //______________________________________________________________________________
  static Bool_t
  TraceToLast( RKHitPointContainer& hitContainer );
  //______________________________________________________________________________
  static RKHitPointContainer
  MakeHPContainer( void );

  ClassDef(RungeKutta,0);
};

//______________________________________________________________________________
class RKFieldIntegral
{
public:
  RKFieldIntegral( Double_t Kx, Double_t Ky,
		   Double_t Axu, Double_t Axv, Double_t Ayu, Double_t Ayv,
		   Double_t Cxx=0., Double_t Cxy=0.,
		   Double_t Cyx=0., Double_t Cyy=0. )
    : kx(Kx), ky(Ky), axu(Axu), axv(Axv), ayu(Ayu), ayv(Ayv),
      cxx(Cxx), cxy(Cxy), cyx(Cyx), cyy(Cyy)
  {
  }

private:
  Double_t kx, ky;
  Double_t axu, axv, ayu, ayv;
  Double_t cxx, cxy, cyx, cyy;

public:
  void Print( std::ostream& ost ) const;

  friend class RungeKutta;
};

//______________________________________________________________________________
class RKDeltaFieldIntegral
{
public:
  RKDeltaFieldIntegral( Double_t dKxx, Double_t dKxy, Double_t dKxu,
			Double_t dKxv, Double_t dKxq,
			Double_t dKyx, Double_t dKyy, Double_t dKyu,
			Double_t dKyv, Double_t dKyq )
    : dkxx(dKxx), dkxy(dKxy), dkxu(dKxu), dkxv(dKxv), dkxq(dKxq),
      dkyx(dKyx), dkyy(dKyy), dkyu(dKyu), dkyv(dKyv), dkyq(dKyq)
  {}

private:
  Double_t dkxx, dkxy, dkxu, dkxv, dkxq;
  Double_t dkyx, dkyy, dkyu, dkyv, dkyq;
public:
  void Print( std::ostream& ost ) const;

  friend class RungeKutta;
};

//______________________________________________________________________________
class RKCordParameter
{
public:
  RKCordParameter( void )
    : x(0.), y(0.), z(0.), u(0.), v(0.), q(0.)
  {}
  RKCordParameter( Double_t X, Double_t Y, Double_t Z,
                   Double_t U, Double_t V, Double_t Q )
    : x(X), y(Y), z(Z), u(U), v(V), q(Q)
  {}

  RKCordParameter( const ThreeVector& pos,
                   Double_t U, Double_t V, Double_t Q )
    : x(pos.x()), y(pos.y()), z(pos.z()),
      u(U), v(V), q(Q)
  {}

  RKCordParameter( const ThreeVector& pos,
                   const ThreeVector& mom );
private:
  Double_t x, y, z, u, v, q;
public:
  ThreeVector PositionInGlobal( void ) const
  { return ThreeVector( x, y, z ); }
  ThreeVector MomentumInGlobal( void ) const;
  void Print( std::ostream& ost ) const;

  Double_t X( void ) const { return x; }
  Double_t Y( void ) const { return y; }
  Double_t Z( void ) const { return z; }
  Double_t U( void ) const { return u; }
  Double_t V( void ) const { return v; }
  Double_t Q( void ) const { return q; }

  friend class RKTrajectoryPoint;
  friend class RungeKutta;
};

//______________________________________________________________________________
class RKcalcHitPoint
{
public:
  RKcalcHitPoint( void ){}
  RKcalcHitPoint( const ThreeVector& pos, const ThreeVector& mom,
                  Double_t S, Double_t L,
                  Double_t Dsdx,  Double_t Dsdy,  Double_t Dsdu,  Double_t Dsdv,  Double_t Dsdq,
                  Double_t Dsdxx, Double_t Dsdxy, Double_t Dsdxu, Double_t Dsdxv, Double_t Dsdxq,
                  Double_t Dsdyx, Double_t Dsdyy, Double_t Dsdyu, Double_t Dsdyv, Double_t Dsdyq,
                  Double_t Dsdux, Double_t Dsduy, Double_t Dsduu, Double_t Dsduv, Double_t Dsduq,
                  Double_t Dsdvx, Double_t Dsdvy, Double_t Dsdvu, Double_t Dsdvv, Double_t Dsdvq,
                  Double_t Dsdqx, Double_t Dsdqy, Double_t Dsdqu, Double_t Dsdqv, Double_t Dsdqq,
                  Double_t Dxdx,  Double_t Dxdy,  Double_t Dxdu,  Double_t Dxdv,  Double_t Dxdq,
                  Double_t Dydx,  Double_t Dydy,  Double_t Dydu,  Double_t Dydv,  Double_t Dydq,
                  Double_t Dudx,  Double_t Dudy,  Double_t Dudu,  Double_t Dudv,  Double_t Dudq,
                  Double_t Dvdx,  Double_t Dvdy,  Double_t Dvdu,  Double_t Dvdv,  Double_t Dvdq )
    : posG(pos), momG(mom), s(S), l(L),
      dsdx(Dsdx),   dsdy(Dsdy),   dsdu(Dsdu),   dsdv(Dsdv),   dsdq(Dsdq),
      dsdxx(Dsdxx), dsdxy(Dsdxy), dsdxu(Dsdxu), dsdxv(Dsdxv), dsdxq(Dsdxq),
      dsdyx(Dsdyx), dsdyy(Dsdyy), dsdyu(Dsdyu), dsdyv(Dsdyv), dsdyq(Dsdyq),
      dsdux(Dsdux), dsduy(Dsduy), dsduu(Dsduu), dsduv(Dsduv), dsduq(Dsduq),
      dsdvx(Dsdvx), dsdvy(Dsdvy), dsdvu(Dsdvu), dsdvv(Dsdvv), dsdvq(Dsdvq),
      dsdqx(Dsdqx), dsdqy(Dsdqy), dsdqu(Dsdqu), dsdqv(Dsdqv), dsdqq(Dsdqq),
      dxdx(Dxdx),   dxdy(Dxdy),   dxdu(Dxdu),   dxdv(Dxdv),   dxdq(Dxdq),
      dydx(Dydx),   dydy(Dydy),   dydu(Dydu),   dydv(Dydv),   dydq(Dydq),
      dudx(Dudx),   dudy(Dudy),   dudu(Dudu),   dudv(Dudv),   dudq(Dudq),
      dvdx(Dvdx),   dvdy(Dvdy),   dvdu(Dvdu),   dvdv(Dvdv),   dvdq(Dvdq)
  {}

private:
  ThreeVector posG, momG;
  Double_t s; // local X
  Double_t l;
  Double_t dsdx,  dsdy,  dsdu,  dsdv,  dsdq;
  Double_t dsdxx, dsdxy, dsdxu, dsdxv, dsdxq;
  Double_t dsdyx, dsdyy, dsdyu, dsdyv, dsdyq;
  Double_t dsdux, dsduy, dsduu, dsduv, dsduq;
  Double_t dsdvx, dsdvy, dsdvu, dsdvv, dsdvq;
  Double_t dsdqx, dsdqy, dsdqu, dsdqv, dsdqq;
  Double_t dxdx,  dxdy,  dxdu,  dxdv,  dxdq;
  Double_t dydx,  dydy,  dydu,  dydv,  dydq;
  Double_t dudx,  dudy,  dudu,  dudv,  dudq;
  Double_t dvdx,  dvdy,  dvdu,  dvdv,  dvdq;

public:
  const ThreeVector& PositionInGlobal( void ) const { return posG; }
  const ThreeVector& MomentumInGlobal( void ) const { return momG; }
  Double_t PositionInLocal( void ) const { return s; }
  Double_t PathLength( void ) const { return l; }
  Double_t coefX( void ) const { return dsdx; }
  Double_t coefY( void ) const { return dsdy; }
  Double_t coefU( void ) const { return dsdu; }
  Double_t coefV( void ) const { return dsdv; }
  Double_t coefQ( void ) const { return dsdq; }
  Double_t coefXX( void ) const { return dsdxx; }
  Double_t coefXY( void ) const { return dsdxy; }
  Double_t coefXU( void ) const { return dsdxu; }
  Double_t coefXV( void ) const { return dsdxv; }
  Double_t coefXQ( void ) const { return dsdxq; }
  Double_t coefYX( void ) const { return dsdyx; }
  Double_t coefYY( void ) const { return dsdyy; }
  Double_t coefYU( void ) const { return dsdyu; }
  Double_t coefYV( void ) const { return dsdyv; }
  Double_t coefYQ( void ) const { return dsdyq; }
  Double_t coefUX( void ) const { return dsdux; }
  Double_t coefUY( void ) const { return dsduy; }
  Double_t coefUU( void ) const { return dsduu; }
  Double_t coefUV( void ) const { return dsduv; }
  Double_t coefUQ( void ) const { return dsduq; }
  Double_t coefVX( void ) const { return dsdvx; }
  Double_t coefVY( void ) const { return dsdvy; }
  Double_t coefVU( void ) const { return dsdvu; }
  Double_t coefVV( void ) const { return dsdvv; }
  Double_t coefVQ( void ) const { return dsdvq; }
  Double_t coefQX( void ) const { return dsdqx; }
  Double_t coefQY( void ) const { return dsdqy; }
  Double_t coefQU( void ) const { return dsdqu; }
  Double_t coefQV( void ) const { return dsdqv; }
  Double_t coefQQ( void ) const { return dsdqq; }

  Double_t dXdX( void ) const { return dxdx; }
  Double_t dXdY( void ) const { return dxdy; }
  Double_t dXdU( void ) const { return dxdu; }
  Double_t dXdV( void ) const { return dxdv; }
  Double_t dXdQ( void ) const { return dxdq; }
  Double_t dYdX( void ) const { return dydx; }
  Double_t dYdY( void ) const { return dydy; }
  Double_t dYdU( void ) const { return dydu; }
  Double_t dYdV( void ) const { return dydv; }
  Double_t dYdQ( void ) const { return dydq; }
  Double_t dUdX( void ) const { return dudx; }
  Double_t dUdY( void ) const { return dudy; }
  Double_t dUdU( void ) const { return dudu; }
  Double_t dUdV( void ) const { return dudv; }
  Double_t dUdQ( void ) const { return dudq; }
  Double_t dVdX( void ) const { return dvdx; }
  Double_t dVdY( void ) const { return dvdy; }
  Double_t dVdU( void ) const { return dvdu; }
  Double_t dVdV( void ) const { return dvdv; }
  Double_t dVdQ( void ) const { return dvdq; }

  friend class RungeKutta;
};

//______________________________________________________________________________
class RKTrajectoryPoint
{
public:
  RKTrajectoryPoint( Double_t X, Double_t Y, Double_t Z,
                     Double_t U, Double_t V, Double_t Q,
                     Double_t Dxdx, Double_t Dxdy, Double_t Dxdu,
                     Double_t Dxdv, Double_t Dxdq,
                     Double_t Dydx, Double_t Dydy, Double_t Dydu,
                     Double_t Dydv, Double_t Dydq,
                     Double_t Dudx, Double_t Dudy, Double_t Dudu,
                     Double_t Dudv, Double_t Dudq,
                     Double_t Dvdx, Double_t Dvdy, Double_t Dvdu,
                     Double_t Dvdv, Double_t Dvdq,
                     Double_t L )
    : r(X,Y,Z,U,V,Q),
      dxdx(Dxdx), dxdy(Dxdy), dxdu(Dxdu), dxdv(Dxdv), dxdq(Dxdq),
      dydx(Dydx), dydy(Dydy), dydu(Dydu), dydv(Dydv), dydq(Dydq),
      dudx(Dudx), dudy(Dudy), dudu(Dudu), dudv(Dudv), dudq(Dudq),
      dvdx(Dvdx), dvdy(Dvdy), dvdu(Dvdu), dvdv(Dvdv), dvdq(Dvdq),
      l(L)
  {}

  RKTrajectoryPoint( const RKCordParameter& R,
                     Double_t Dxdx, Double_t Dxdy, Double_t Dxdu,
                     Double_t Dxdv, Double_t Dxdq,
                     Double_t Dydx, Double_t Dydy, Double_t Dydu,
                     Double_t Dydv, Double_t Dydq,
                     Double_t Dudx, Double_t Dudy, Double_t Dudu,
                     Double_t Dudv, Double_t Dudq,
                     Double_t Dvdx, Double_t Dvdy, Double_t Dvdu,
                     Double_t Dvdv, Double_t Dvdq,
                     Double_t L )
    : r(R),
      dxdx(Dxdx), dxdy(Dxdy), dxdu(Dxdu), dxdv(Dxdv), dxdq(Dxdq),
      dydx(Dydx), dydy(Dydy), dydu(Dydu), dydv(Dydv), dydq(Dydq),
      dudx(Dudx), dudy(Dudy), dudu(Dudu), dudv(Dudv), dudq(Dudq),
      dvdx(Dvdx), dvdy(Dvdy), dvdu(Dvdu), dvdv(Dvdv), dvdq(Dvdq),
      l(L)
  {}

  RKTrajectoryPoint( const ThreeVector& pos,
                     Double_t U, Double_t V, Double_t Q,
                     Double_t Dxdx, Double_t Dxdy, Double_t Dxdu,
                     Double_t Dxdv, Double_t Dxdq,
                     Double_t Dydx, Double_t Dydy, Double_t Dydu,
                     Double_t Dydv, Double_t Dydq,
                     Double_t Dudx, Double_t Dudy, Double_t Dudu,
                     Double_t Dudv, Double_t Dudq,
                     Double_t Dvdx, Double_t Dvdy, Double_t Dvdu,
                     Double_t Dvdv, Double_t Dvdq,
                     Double_t L )
    : r(pos,U,V,Q),
      dxdx(Dxdx), dxdy(Dxdy), dxdu(Dxdu), dxdv(Dxdv), dxdq(Dxdq),
      dydx(Dydx), dydy(Dydy), dydu(Dydu), dydv(Dydv), dydq(Dydq),
      dudx(Dudx), dudy(Dudy), dudu(Dudu), dudv(Dudv), dudq(Dudq),
      dvdx(Dvdx), dvdy(Dvdy), dvdu(Dvdu), dvdv(Dvdv), dvdq(Dvdq),
      l(L)
  {}

  RKTrajectoryPoint( const ThreeVector& pos,
                     const ThreeVector& mom,
                     Double_t Dxdx, Double_t Dxdy, Double_t Dxdu,
                     Double_t Dxdv, Double_t Dxdq,
                     Double_t Dydx, Double_t Dydy, Double_t Dydu,
                     Double_t Dydv, Double_t Dydq,
                     Double_t Dudx, Double_t Dudy, Double_t Dudu,
                     Double_t Dudv, Double_t Dudq,
                     Double_t Dvdx, Double_t Dvdy, Double_t Dvdu,
                     Double_t Dvdv, Double_t Dvdq,
                     Double_t L )
    : r(pos,mom),
      dxdx(Dxdx), dxdy(Dxdy), dxdu(Dxdu), dxdv(Dxdv), dxdq(Dxdq),
      dydx(Dydx), dydy(Dydy), dydu(Dydu), dydv(Dydv), dydq(Dydq),
      dudx(Dudx), dudy(Dudy), dudu(Dudu), dudv(Dudv), dudq(Dudq),
      dvdx(Dvdx), dvdy(Dvdy), dvdu(Dvdu), dvdv(Dvdv), dvdq(Dvdq),
      l(L)
  {}

  RKTrajectoryPoint( const RKcalcHitPoint& hp )
    : r( hp.PositionInGlobal(), hp.MomentumInGlobal() ),
      dxdx( hp.dXdX() ), dxdy( hp.dXdY() ),
      dxdu( hp.dXdU() ), dxdv( hp.dXdV() ), dxdq( hp.dXdQ() ),
      dydx( hp.dYdX() ), dydy( hp.dYdY() ),
      dydu( hp.dYdU() ), dydv( hp.dYdV() ), dydq( hp.dYdQ() ),
      dudx( hp.dUdX() ), dudy( hp.dUdY() ),
      dudu( hp.dUdU() ), dudv( hp.dUdV() ), dudq( hp.dUdQ() ),
      dvdx( hp.dVdX() ), dvdy( hp.dVdY() ),
      dvdu( hp.dVdU() ), dvdv( hp.dVdV() ), dvdq( hp.dVdQ() ),
      l( hp.PathLength() )
  {}

private:
  RKCordParameter r;
  Double_t dxdx, dxdy, dxdu, dxdv, dxdq;
  Double_t dydx, dydy, dydu, dydv, dydq;
  Double_t dudx, dudy, dudu, dudv, dudq;
  Double_t dvdx, dvdy, dvdu, dvdv, dvdq;
  Double_t l;

public:
  ThreeVector PositionInGlobal( void ) const { return r.PositionInGlobal(); }
  ThreeVector MomentumInGlobal( void ) const { return r.MomentumInGlobal(); }
  Double_t      PathLength( void )         const { return l; }
  void        Print( std::ostream& ost ) const;

  friend class RungeKutta;
};

//______________________________________________________________________________
class RKHitPointContainer
  : public std::vector< std::pair<int,RKcalcHitPoint> >
{
public:
  const RKcalcHitPoint&  HitPointOfLayer( Int_t lnum ) const;
  RKcalcHitPoint&  HitPointOfLayer( Int_t lnum );

  typedef std::vector< std::pair<int,RKcalcHitPoint> >::const_iterator RKHpCIterator;
  typedef std::vector< std::pair<int,RKcalcHitPoint> >::iterator       RKHpIterator;

  static TString ClassName( void )
  { static TString g_name("RKHitPointContainer"); return g_name; }
};

//______________________________________________________________________________
inline std::ostream&
operator <<( std::ostream& ost, const RKFieldIntegral& obj )
{
  obj.Print( ost );
  return ost;
}

//______________________________________________________________________________
inline std::ostream&
operator <<( std::ostream& ost, const RKDeltaFieldIntegral& obj )
{
  obj.Print( ost );
  return ost;
}

//______________________________________________________________________________
inline std::ostream&
operator <<( std::ostream& ost, const RKCordParameter& obj )
{
  obj.Print( ost );
  return ost;
}

//______________________________________________________________________________
inline std::ostream&
operator <<( std::ostream& ost, const RKTrajectoryPoint& obj )
{
  obj.Print( ost );
  return ost;
}

#endif
