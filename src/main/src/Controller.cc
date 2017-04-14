// -*- C++ -*-
// Author: Tomonori Takahashi

#include "Controller.hh"

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>

#include <TROOT.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGTextEntry.h>

#include <TRootBrowser.h>
#include <TBrowserImp.h>
#include <TBrowser.h>
#include <TGFileBrowser.h>
#include <TThread.h>
#include <TFolder.h>
#include <TCanvas.h>

#include "lexical_cast.hh"
#include "filesystem_util.hh"
#include "Main.hh"
#include "Updater.hh"

ClassImp(hddaq::gui::UpdateInterval)
ClassImp(hddaq::gui::Controller)

namespace hddaq
{
  namespace gui
  {
    namespace
    {
      using analyzer::Main;
    }

//______________________________________________________________________________
UpdateInterval::UpdateInterval(const TGWindow* window)
  : TGGroupFrame(window, "Interval",  kHorizontalFrame),
    m_combo(0),
    m_text(0)
{
  TGHorizontalFrame* hf = new TGHorizontalFrame(this);
  AddFrame(hf, new TGLayoutHints(kLHintsLeft));

  m_text = new TGTextEntry(hf);
//   hf->AddFrame(new TGLabel(hf, "Interval: "),
// 	       new TGLayoutHints(kLHintsCenterY|kLHintsLeft));//, 2, 2, 2 ,2));
  hf->AddFrame(m_text,
	       new TGLayoutHints(kLHintsLeft));//, 2, 2, 2 ,2));
  m_text->SetToolTipText("set update interval [second / event]");
  m_text->Resize(100, m_text->GetDefaultHeight());
  m_text->Connect("ReturnPressed()",
		  "hddaq::gui::UpdateInterval", this, "setInterval()");
  double interval = Updater::getInstance().getInterval();
  m_text->SetText(d2a(interval).c_str());

  m_combo = new TGComboBox(hf);
  hf->AddFrame(m_combo,
	       new TGLayoutHints(kLHintsLeft | kLHintsExpandY));//, 2, 2, 2, 2));
  m_combo->Resize(80, m_combo->GetDefaultHeight());
  m_combo->AddEntry("seconds", Updater::k_seconds);
  m_combo->AddEntry("events",  Updater::k_events);
  m_combo->AddEntry("clicked", Updater::k_clicked);
  m_combo->Connect("Selected(Int_t)",
		   "hddaq::gui::UpdateInterval", this, "setMode()");
//   m_combo->Select(Updater::k_seconds);
  m_combo->Select(Updater::k_clicked);


}

//______________________________________________________________________________
UpdateInterval::~UpdateInterval()
{
}

//______________________________________________________________________________
void
UpdateInterval::setInterval()
{
  double interval = a2d(m_text->GetText());
  if (m_combo->GetSelected()==Updater::k_events)
    {
      interval = static_cast<int>(interval);
      m_text->SetText(d2a(interval).c_str());
    }
  Updater::getInstance().setInterval(interval);
  return;
}

//______________________________________________________________________________
void
UpdateInterval::setMode()
{
  const int mode = m_combo->GetSelected();
  if ((mode==Updater::k_clicked))
    {
      m_text->SetEnabled(false);
//       m_default_color = m_text->GetTextColor();
//       TColor* c = gROOT->GetColor(kGray);
//       m_text->SetTextColor(c, true);
    }
  else
    m_text->SetEnabled(true);
  Updater::getInstance().setUpdateMode(mode);
  setInterval();
  return;
}

//______________________________________________________________________________
void
UpdateInterval::setTextColor(TColor* color)
{
  if (!color)
    color = gROOT->GetColor(kBlack);
  m_text->SetTextColor(color, true);
  return;
}

//______________________________________________________________________________
//______________________________________________________________________________
Controller::Controller()
  :
    m_frame(0),
    m_command(k_n_command),
    m_interval(0),
    m_browser(0)
{
  ::pid_t pid = ::getpid();
  const std::string host = std::getenv("HOSTNAME");
  const std::string user = std::getenv("USER");
  const std::vector<std::string>& argv = Main::getInstance().getArgv();
  const std::string proc = hddaq::basename(argv[0]);
  const std::string windowName
    = proc + " " + user + "@" + host + "(pid = " + i2a(pid) + ")";

//   m_browser = new TRootBrowser(0, windowName.c_str(), 1024, 738, "CI");
  //m_browser = new TRootBrowser(0, windowName.c_str(), 1024, 738, "I");
  m_browser = new TRootBrowser(0, windowName.c_str(), 1200, 800, "I");

  m_browser->StartEmbedding(TRootBrowser::kBottom);
  m_frame = new TGMainFrame(gClient->GetRoot());
  m_browser->StopEmbedding("control");
  //  m_browser->SetIconPixmap(axis_logo);
  m_frame->SetCleanup(kDeepCleanup);

  TGFileBrowser* fb = makeFileBrowser("Files");
  fb->BrowseObj(gROOT);
  fb->Show();

  m_browser->StartEmbedding(TRootBrowser::kRight);
  new TCanvas("c1");
  m_browser->StopEmbedding("c1");
  m_browser->StartEmbedding(TRootBrowser::kRight);
  new TCanvas("c2");
  m_browser->StopEmbedding("c2");
  m_browser->StartEmbedding(TRootBrowser::kRight);
  new TCanvas("c3");
  m_browser->StopEmbedding("c3");
  m_browser->StartEmbedding(TRootBrowser::kRight);
  new TCanvas("c4");
  m_browser->StopEmbedding("c4");
  m_browser->StartEmbedding(TRootBrowser::kRight);
  new TCanvas("c5");
  m_browser->StopEmbedding("c5");


  initialize();
}

//______________________________________________________________________________
Controller::~Controller()
{
  delete m_frame;
  m_frame = 0;

  std::exit(1);
}

//______________________________________________________________________________
void
Controller::hoge() const
{
  std::cout << "#D Controller::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
void
Controller::disableCommand(e_command command)
{
  m_command[command]->SetState(kButtonDisabled);
  return;
}

//______________________________________________________________________________
void
Controller::enableCommand(e_command command)
{
  m_command[command]->SetState(kButtonUp);
  return;
}

//______________________________________________________________________________
void
Controller::exit()
{
//   suspend();
  stop();
  ::usleep(1000000);
  std::cout << "#D Controller::exit()" << std::endl;
  std::exit(0);
  return;
}

//______________________________________________________________________________
TRootBrowser*
Controller::getBrowser() const
{
  return m_browser;
}

//______________________________________________________________________________
Controller&
Controller::getInstance()
{
  static Controller g_instance;
  return g_instance;
}

//______________________________________________________________________________
void
Controller::initialize()
{
  // Command Button Widgets
  TGHorizontalFrame* h_frame = new TGHorizontalFrame(m_frame);

  m_command[k_start] = new TGTextButton(h_frame, "Start");
  m_command[k_start]
    ->Connect("Clicked()", "hddaq::gui::Controller", this, "start()");
  h_frame->AddFrame(m_command[k_start],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  m_command[k_suspend] = new TGTextButton(h_frame, "Suspend");
  m_command[k_suspend]
    ->Connect("Clicked()", "hddaq::gui::Controller", this, "suspend()");
  h_frame->AddFrame(m_command[k_suspend],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  m_command[k_refresh] = new TGTextButton(h_frame, "Refresh");
  m_command[k_refresh]
    ->Connect("Clicked()", "hddaq::gui::Controller", this, "refresh()");
  h_frame->AddFrame(m_command[k_refresh],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  m_command[k_stat] = new TGTextButton(h_frame, "stat");
  m_command[k_stat]
    ->Connect("Clicked()", "hddaq::gui::Controller", this, "stat()");
  h_frame->AddFrame(m_command[k_stat],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));


  m_command[k_reset] = new TGTextButton(h_frame, "Reset");
  m_command[k_reset]
    ->Connect("Clicked()", "hddaq::gui::Controller", this, "reset()");
  h_frame->AddFrame(m_command[k_reset],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));


  m_command[k_resetAll] = new TGTextButton(h_frame, "ResetAll");
  m_command[k_resetAll]
    ->Connect("Clicked()", "hddaq::gui::Controller", this, "resetAll()");
  h_frame->AddFrame(m_command[k_resetAll],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  m_command[k_exit] = new TGTextButton(h_frame, "Exit");
  m_command[k_exit]->Connect("Clicked()", "hddaq::gui::Controller",
			     this, "exit()");
  h_frame->AddFrame(m_command[k_exit],
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  m_frame->AddFrame(h_frame,
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  // Update Interval Combo-Text widget
  m_interval = new UpdateInterval(m_frame);
  m_frame->AddFrame(m_interval,
		    new TGLayoutHints(kLHintsLeft));//, 1, 1, 1, 1));

  m_browser->Connect("CloseWindow()", "hddaq::gui::Controller", this, "exit()");
  m_frame->MapSubwindows();
  m_frame->Resize(m_frame->GetDefaultSize());
  m_frame->MapWindow();

  m_command[k_start]->SetState(kButtonDisabled);
  return;
}

//______________________________________________________________________________
TGFileBrowser*
Controller::makeFileBrowser(const std::string& name)
{
  m_browser->StartEmbedding(TRootBrowser::kLeft);
  TBrowserImp imp;
  TBrowser *tb = new TBrowser("tb", "tmp", &imp);
  TGFileBrowser *fb = new TGFileBrowser(gClient->GetRoot(), tb, 200, 500);
  m_browser->StopEmbedding(name.c_str());
  tb->SetBrowserImp(dynamic_cast<TBrowserImp*>(m_browser));
  fb->SetBrowser(tb);
  fb->SetNewBrowser(m_browser);
  gROOT->GetListOfBrowsers()->Remove(tb);
  return fb;
}

//______________________________________________________________________________
void
Controller::refresh() const
{
  m_command[k_refresh]->SetState(kButtonDisabled);
  Updater::getInstance().refresh();
  m_command[k_reset]->SetState(kButtonUp);
  return;
}


//______________________________________________________________________________
void
Controller::reset() const
{
  m_command[k_reset]->SetState(kButtonDisabled);
//   std::cout << "#D Controller::reset()" << std::endl;
  Updater::getInstance().reset();
  m_command[k_reset]->SetState(kButtonUp);
  return;
}

//______________________________________________________________________________
void
Controller::resetAll() const
{
  m_command[k_resetAll]->SetState(kButtonDisabled);
  Updater::getInstance().resetAll();
  m_command[k_resetAll]->SetState(kButtonUp);
}

//______________________________________________________________________________
void
Controller::setIntervalTextColor(TColor* color)
{
  m_interval->setTextColor(color);
  return;
}

//______________________________________________________________________________
void
Controller::start() const
{
  m_command[k_start]->SetState(kButtonDisabled);
//   m_command[k_resume]->SetState(kButtonDisabled);
  std::cout << "#D Controller::start()" << std::endl;

  m_command[k_suspend]->SetState(kButtonUp);
  Main::getInstance().start();
//   m_command[k_start]->SetState(kButtonUp);

  return;
}

//______________________________________________________________________________
void
Controller::stat() const
{
  m_command[k_stat]->SetState(kButtonDisabled);
  std::cout << "#D Controller::stat()" << std::endl;

  Main::getInstance().stat();
  m_command[k_stat]->SetState(kButtonUp);
  return;
}

//______________________________________________________________________________
void
Controller::stop() const
{
  m_command[k_suspend]->SetState(kButtonDisabled);
  std::cout << "#D Controller::stop()" << std::endl;
  Updater::getInstance().stop();
  Main::getInstance().stop();
  return;
}

//______________________________________________________________________________
void
Controller::suspend() const
{
  m_command[k_suspend]->SetState(kButtonDisabled);
//   m_command[k_resume]->SetState(kButtonUp);
  m_command[k_start]->SetState(kButtonUp);
  std::cout << "#D Controller::suspend()" << std::endl;
  Main::getInstance().suspend();
  return;
}

  }
}
