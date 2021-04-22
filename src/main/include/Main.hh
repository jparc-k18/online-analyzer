// Author: Tomonori Takahashi

#ifndef ANALYZER_MAIN_H
#define ANALYZER_MAIN_H

#include <string>
#include <vector>

#include <Rtypes.h>

class TThread;

namespace analyzer
{

  class Main
  {
  public:
    enum e_state
      {
	k_running,
	k_idle,
	k_zombie,
	k_n_state
      };

  private:
    e_state                  m_state;
    std::vector<std::string> m_argv;
    TThread*                 m_thread;
    int                      m_count;
    bool                     m_is_overwrite;
    bool                     m_is_batch;
    bool                     m_is_jsroot;

  public:
    static Main& getInstance();
    virtual ~Main();

    void hoge() const;
    const std::vector<std::string>& getArgv() const;
    int  getCounter() const;
//     void initialize(int argc,
// 		    char* argv[]);
    void initialize(const std::vector<std::string>& argV);
    bool isBatch() const;
    bool isForceOverwrite() const;
    bool isIdle() const;
    bool isJsRoot() const;
    bool isRunning() const;
    bool isZombie() const;
    int  join();
    int  run();
    void setBatchMode(bool flag);
    void setForceOverwrite(bool flag);
    void start();
    void stat();
    void stop();
    void suspend();
    double get_dtime();

  private:
    Main();
    Main(const Main&);
    Main& operator=(const Main&);

    ClassDef(analyzer::Main, 0)

  };

}

#endif
