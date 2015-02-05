// Author: Tomonori Takahashi

#ifndef ANALYZER_HISTOGRAM_HELPER_H
#define ANALYZER_HISTOGRAM_HELPER_H

#include <string>
#include <vector>
#include <map>
#include <Rtypes.h>

class TH1;
class TH2;

class GHist
{

protected:
  std::map<int, std::string> m_index;

public:
  static GHist& getInstance();
  virtual ~GHist();

  // methods for TH1 --------------------------------------------------
  static TH1* D1(const std::string& name,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xup);
  static TH1* D1(int id,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xup);
  static TH1* I1(const std::string& name,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xup);
  static TH1* I1(int id,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xup);
  static int  fill1(const std::string& name, 
		    double x,
		    double w=1.0);
  static int  fill1(int id,
		    double x,
		    double w=1.0);
  static int  fill1(TH1* h,
		    double x,
		    double w=1.0);

  // methods for TH1 --------------------------------------------------
  static TH2* D2(const std::string& name,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xip,
		 int nbinsy,
		 double ylow,
		 double yup);
  static TH2* D2(int id,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xip,
		 int nbinsy,
		 double ylow,
		 double yup);
  static TH2* I2(const std::string& name,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xip,
		 int nbinsy,
		 double ylow,
		 double yup);
  static TH2* I2(int id,
		 const std::string& title,
		 int nbinsx,
		 double xlow,
		 double xip,
		 int nbinsy,
		 double ylow,
		 double yup);
  static int  fill2(const std::string& name,
		    double x,
		    double y,
		    double w=1.0);
  static int  fill2(int id,
		    double x,
		    double y,
		    double w=1.0);
  static int  fill2(TH2* h,
		    double x,
		    double y,
		    double w=1.0);

  // Common mothod ------------------------------------------------------
  // No function which return TH2 pointer is prepared.
  // If the actual class of the pointer is TH2, 
  // dynamic_cast in the user source code is need.
  static TH1* get(const std::string& name);
  static TH1* get(int id);

private:
  GHist();
  GHist(const GHist&);
  GHist& operator=(const GHist&);

  ClassDef(GHist, 0)

};

#endif
