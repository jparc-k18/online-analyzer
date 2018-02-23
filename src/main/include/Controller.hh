// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__GUI_CONTROLLER_H
#define HDDAQ__GUI_CONTROLLER_H

#include <vector>

#include <TGFrame.h>

class TGTextButton;
class TGTextEntry;
class TGComboBox;
class TColor;
class TRootBrowser;
class TGFileBrowser;

namespace hddaq
{
  namespace gui
  {

    class UpdateInterval
      : public TGGroupFrame
//       : public TGCompositeFrame
    {

    private:
      TGComboBox*  m_combo;
      TGTextEntry* m_text;

    public:
      UpdateInterval(const TGWindow* window);
      virtual ~UpdateInterval();

      void setInterval();
      void setMode();
      void setTextColor(TColor* color=0);

      ClassDef(hddaq::gui::UpdateInterval, 0)

    };


    //__________________________________________________________________________

    class Controller
    {

    public:
      enum e_command
	{
	  k_start,
	  k_suspend,
	  k_refresh,
	  k_stat,
	  k_logy,
	  k_reset,
	  k_resetAll,
	  k_exit,
	  k_n_command
	};

    private:
      TGCompositeFrame*          m_frame;
      std::vector<TGTextButton*> m_command;
      UpdateInterval*            m_interval;
      TRootBrowser*              m_browser;

    public:
      static Controller& getInstance();
      virtual ~Controller();

      void hoge() const;
      void disableCommand(e_command command);
      void enableCommand(e_command command);
      void exit();
      TRootBrowser* getBrowser() const;
      void initialize();
      TGFileBrowser* makeFileBrowser(const std::string& name);
      void refresh() const;
      void reset() const;
      void resetAll() const;
      void setIntervalTextColor(TColor* color=0);
      void start() const;
      void stat() const;
      void stop() const;
      void suspend() const;

    private:
      Controller();
      Controller(const Controller&);
      Controller& operator=(const Controller&);

      ClassDef(hddaq::gui::Controller,0)

    };

  }
}
#endif
