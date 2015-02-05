// Author: Tomonori Takahashi

#ifndef HDDAQ__SIGWAIT_H
#define HDDAQ__SIGWIAT_H

#include <string>
#include <vector>

#include "Uncopyable.hh"

namespace analyzer
{

  class Sigwait
    : private hddaq::Uncopyable<Sigwait>
  {

  private:
    std::vector<int>         m_signal_id;
    std::vector<std::string> m_message;
    bool                     m_is_end;
    ::pthread_t*             m_thread;

  public:
    ~Sigwait();

    static Sigwait& getInstance();
    void            handleSignal();
    void            hoge(const std::string& arg="") const;
    bool            is_end() const;
    void            kill();
    int             run();

  private:
    Sigwait();

  };

}
#endif
