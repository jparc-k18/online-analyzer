// Autohr: Tomonori Takahahi

#include "Sigwait.hh"

#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <cstdlib>

#include <TFile.h>

#include "user_analyzer.hh"

namespace analyzer
{

  namespace 
  {

//______________________________________________________________________________
    void* startSignalHandler(void* arg)
    {
      Sigwait& g_sigwait = Sigwait::getInstance();
      g_sigwait.handleSignal();
      return &g_sigwait;
    };

  }

//______________________________________________________________________________
Sigwait::Sigwait()
  : m_signal_id(),
    m_message(),
    m_is_end(false),
    m_thread(0)
{
  m_signal_id.push_back(SIGINT);
  m_message.push_back("\n#E Sigwait: Got SIGINT");

  m_signal_id.push_back(SIGHUP);
  m_message.push_back("\n#E Sigwait: Got SIGHUP");

//   m_signal_id.push_back(SIGTERM);
//   m_message.push_back("\n#E Sigwait: Got SIGTERM");

  m_signal_id.push_back(SIGQUIT);
  m_message.push_back("\n#E Sigwait: Got SIGQUIT");

}

//______________________________________________________________________________
Sigwait::~Sigwait()
{
  if (m_thread)
    delete m_thread;
  m_thread = 0;
}

//______________________________________________________________________________
Sigwait&
Sigwait::getInstance()
{
  static Sigwait s_instance;
  return s_instance;
}

//______________________________________________________________________________
void
Sigwait::handleSignal()
{
  ::sigset_t signal_set;
  int sig;
  ::sigemptyset(&signal_set);
  for (unsigned int i=0; i<m_signal_id.size(); ++i)
    ::sigaddset(&signal_set, m_signal_id[i]);
  ::sigprocmask(SIG_BLOCK, &signal_set, 0);

  while (!m_is_end)
    {
      if (::sigwait(&signal_set, &sig))
	{
	  std::cout << "#D Sigwait: not SIGINT n" << std::endl;
	  continue;
	}
      for (unsigned int i=0; i<m_signal_id.size(); ++i)
	{
	  if (m_signal_id[i]!=sig)
	    continue;

	  std::cerr << m_message[i] << " (signal = " << m_signal_id[i] << ")"
		    << std::endl;

	  closeTFile();
	  m_is_end = true;
	  std::cout << "#D exit by signal" << std::endl;
	  kill();
	  std::exit(1);
	  
	  break;
	}
    }
  return;
}

//______________________________________________________________________________
void 
Sigwait::hoge(const std::string& arg) const
{
  std::cout << "#D Sigwait::hoge(" << arg << ")" << std::endl;
  return;
}

//______________________________________________________________________________
bool
Sigwait::is_end() const
{
  return m_is_end;
}

//______________________________________________________________________________
void
Sigwait::kill()
{
  m_is_end = true;
  ::pthread_kill(*m_thread, SIGTERM);
  if (m_thread)
    delete m_thread;
  m_thread = 0;
  return;
}

//______________________________________________________________________________
int
Sigwait::run()
{
  ::sigset_t signal_set;
  ::sigemptyset(&signal_set);
  for (unsigned int i=0; i<m_signal_id.size(); ++i)
    ::sigaddset(&signal_set, m_signal_id[i]);
  ::sigprocmask(SIG_BLOCK, &signal_set, 0);

  m_thread = new ::pthread_t;
  ::pthread_attr_t attr;
  ::pthread_attr_init(&attr);
  ::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ::pthread_create(m_thread, &attr, startSignalHandler, 0);

  return 0;

}

}
