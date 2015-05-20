// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

void clear_all_canvas()
{
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  const int canvas_num = 5;
  for(int i=0; i<canvas_num; i++){
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", i+1));
    c->Clear();
    c->Update();
  }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
