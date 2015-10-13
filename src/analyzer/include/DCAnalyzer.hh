// Author : Hitoshi Sugimura

#ifndef DCANALYZER_H
#define DCANALYZER_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

//______________________________________________________________________________
class DCRHC
{
private:
  std::vector<std::vector<int> >     m_hitwire;
  std::vector<std::vector<double> >  m_hitpos;
  std::vector<std::vector<int> >     m_tdc;
  std::vector<std::vector<double> >  m_dtime;
  std::vector<std::vector<double> >  m_dlength;
  std::vector<double> buf_hitpos;
  std::vector<int> H;
  std::vector<double> A;
  std::vector<double> z;
  std::vector<double> sinvector;
  std::vector<double> cosvector;
  std::vector<std::vector<double> > inv_matrix; 
  std::vector<int> lr;
  double X0,Y0,dXdZ,dYdZ;
  int detid;
  double chi2;

public:
  DCRHC(int DetectorID);
  ~DCRHC();
  bool TrackSearch(int min_plane);
  bool PairPlaneSearch(int pairplane);
  bool make_MinimumPoint(double& dxdZ,double& dydZ,double& x0,double& y0);
  double GetChisquare(){return chi2;};
  double GetDriftLength(int plane);
  double GetDriftTime(int plane);
  int Getlr(int plane);
  double GetPosX(int PosZ);
  double GetPosY(int PosZ);
  double GetResidual(int plane);
  double GetResidualSdcOut(int plane);
  double GetU(){return dXdZ;};
  double GetV(){return dYdZ;};
  std::vector<int > GetTdc(int plane);
  std::vector<int > GetHitWire(int plane);
  void FullTrackSearch();
  void HitPosition(int DetectorID);
  void hoge();
  void init_BcOut();
  void init_SdcIn();
  void init_SdcOut();
  void init_Ssd();
  void pushback(int DetectorID);
  void pushback_BcOut();
  void pushback_SdcIn();
  void pushback_SdcOut();
  void pushback_Ssd();
  void ReHitPosition(int DetectorID);
  void SetGeomBcOut(std::vector<double> & wo,
		      std::vector<double> & pit,
		      std::vector<double> & wc);
  void SetGeomSdcIn(std::vector<double> & wo,
		      std::vector<double> & pit,
		      std::vector<double> & wc);
  void SetGeomSdcOut(std::vector<double> & wo,
		       std::vector<double> & pit,
		       std::vector<double> & wc);
  void makeAvector();
  void makeChisquare(int DetectorID);
  void makeChisquareSdcOut();
  void makeDriftLength(int DetectorID);
  void makeDriftTime(int DetectorID);
  void makeHvector(int DetectorID);
  void makeMatrix();
  int num_Hitplane();
  std::vector<double> resolveLR(double left1,
				double right1,
				double left2,
				double right2);
  double TimeToLength(std::vector<std::vector<double> > dc_geom,
		      std::map<double,double> t0list);
private:
  DCRHC(const DCRHC&);
  DCRHC& operator = (const DCRHC);
};

// Global functions
std::vector<std::vector<double> >
InvMatrix(std::vector<std::vector<double> > Matrix);

std::vector<double>
angle(int degree);

#endif
