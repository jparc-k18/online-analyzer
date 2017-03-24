// Author: Tomonori Takahashi

#include "Main.hh"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <sys/time.h>
#include <TThread.h>

#include "UnpackerManager.hh"
#include "user_analyzer.hh"

ClassImp(analyzer::Main)

namespace analyzer
{
  namespace
  {
    typedef hddaq::unpacker::UnpackerManager UnpackerManager;
    typedef hddaq::unpacker::GUnpacker       GUnpacker;
    
    void
    thread_function(void* arg)
    {
      Main::getInstance().run();
      return;
    }
  }
//______________________________________________________________________________
Main& 
Main::getInstance()
{
  static Main g_main;
  return g_main;
};

//______________________________________________________________________________
Main::Main()
  : m_state(k_idle),
    m_argv(),
    m_thread(0),
    m_count(0),
    m_is_overwrite(false),
    m_is_batch(false)
{
}

//______________________________________________________________________________
Main::~Main()
{
//   if (m_thread)
//     delete m_thread;
//   m_thread = 0;
}

//______________________________________________________________________________
void
Main::hoge() const
{
  std::cout << "#D Main::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
const std::vector<std::string>&
Main::getArgv() const
{
  return m_argv;
}

//______________________________________________________________________________
int
Main::getCounter() const
{
  return m_count;
}

//______________________________________________________________________________
// void
// Main::initialize(int argc,
// 		 char* argv[])

// {
//   m_argv.assign(argv, argv+argc);
//   m_count = 0;
//   int ret = process_begin(m_argv);
//   if (ret!=0)
//     {
//       std::cout << "#D Main::initialize()\n"
// 		<< " got non-zero value from process_begin()"
// 		<< "\n  exit"
// 		<< std::endl;
//       std::exit(ret);
//     }
//   hoge();
//   return;
// }

//______________________________________________________________________________
void
Main::initialize(const std::vector<std::string>& argV)
{
//   std::cout << "Main::initialize()" << std::endl;
//   std::copy(argV.begin(), argV.end(), 
// 	    std::ostream_iterator<std::string>(std::cout, " " ));
//   std::cout << std::endl;
  m_argv = argV;
  m_count = 0;
  int ret = process_begin(m_argv);
  if (ret!=0)
    {
      std::cout << "#D Main::initialize()\n"
		<< " got non-zero value from process_begin()"
		<< "\n  exit"
		<< std::endl;
      std::exit(ret);
    }

  return;
}

//______________________________________________________________________________
bool
Main::isBatch() const
{
  return m_is_batch;
}

//______________________________________________________________________________
bool
Main::isForceOverwrite() const
{
  return m_is_overwrite;
}

//______________________________________________________________________________
bool
Main::isIdle() const
{
  return (m_state==k_idle);
}

//______________________________________________________________________________
bool
Main::isRunning() const
{
  return (m_state==k_running);
}

//______________________________________________________________________________
bool
Main::isZombie() const
{
  return (m_state==k_zombie);
}

//______________________________________________________________________________
int
Main::join()
{
  std::cout << "#D Main::join()" << std::endl;
  return m_thread->Join();
}

//______________________________________________________________________________
double 
Main::get_dtime()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return ((double)(tv.tv_sec)+(double)(tv.tv_usec)*0.001*0.001);
}

//______________________________________________________________________________
int
Main::run()
{
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
//   if (g_unpacker.is_online())
  if (!m_is_batch)
    {
      for (;;)
	{
	  g_unpacker.initialize();
	  //	  double d3_last;
	  //	  d3_last = 0;
	  //	  double d4 = 0;
	  //	  int nevt=0;
	  for (;!g_unpacker.eof();++g_unpacker, ++m_count)
	    {
	      //	      double d0_last = get_dtime();
	      //	      if(nevt!=0)
	      //		{
	      //		  d4 += d0_last - d3_last;
	      //		}
	      //	      d3_last = d0_last;
	      if (isZombie())
		break;
	      
	      if (isIdle())
		{
		  for(;;)
		    {
		      if (!isIdle())
			break;
		      continue;
		    }
		}
	      
	      if (isRunning())
		{
		  // TThread::Lock();
		  int ret = process_event();
		  if( ret!=0 ){
		    std::cout << "#D1 analyzer::process_event() return " << ret << std::endl;
		    break;
		  }
		  // TThread::UnLock();
		}
// 	      double d1 = get_dtime();
// 	      d3 += d1 - d0;
//	      nevt++ ;
//	      if(d4>1)
//		{
//		  d4 = 0;
//		  std::cout<<"************************************************"<<std::endl;
//		  std::cout<<std::endl;
//		  std::cout<<std::endl;
//		  std::cout<<"Nevent : "<<nevt<<std::endl;
//		  getchar();
//		  std::cout<<std::endl;
//		  std::cout<<std::endl;
//		  std::cout<<std::endl;
//		  nevt = 0;
//		}
	      //	      std::cout<<"time2:" <<d1 - d0<<std::endl;
	    }
	  
	  std::cout << "#D1 Main::run() exit loop"  << std::endl;
	  if (isZombie())
	    break;

	  if (!g_unpacker.is_online())
	    break;
	}
    }
  else
    {
      g_unpacker.initialize();
      for (;!g_unpacker.eof();++g_unpacker){
	int ret = process_event();
	if( ret!=0 ){
	  std::cout << "#D2 analyzer::process_event() return " << ret << std::endl;
	  break;
	}
      }
      std::cout << "#D2 Main::run() exit loop"  << std::endl;
    }
  process_end();

  std::cout << "#D Main::run() after process_end()"  << std::endl;
  return 0;
}

//______________________________________________________________________________
void
Main::setBatchMode(bool flag)
{
  m_is_batch = flag;
  return;
}

//______________________________________________________________________________
void
Main::setForceOverwrite(bool flag)
{
  m_is_overwrite = flag;
  return;
}

//______________________________________________________________________________
void
Main::start()
{
  if (!m_thread)
    {
      m_thread = new TThread("MainThread",
			     &thread_function,
			     reinterpret_cast<void*>(0U));
      m_thread->Run();
    }
  
  m_state = k_running;

  return;
}

//______________________________________________________________________________
void
Main::stat()
{
  std::cout << "#D Main::stat()" << std::endl;
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
//   std::cout << "#D " << g_unpacker.get_counter() 
// 	    << " events unpacked" << std::endl;
//  TThread::Lock();
  g_unpacker.show_event_number();
//   g_unpacker.show_summary(true);
//  TThread::UnLock();
  return;
}

//______________________________________________________________________________
void
Main::stop()
{
  m_state = k_zombie;
  return;
}

//______________________________________________________________________________
void
Main::suspend()
{
  m_state = k_idle;
  return;
}

}

