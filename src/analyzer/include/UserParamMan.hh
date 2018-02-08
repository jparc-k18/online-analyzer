// -*- C++ -*-

#ifndef USER_PARAM_MAN_HH
#define USER_PARAM_MAN_HH

#include <vector>
#include <map>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class UserParamMan : public TObject
{
public:
  static UserParamMan& GetInstance( void );
  ~UserParamMan( void );

private:
  UserParamMan( void );
  UserParamMan( const UserParamMan& );
  UserParamMan& operator =( const UserParamMan& );

private:
  typedef std::vector<Double_t>         arrayType;
  typedef std::map<TString, arrayType>  mapType;

  mapType param_container_;

public:
  Bool_t   Initialize( const TString& filename );
  Double_t GetParameter( const TString& name, Int_t index=0 ) const;
  Int_t    GetSize( const TString& name) const;
  Bool_t   IsGood( void ) const;

  ClassDef(UserParamMan,0);
};

//______________________________________________________________________________
inline UserParamMan&
UserParamMan::GetInstance( void )
{
  static UserParamMan g_instance;
  return g_instance;
}

#endif
