// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

const int n_seg_tof = 24;
const int n_seg_fbh = 31;
const int n_seg_sch = 64;

void dispMtx3D(int fbh_seg)
{
  std::cout << "#D You selected FBH seg" << fbh_seg << std::endl;

  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // Draw Mtx3D pattern and TOFxSCH
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->cd(0);
    int mtx3d_id  = HistMaker::getUniqueID(kMisc,  kHul3D,      kHitPat2D);
    int tofsch_id = HistMaker::getUniqueID(kMtx3D, kHul3DHitPat,kHitPat2D);
    tofsch_id    += fbh_seg;
    TH2 *hmtx3d  = (TH1*)GHist::get(mtx3d_id+fbh_seg);
    TH2 *htofsch = (TH1*)GHist::get(tofsch_id);
    
    hmtx3d->Reset();
    hmtx3d->SetLineWidth(2);
    hmtx3d->SetLineColor(2);
    MatrixParamMan&  gMatrix = MatrixParamMan::GetInstance();
    for(int i_tof = 0; i_tof<n_seg_tof; ++i_tof){
      for(int i_sch = 0; i_sch<n_seg_sch; ++i_sch){
	bool hul3d_flag = gMatrix.IsAccept( i_tof, i_sch, fbh_seg );
	if(!hul3d_flag) hmtx3d->Fill(i_sch, i_tof);
      } // for(i_sch)
    } // for(i_tof)
    
    hmtx3d->GetXaxis()->SetRangeUser(0, n_seg_sch);
    hmtx3d->GetYaxis()->SetRangeUser(0, n_seg_tof+1);
    hmtx3d->Draw("box");

    htofsch->GetXaxis()->SetRangeUser(0, n_seg_sch);
    htofsch->GetYaxis()->SetRangeUser(0, n_seg_tof);
    htofsch->Draw("same");
    
    gPad->SetGridx();
    gPad->SetGridy();
    
    c->Update();
  }

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
