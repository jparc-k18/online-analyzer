#include"MacroBuilder.hh"
#include"HistMaker.hh"
#include"Main.hh"

#include<vector>
#include<string>

#include<TMacro.h>

// Get execute path ---------------------------------------------
std::string getMacroPath( void )
{
  const std::vector<std::string> argv 
    = analyzer::Main::getInstance().getArgv();
  std::string str_process_name = argv[0];
  int n = str_process_name.find("bin");
  return str_process_name.substr(0, n) + "src/analyzer/macro/";
}

// hoge ---------------------------------------------------------
// This is just an exsample of macros
TObject* hoge( void )
{
  std::string path = getMacroPath() + "hoge.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("hoge");
  return m;
}

// clear_all_canvas --------------------------------------------------
TObject* clear_all_canvas( void )
{
  std::string path = getMacroPath() + "clear_all_canvas.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("clear_all_canvas");
  return m;
}

// clear_canvas --------------------------------------------------
TObject* clear_canvas( void )
{
  std::string path = getMacroPath() + "clear_canvas.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("clear_canvas");
  return m;
}

// split22 -------------------------------------------------------
TObject* split22( void )
{
  std::string path = getMacroPath() + "split22.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("split22");
  return m;
}

// split32 -------------------------------------------------------
TObject* split32( void )
{
  std::string path = getMacroPath() + "split32.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("split32");
  return m;
}

// split33 -------------------------------------------------------
TObject* split33( void )
{
  std::string path = getMacroPath() + "split33.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("split33");
  return m;
}

// dispBH1 -------------------------------------------------------
TObject* dispBH1( void )
{
  std::string path = getMacroPath() + "dispBH1.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBH1");
  return m;
}

// dispBFT -------------------------------------------------------
TObject* dispBFT( void )
{
  std::string path = getMacroPath() + "dispBFT.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBFT");
  return m;
}

// dispBH2 -------------------------------------------------------
TObject* dispBH2( void )
{
  std::string path = getMacroPath() + "dispBH2.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBH2");
  return m;
}

// dispACs -------------------------------------------------------
TObject* dispACs( void )
{
  std::string path = getMacroPath() + "dispACs.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispACs");
  return m;
}

// dispFBH -------------------------------------------------------
TObject* dispFBH( void )
{
  std::string path = getMacroPath() + "dispFBH.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispFBH");
  return m;
}

// dispSSD1 -------------------------------------------------------
TObject* dispSSD1( void )
{
  std::string path = getMacroPath() + "dispSSD1.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSSD1");
  return m;
}

// dispSSD2 -------------------------------------------------------
TObject* dispSSD2( void )
{
  std::string path = getMacroPath() + "dispSSD2.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSSD2");
  return m;
}

// dispSSDHitPat -------------------------------------------------------
TObject* dispSSDHitPat( void )
{
  std::string path = getMacroPath() + "dispSSDHitPat.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSSDHitPat");
  return m;
}

// dispProfileSSD -------------------------------------------------------
TObject* dispProfileSSD( void )
{
  std::string path = getMacroPath() + "dispProfileSSD.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispProfileSSD");
  return m;
}

// dispSCH -------------------------------------------------------
TObject* dispSCH( void )
{
  std::string path = getMacroPath() + "dispSCH.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSCH");
  return m;
}

// dispTOF -------------------------------------------------------
TObject* dispTOF( void )
{
  std::string path = getMacroPath() + "dispTOF.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispTOF");
  return m;
}

// dispMsT -------------------------------------------------------
TObject* dispMsT( void )
{
  std::string path = getMacroPath() + "dispMsT.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispMsT");
  return m;
}

// dispBC3 -------------------------------------------------------
TObject* dispBC3( void )
{
  std::string path = getMacroPath() + "dispBC3.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBC3");
  return m;
}

// dispBC4 -------------------------------------------------------
TObject* dispBC4( void )
{
  std::string path = getMacroPath() + "dispBC4.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBC4");
  return m;
}

// dispSDC1 -------------------------------------------------------
TObject* dispSDC1( void )
{
  std::string path = getMacroPath() + "dispSDC1.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSDC1");
  return m;
}

// dispSDC2 -------------------------------------------------------
TObject* dispSDC2( void )
{
  std::string path = getMacroPath() + "dispSDC2.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSDC2");
  return m;
}

// dispSDC3 -------------------------------------------------------
TObject* dispSDC3( void )
{
  std::string path = getMacroPath() + "dispSDC3.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSDC3");
  return m;
}

// dispHitPat -----------------------------------------------------
TObject* dispHitPat( void )
{
  std::string path = getMacroPath() + "dispHitPat.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispHitPat");
  return m;
}

// efficiency BcOut -----------------------------------------------
TObject* effBcOut( void )
{
  std::string path = getMacroPath() + "effBcOut.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("effBcOut");
  return m;
}

// efficiency SdcIn -----------------------------------------------
TObject* effSdcIn( void )
{
  std::string path = getMacroPath() + "effSdcIn.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("effSdcIn");
  return m;
}

