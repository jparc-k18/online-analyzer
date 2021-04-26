// Author: Tomonori Takahashi

#ifndef HDDAQ__JSROOT_UPDATER_H
#define HDDAQ__JSROOT_UPDATER_H

#include <Rtypes.h>
#include <set>

class TThread;

namespace analyzer
{
  class JsRootUpdater
  {
  private:
    int      m_locked;
    TThread* m_thread;
    bool     m_during_update;

  public:
    static JsRootUpdater& getInstance();
    virtual ~JsRootUpdater();

    static bool isUpdating();
    int    join();
    void   lock();
    int    run();
    void   start();
    void   unlock();

  private:
    JsRootUpdater();
    JsRootUpdater(const JsRootUpdater&);
    JsRootUpdater& operator=(const JsRootUpdater&);

    ClassDef(analyzer::JsRootUpdater, 0)
  };

  //___________________________________________________________________________
  inline JsRootUpdater&
  JsRootUpdater::getInstance()
  {
    static JsRootUpdater g_updater;
    return g_updater;
  }
}

#endif
