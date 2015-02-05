// Author: Tomonori Takahashi

#ifndef HDDAQ__UPDATER_H
#define HDDAQ__UPDATER_H

#include <Rtypes.h>
#include <set>

class TThread;
class TMutex;


namespace hddaq
{
  namespace gui
  {

  class Updater
  {

  public:
    enum e_state
      {
	k_running,
	k_idle,
	k_zombie,
	k_n_state
      };

    enum e_mode
      {
	k_seconds,
	k_events,
	k_clicked,
	k_n_mode
      };


  private:
    double   m_refresh_interval;
    int      m_mode;
    int      m_locked;
    e_state  m_state;
    TThread* m_thread;
    bool     m_during_update;
    //    TMutex*  m_mutex;
    
  public:
    static Updater& getInstance();
    virtual ~Updater();

    static bool isUpdating();
    void   hoge() const;
    int    getCounter() const;
    double getInterval() const;
    bool   isIdle() const;
    bool   isRunning() const;
    bool   isZombie() const;
    int    join();
    //    static int lock();
    void   refresh();
    void   reset();
    void   resetAll();
    int    run();
    void   setInterval(double interval);
    void   setUpdateMode(int mode);
    static void setUpdating(bool flag);
    void   start();
    void   stop();
    //    static int unlock();
    void   update();
    int    wait();

  private:
    Updater();
    Updater(const Updater&);
    Updater& operator=(const Updater&);

    ClassDef(hddaq::gui::Updater, 0)

  };

  }
}
#endif
