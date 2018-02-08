// -*- C++ -*-

#ifndef CONF_MAN_HH
#define CONF_MAN_HH

#include <string>
#include <vector>
#include <map>
#include <bitset>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class ConfMan : public TObject
{
public:
  static ConfMan& GetInstance( void );
  virtual ~ConfMan( void );

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
  std::map<TString, TString> name_file_;

  // Flags
  enum Flag {
    kIsGood,
    sizeFlag
  };
  std::bitset<sizeFlag> flag_;

public:
  void Initialize(const std::vector<std::string>& argv);
  bool IsGood( void ) const { return flag_[kIsGood]; }
  void InitializeHodoParamMan( void );
  void InitializeHodoPHCMan( void );
  void InitializeDCGeomMan( void );
  void InitializeDCTdcCalibMan( void );
  void InitializeDCDriftParamMan( void );
  void InitializeEMCParamMan( void );
  void InitializeMatrixParamMan( void );
  void InitializeMsTParamMan( void );
  void InitializeUserParamMan( void );
  void InitializeGeAdcCalibMan( void );

  ClassDef(ConfMan,0);
};

//______________________________________________________________________________
inline ConfMan& ConfMan::GetInstance( void )
{
  static ConfMan object;
  return object;
}

#endif
