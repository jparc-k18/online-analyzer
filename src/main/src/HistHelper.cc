// Author: Tomonori Takahashi

#include "HistHelper.hh"

#include <iostream>

#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include "lexical_cast.hh"

ClassImp(GHist)

//______________________________________________________________________________
GHist::GHist()
  : m_index()
{
}

//______________________________________________________________________________
GHist::~GHist()
{
}
//______________________________________________________________________________
TH1*
GHist::D1(const std::string& name,
	  const std::string& title,
	  int nbinsx,
	  double xlow,
	  double xup)
{
  return new TH1D(name.c_str(), title.c_str(), nbinsx, xlow, xup);
}

//______________________________________________________________________________
TH1*
GHist::D1(int id,
	  const std::string& title,
	  int nbinsx,
	  double xlow,
	  double xup)
{
  std::string name = title;
  GHist& g_h = GHist::getInstance();
  if("" != g_h.m_index[id]){
    std::cerr << "#E HistHelper::D1" << std::endl;
    std::cerr << " The histogram already exists with the same ID" << std::endl;
    std::cerr << " ID : " << id << std::endl;
    return NULL;
  }
  g_h.m_index[id] = name;
  return new TH1D(name.c_str(), title.c_str(), nbinsx, xlow, xup);
}

//______________________________________________________________________________
TH1*
GHist::I1(const std::string& name,
	  const std::string& title,
	  int nbinsx,
	  double xlow,
	  double xup)
{
  return new TH1I(name.c_str(), title.c_str(), nbinsx, xlow, xup);
}

//______________________________________________________________________________
TH1*
GHist::I1(int id,
	  const std::string& title,
	  int nbinsx,
	  double xlow,
	  double xup)
{
  std::string name = title;
  GHist& g_h = GHist::getInstance();
  if("" != g_h.m_index[id]){
    std::cerr << "#E HistHelper::I1" << std::endl;
    std::cerr << " The histogram already exists with the same ID" << std::endl;
    std::cerr << " ID : " << id << std::endl;
    return NULL;
  }
  g_h.m_index[id] = name;
  return new TH1I(name.c_str(), title.c_str(), nbinsx, xlow, xup);
}

//______________________________________________________________________________
int
GHist::fill1(const std::string& name,
	     double x,
	     double w)
{
  TH1* h = GHist::get(name);
  if (h)
    return h->Fill(x, w);
  return 0;
}

//______________________________________________________________________________
int
GHist::fill1(int id,
	     double x,
	     double w)
{
  TH1* h = GHist::get(id);
  if (h)
    return h->Fill(x, w);
  return 0;
}

//______________________________________________________________________________
int
GHist::fill1(TH1* h,
	     double x,
	     double w)
{

  if (h)
    {
//       std::cout << "#D H1::fill() \n" 
// 		<< h->GetName() << " x = " << x << " w = " << w 
// 		<< std::endl;
      return h->Fill(x, w);
    }
  else
    std::cerr << "#E GHist::fill()\n" 
	      << " got null pointer" << std::endl;
  return 0;
}

//______________________________________________________________________________
TH2*
GHist::D2(const std::string& name,
       const std::string& title,
       int nbinsx,
       double xlow,
       double xup,
       int nbinsy,
       double ylow,
       double yup)
{
  return new TH2D(name.c_str(), title.c_str(),
		  nbinsx, xlow, xup,
		  nbinsy, ylow, yup);
}

//______________________________________________________________________________
TH2*
GHist::D2(int id,
       const std::string& title,
       int nbinsx,
       double xlow,
       double xup,
       int nbinsy,
       double ylow,
       double yup)
{
  std::string name = title;
  GHist& g_h = GHist::getInstance();
  if("" != g_h.m_index[id]){
    std::cerr << "#E HistHelper::I1" << std::endl;
    std::cerr << " The histogram already exists with the same ID" << std::endl;
    std::cerr << " ID : " << id << std::endl;
    return NULL;
  }
  g_h.m_index[id] = name;
  return new TH2D(name.c_str(), title.c_str(),
		  nbinsx, xlow, xup,
		  nbinsy, ylow, yup);
}

//______________________________________________________________________________
TH2*
GHist::I2(const std::string& name,
	  const std::string& title,
	  int nbinsx,
	  double xlow,
	  double xup,
	  int nbinsy,
	  double ylow,
	  double yup)
{
  return new TH2I(name.c_str(), title.c_str(),
		  nbinsx, xlow, xup,
		  nbinsy, ylow, yup);
}

//______________________________________________________________________________
TH2*
GHist::I2(int id,
	  const std::string& title,
	  int nbinsx,
	  double xlow,
	  double xup,
	  int nbinsy,
	  double ylow,
	  double yup)
{
  std::string name = title;
  GHist& g_h = GHist::getInstance();
  if("" != g_h.m_index[id]){
    std::cerr << "#E HistHelper::I2" << std::endl;
    std::cerr << " The histogram already exists with the same ID" << std::endl;
    std::cerr << " ID : " << id << std::endl;
    return NULL;
  }
  g_h.m_index[id] = name;
  return new TH2I(name.c_str(), title.c_str(), 
		  nbinsx, xlow, xup,
		  nbinsy, ylow, yup);
}

//______________________________________________________________________________
int
GHist::fill2(const std::string& name,
	     double x,
	     double y,
	     double w)
{
  TH2* h = dynamic_cast<TH2*>(GHist::get(name));
  if (h)
    return h->Fill(x, y, w);
  return 0;
}

//______________________________________________________________________________
int
GHist::fill2(int id,
	     double x,
	     double y,
	     double w)
{
  TH2* h = dynamic_cast<TH2*>(GHist::get(id));
  if (h)
    return h->Fill(x, y, w);
  return 0;
}

//______________________________________________________________________________
int
GHist::fill2(TH2* h,
	     double x,
	     double y,
	     double w)
{
  if (h)
    return h->Fill(x, y, w);
  return 0;
}

//______________________________________________________________________________
//______________________________________________________________________________
TH1*
GHist::get(const std::string& name)
{
  TObject *ptr = gDirectory->Get(name.c_str());
  if(ptr == NULL){
    std::cerr << "#E HistHelper::get" << std::endl;
    std::cerr << " NULL pointer is returned" << std::endl;
    std::cerr << " name : " << name << std::endl;
    return NULL;
  }
  return dynamic_cast<TH1*>(ptr);
}
	   
//______________________________________________________________________________
TH1*
GHist::get(int id)
{
  TH1 *ptr = GHist::get(GHist::getInstance().m_index[id]);
  if(ptr == NULL){
    std::cerr << "#E HistHelper::get" << std::endl;
    std::cerr << " NULL pointer is returned" << std::endl;
    std::cerr << " ID : " << id << std::endl;
    return NULL;
  }
  return ptr;
}
	   
//______________________________________________________________________________
GHist&
GHist::getInstance()
{
  static GHist g_instance;
  return g_instance;
}
