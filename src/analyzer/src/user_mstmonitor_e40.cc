// Author: Tomonori Takahashi

#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>

#include "user_analyzer.hh"
#include "Controller.hh"
#include "ConfMan.hh"
#include "HistMaker.hh"
#include "MacroBuilder.hh"
#include "DetectorID.hh"

#include <DAQNode.hh>
#include <filesystem_util.hh>
#include <Unpacker.hh>
#include <UnpackerManager.hh>

namespace analyzer
{
  using namespace hddaq::unpacker;
  using namespace hddaq;

  namespace{
    const int n_ch_tof = 24;
    std::vector<TH1*> hptr_array;

    const int n_ref = 4;
    const int ref_seg[n_ref] = {5, 13, 17, 21};
    TH1* ref_hist[n_ref];
    std::string out_pdf = "";
  }

//____________________________________________________________________________
int
process_begin(const std::vector<std::string>& argv)
{
  ConfMan& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  //  GUnpacker::get_instance().set_decode_mode(false);

  std::string runno      = argv.at(argv.size()-1);
  std::string ref_runno  = argv.at(argv.size()-2);
  std::string result_dir = argv.at(argv.size()-3);

  // Reference
  const std::string ref_file = result_dir + "/rootfile/run0" +ref_runno + ".root";
  new TFile(ref_file.c_str());
  if(!gFile->IsOpen()){
    std::cerr << "#E: " << __func__ << "() "
	      << "Rootfile open error (" << ref_file << ")"
	      << std::endl;
    return -1;
  }
  for(int i = 0; i<n_ref; ++i){
    ref_hist[i] = dynamic_cast<TH1*>(gFile->Get(Form("MsT_TOF_TDC_%d", ref_seg[i])));
  }

  // Output root file
  const std::string out_file = result_dir + "/rootfile/run" + runno + ".root";
  new TFile(out_file.c_str(),"recreate");

  // pdf
  out_pdf = result_dir + "/pdf/run" + runno + ".pdf";

  // HistMaker
  HistMaker& gHist = HistMaker::getInstance();
  gHist.createMsT();
  //  tab_hist->Add(gHist.createMsT());
  if(0 != gHist.setHistPtr(hptr_array)){ return -1; }

  // mst-tof ________________________________________
  const int mst_tof_id = gHist.getSequentialID( kMsT, 0, kTDC);
  for(int i = 0; i<n_ch_tof; ++i){
    hptr_array[mst_tof_id + i]->Rebin(10);
  }// for(i)

  return 0;
}

//____________________________________________________________________________
int
process_end()
{
  HistMaker& gHist = HistMaker::getInstance();

  const int tgt_seg_id[] =
    {
      gHist.getSequentialID(kMsT, 0, kTDC, ref_seg[0]),
      gHist.getSequentialID(kMsT, 0, kTDC, ref_seg[1]),
      gHist.getSequentialID(kMsT, 0, kTDC, ref_seg[2]),
      gHist.getSequentialID(kMsT, 0, kTDC, ref_seg[3])
    };

  TCanvas *c1 = new TCanvas("c1", "c1", 1200, 800);
  c1->Divide(2,2);
  for(int i = 0; i<n_ref; ++i){
    c1->cd(i+1);
    double scale = hptr_array[tgt_seg_id[i]]->GetEntries()/ref_hist[i]->GetEntries();
    ref_hist[i]->GetXaxis()->SetRangeUser(180*1000, 220*1000);
    ref_hist[i]->SetLineColor(2);
    ref_hist[i]->Scale(scale);
    ref_hist[i]->Draw("HIST");

    hptr_array[tgt_seg_id[i]]->Draw("same");
  }// for(i)

  c1->Print(out_pdf.c_str());

  return 0;
}

//____________________________________________________________________________
int
process_event()
{
  static UnpackerManager& gUnpacker = GUnpacker::get_instance();
  HistMaker & gHist = HistMaker::getInstance();

  // trigger flag
  bool pipi_flag = false;
  const int tdc_min = 900;
  const int tdc_max = 1000;
  {
    static const int k_device = gUnpacker.get_device_id("TFlag");
    static const int k_tdc    = gUnpacker.get_data_id("TFlag", "tdc");

    int nhit = gUnpacker.get_entries( k_device, 0, trigger::kBeamPiPS, 0, k_tdc );
    for(int m = 0; m<nhit; ++m){
      int tdc = gUnpacker.get( k_device, 0, trigger::kBeamPiPS, 0, k_tdc, m );
      if(tdc_min < tdc && tdc < tdc_max) pipi_flag = true;
    }// for(m)
  }

  if(!pipi_flag) return 0;

  // MsT TOF
  {
    static const int k_device = gUnpacker.get_device_id("MsT");
    static const int k_tof    = gUnpacker.get_plane_id("MsT", "TOF");
    static const int k_tdc    = 0;

    static const int mst_tof_id = gHist.getSequentialID( kMsT, 0, kTDC);

    // TOF
    for(int i = 0; i<n_ch_tof; ++i){
      int nhit = gUnpacker.get_entries( k_device, k_tof, i, 0, k_tdc );
      for(int m = 0; m<nhit; ++m){
	int tdc = gUnpacker.get( k_device, k_tof, i, 0, k_tdc, m );
	hptr_array[mst_tof_id + i]->Fill(tdc);
      }// for(m)
    }// for(i)
  }

  return 0;
}

}
