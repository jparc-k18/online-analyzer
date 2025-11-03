// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void dispSAC_ktune_e63()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  gStyle->SetOptStat(0);

  // draw BAC ADC & TDC
  TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  c->Clear();
  c->Divide(3,2);

  c->cd(1);
  gPad->SetLogy();
  {
    TH1 *h = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+25+16);
    h->GetXaxis()->SetRangeUser(0,2000);
    h->Draw();
    TH1 *hh = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+26+16);
    hh->SetLineColor( kRed );
    hh->Draw("same");
    TText *t1 = new TText(0.65,0.85,"Red: w/TDC");
    t1->SetNDC(1);
    t1->SetTextColor(kRed);
    t1->SetTextSize(0.04);
    t1->Draw("same");
  }

  c->cd(2);
  gPad->SetLogy();
  {
    TH1 *h = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+25+16);
    h->GetXaxis()->SetRangeUser(0,2000);
    h->Draw();
    TH1 *hpi = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+27+16);
    hpi->SetLineColor( kBlue );
    hpi->Draw("same");
    TH1 *hk = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+28+16);
    hk->SetLineColor( kGreen );
    hk->Draw("same");
    TText *t1 = new TText(0.65,0.85,"Blue: pion");
    t1->SetNDC(1);
    t1->SetTextColor(kBlue);
    t1->SetTextSize(0.04);
    t1->Draw("same");
    TText *t2 = new TText(0.65,0.78,"Green: kaon");
    t2->SetNDC(1);
    t2->SetTextColor(kGreen);
    t2->SetTextSize(0.04);
    t2->Draw("same");
  }

  c->cd(3);
  {
    TH1 *h = (TH1*)GHist::get(HistMaker::getUniqueID(kSAC,  0, kTDC, 1));
    h->GetXaxis()->SetRangeUser(250e3,350e3);
    h->Draw();
  }

  c->cd(4);
  {
    TH1 *h = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc,  0, kTDC)+0);
    h->GetXaxis()->SetRangeUser(-6,2);
    h->Draw();
    TH1 *hpi = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+1);
    hpi->SetLineColor( kBlue );
    hpi->Draw("same");
    TH1 *hk = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+2);
    hk->SetLineColor( kGreen );
    hk->Draw("same");
    TH1 *hp = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc, 0, kTDC)+3);
    hp->SetLineColor( kRed );
    hp->Draw("same");
    TLatex *t1 = new TLatex(0.72,0.86,"#pi");
    t1->SetNDC(1);
    t1->SetTextColor(kBlue);
    t1->SetTextSize(0.04);
    t1->Draw("same");
    TText *t2 = new TText(0.55,0.86,"K");
    t2->SetNDC(1);
    t2->SetTextColor(kGreen);
    t2->SetTextSize(0.04);
    t2->Draw("same");
  }

  c->cd(5);
  {
    TH1 *h = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc,  0, kTDC)+23+16);
    h->GetXaxis()->SetRangeUser(0,5);
    h->Draw();
    int bin0 = h->GetBinContent(1);
    int bin1 = h->GetBinContent(2);
    std::stringstream oss;
    oss.str("");
    oss << "Eff[pi]: " << std::fixed << std::setprecision(2) << ((double)bin1/(double)bin0)*100. << "%";
    TText *t1 = new TText(0.5,0.8,oss.str().c_str());
    t1->SetNDC(1);
    t1->SetTextColor(kBlack);
    t1->SetTextSize(0.06);
    t1->Draw("same");
  }

  c->cd(6);
  {
    TH1 *h = (TH1*)GHist::get(HistMaker::getUniqueID(kMisc,  0, kTDC)+24+16);
    h->GetXaxis()->SetRangeUser(0,5);
    h->Draw();
    int bin0 = h->GetBinContent(1);
    int bin1 = h->GetBinContent(2);
    std::stringstream oss;
    oss.str("");
    oss << "Eff[K]: " << std::fixed << std::setprecision(2) << ((double)bin1/(double)bin0)*100. << "%";
    TText *t1 = new TText(0.5,0.8,oss.str().c_str());
    t1->SetNDC(1);
    t1->SetTextColor(kBlack);
    t1->SetTextSize(0.06);
    t1->Draw("same");
  }

  c->Update();

  c->cd(0);

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
