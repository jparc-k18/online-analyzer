// Author: Tomonori Takahashi

#include "JsRootUpdater.hh"

#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <exception>
#include <stdexcept>

#include <TROOT.h>
#include <TThread.h>
#include <TSystem.h>

#include "Main.hh"
#include "lexical_cast.hh"
#include "Controller.hh"

ClassImp(analyzer::JsRootUpdater)

namespace analyzer
{
  namespace
  {
    //_______________________________________________________________________
    void
    thread_function(void* arg)
    {
      JsRootUpdater::getInstance().run();
      return;
    }
  }

  //___________________________________________________________________________
  JsRootUpdater&
  JsRootUpdater::getInstance()
  {
    static JsRootUpdater g_updater;
    return g_updater;
  }

  //___________________________________________________________________________
  JsRootUpdater::JsRootUpdater()
    : m_thread(0),
      m_during_update(false)
  {
  }

  //___________________________________________________________________________
  JsRootUpdater::~JsRootUpdater()
  {
  }

  //___________________________________________________________________________
  bool
  JsRootUpdater::isUpdating()
  {
    TThread::Lock();
    JsRootUpdater& g = JsRootUpdater::getInstance();
    bool ret = g.m_during_update;
    TThread::UnLock();
    return ret;
  }

  //___________________________________________________________________________
  int
  JsRootUpdater::join()
  {
    std::cout << "#D JsRootUpdater::join()" << std::endl;
    return m_thread->Join();
  }

  //___________________________________________________________________________
  int
  JsRootUpdater::run()
  {
    while (!gSystem->ProcessEvents()) {
      // std::cout << "#D JsRootUpdater is running" << std::endl;
      gSystem->Sleep(200);
    }
    std::cout << "#D JsRootUpdater exited loop" << std::endl;
    return 0;
  }

  //___________________________________________________________________________
  void
  JsRootUpdater::start()
  {
    TThread::Lock();
    if (!m_thread) {
      m_thread = new TThread("JsRootUpdaterThread",
			     &thread_function,
			     reinterpret_cast<void*>(0U));
      m_thread->Run();
    }
    TThread::UnLock();
    return;
  }
}
