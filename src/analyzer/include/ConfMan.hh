// -*- C++ -*-

#ifndef CONF_MAN_HH
#define CONF_MAN_HH

#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <iomanip>

#include <std_ostream.hh>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class ConfMan : public TObject
{
public:
  static ConfMan& GetInstance( void );
  ~ConfMan( void );

private:
  ConfMan( void );
  ConfMan( const ConfMan& );
  ConfMan& operator =( const ConfMan& );

  // Private parameter declarations ----------------------------------------
  // Arguments of main function
  enum ArgumentList {
    kProcess, kConfPath, kStreamPath,
    sizeArgumentList
  };
  TString path_file_[sizeArgumentList];

  // File path list in configuration file
  std::map<TString, TString> m_key_map;
  std::map<TString, Int_t>   m_int_map;
  std::map<TString, Double_t>   m_double_map;

  // Flags
  enum Flag {
    kIsGood,
    sizeFlag
  };
  std::bitset<sizeFlag> flag_;

public:
  void   Initialize(const std::vector<std::string>& argv);
  Bool_t IsGood( void ) const { return flag_[kIsGood]; }

  template <typename T>
  static const T& Get( const TString& key ) { return T(); }

  template <typename T>
  Bool_t InitializeParameter( void );
  template <typename T>
  Bool_t InitializeParameter( const TString& key );
  template <typename T>
  Bool_t InitializeParameter( const TString& key1,
			      const TString& key2 );

private:
  Bool_t ShowResult( Bool_t s, const TString& name );

  ClassDef(ConfMan,0);
};

//______________________________________________________________________________
inline ConfMan& ConfMan::GetInstance( void )
{
  static ConfMan g_instance;
  return g_instance;
}

//______________________________________________________________________________
template <>
inline const Int_t&
ConfMan::Get<Int_t>( const TString& key )
{
  return GetInstance().m_int_map[key];
}

//______________________________________________________________________________
template <>
inline const Double_t&
ConfMan::Get<Double_t>( const TString& key )
{
  return GetInstance().m_double_map[key];
}

//______________________________________________________________________________
template <typename T>
inline Bool_t
ConfMan::InitializeParameter( void )
{
  return
    ShowResult( T::GetInstance().Initialize(),
		T::GetInstance().ClassName() );
}

//______________________________________________________________________________
template <typename T>
inline Bool_t
ConfMan::InitializeParameter( const TString& key )
{
  return
    ShowResult( T::GetInstance().Initialize( m_key_map[key] ),
		T::GetInstance().ClassName() );
}

//______________________________________________________________________________
template <typename T>
inline Bool_t
ConfMan::InitializeParameter( const TString& key1,
			      const TString& key2 )
{
  return
    ShowResult( T::GetInstance().Initialize( m_key_map[key1],
					     m_key_map[key2] ),
		T::GetInstance().ClassName() );
}

//______________________________________________________________________________
inline Bool_t
ConfMan::ShowResult( Bool_t s, const TString& name )
{
  if( s )
    hddaq::cout << std::setw(20) << std::left
                << " ["+name+"]"
                << "-> Initialized" << std::endl;
  else
    hddaq::cout << std::setw(20) << std::left
                << " ["+name+"]"
                << "-> Failed" << std::endl;

  if( flag_[kIsGood] && !s ) flag_.reset(kIsGood);
  return s;
}

#endif