// efficiency SdcOut ----------------------------------------------
TObject* effSdcOut( void )
{
  std::string path = getMacroPath() + "effSdcOut.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("effSdcOut");
  return m;
}

// Beam profile ---------------------------------------------------
TObject* dispBeamProfile( void )
{
  std::string path = getMacroPath() + "dispBeamProfile.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBeamProfile");
  return m;
}

// dispBAC -------------------------------------------------------
TObject* dispBAC( void )
{
  std::string path = getMacroPath() + "dispBAC.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBAC");
  return m;
}

// dispACs_SFV -------------------------------------------------------
TObject* dispACs_SFV( void )
{
  std::string path = getMacroPath() + "dispACs_SFV.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispACs_SFV");
  return m;
}

// dispGeAdc -------------------------------------------------------
TObject* dispGeAdc( void )
{
  std::string path = getMacroPath() + "dispGeAdc.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispGeAdc");
  return m;
}

// dispGeAdc_60Co -------------------------------------------------------
TObject* dispGeAdc_60Co( void )
{
  std::string path = getMacroPath() + "dispGeAdc_60Co.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispGeAdc_60Co");
  return m;
}

// dispGeAdc_60Co_1170 -------------------------------------------------------
TObject* dispGeAdc_60Co_1170( void )
{
  std::string path = getMacroPath() + "dispGeAdc_60Co_1170.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispGeAdc_60Co_1170");
  return m;
}

// dispGeAdc_60Co_1330 -------------------------------------------------------
TObject* dispGeAdc_60Co_1330( void )
{
  std::string path = getMacroPath() + "dispGeAdc_60Co_1330.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispGeAdc_60Co_1330");
  return m;
}

// dispGeTdc -------------------------------------------------------
TObject* dispGeTdc( void )
{
  std::string path = getMacroPath() + "dispGeTdc.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispGeTdc");
  return m;
}

// dispPWOTdc -------------------------------------------------------
TObject* dispPWOTdc( void )
{
  std::string path = getMacroPath() + "dispPWOTdc.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispPWOTdc");
  return m;
}

// dispBH2_E07 -------------------------------------------------------
TObject* dispBH2_E07( void )
{
  std::string path = getMacroPath() + "dispBH2_E07.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBH2_E07");
  return m;
}

// dispACs_E07 -------------------------------------------------------
TObject* dispACs_E07( void )
{
  std::string path = getMacroPath() + "dispACs_E07.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispACs_E07");
  return m;
}

// dispHitPatE07 -----------------------------------------------------
TObject* dispHitPatE07( void )
{
  std::string path = getMacroPath() + "dispHitPatE07.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispHitPatE07");
  return m;
}

// dispSSD0 -------------------------------------------------------
TObject* dispSSD0( void )
{
  std::string path = getMacroPath() + "dispSSD0.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSSD0");
  return m;
}

// dispPWO_E05 -------------------------------------------------------
TObject* dispPWO_E05( void )
{
  std::string path = getMacroPath() + "dispPWO_E05.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispPWO_E05");
  return m;
}

// dispKIC -------------------------------------------------------
TObject* dispKIC( void )
{
  std::string path = getMacroPath() + "dispKIC.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispKIC");
  return m;
}

// dispSP0Adc ----------------------------------------------------
TObject* dispSP0Adc( void )
{
  std::string path = getMacroPath() + "dispSP0Adc.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSP0Adc");
  return m;
}

// dispSP0Tdc ----------------------------------------------------
TObject* dispSP0Tdc( void )
{
  std::string path = getMacroPath() + "dispSP0Tdc.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSP0Tdc");
  return m;
}

// dispLAC -------------------------------------------------------
TObject* dispLAC( void )
{
  std::string path = getMacroPath() + "dispLAC.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispLAC");
  return m;
}

// dispLC -------------------------------------------------------
TObject* dispLC( void )
{
  std::string path = getMacroPath() + "dispLC.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispLC");
  return m;
}

// dispHDC -------------------------------------------------------
TObject* dispHDC( void )
{
  std::string path = getMacroPath() + "dispHDC.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispHDC");
  return m;
}

// dispSDC4 -------------------------------------------------------
TObject* dispSDC4( void )
{
  std::string path = getMacroPath() + "dispSDC4.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispSDC4");
  return m;
}

// Beam profile for E07 study -------------------------------------
TObject* dispBeamProfile_e07( void )
{
  std::string path = getMacroPath() + "dispBeamProfile_e07.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBeamProfile_e07");
  return m;
}

// Beam profile for E05 -------------------------------------------
TObject* dispBeamProfile_e05( void )
{
  std::string path = getMacroPath() + "dispBeamProfile_e05.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("dispBeamProfile_e05");
  return m;
}

// auto_monitor_all -----------------------------------------------
TObject* auto_monitor_all( void )
{
  std::string path = getMacroPath() + "auto_monitor_all.C";
  TMacro* m = new TMacro(path.c_str());
  m->SetName("auto_monitor_all");
  return m;
}
