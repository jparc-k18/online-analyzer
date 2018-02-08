// -*- C++ -*-

#ifndef MST_PARAM_MAN_HH
#define MST_PARAM_MAN_HH

#include <vector>
#include <map>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class MsTParamMan : public TObject
{
public:
  static MsTParamMan& GetInstance( void );
  ~MsTParamMan( void );

private:
  MsTParamMan( void );
  MsTParamMan( const MsTParamMan& );
  MsTParamMan& operator =( const MsTParamMan& );

private:
  std::vector< std::vector<Double_t> > m_low_threshold;
  std::vector< std::vector<Double_t> > m_high_threshold;

public:
  bool Initialize( const TString& filename );
  bool IsAccept( Int_t detA, Int_t detB, Int_t tdc );

  ClassDef(MsTParamMan,0);
};

//______________________________________________________________________________
inline MsTParamMan&
MsTParamMan::GetInstance( void )
{
  static MsTParamMan g_instance;
  return g_instance;
}

#endif
