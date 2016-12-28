#ifndef __core_capp_hpp__
#define __core_capp_hpp__

#include <core/CEventEmiter.hpp>

#ifdef _WIN32_WINNT
#include <windows.h>
#endif // _WINNT_

namespace core
{
  BOOL WINAPI onCtrl(DWORD dwCtrlType);

  class CApp : public CEventEmiter
  {
    public:
    static CApp* sInstance;

    public:
    CApp()
    {
      log::debug << "core::CApp::CApp()" << log::endl;
      sInstance = this;
      ::SetConsoleCtrlHandler((PHANDLER_ROUTINE) &onCtrl, TRUE);
    }

    virtual ~CApp()
    {
      log::debug << "core::CApp::~CApp()" << log::endl;
    }

    virtual void close()
    {
      log::debug << "core::CApp::close()" << log::endl;
      //server.close();
    }
  };
  CApp* CApp::sInstance = nullptr;

  BOOL WINAPI onCtrl(DWORD dwCtrlType) // control signal type
  {
    switch(dwCtrlType)
    {
      case CTRL_C_EVENT:
      case CTRL_BREAK_EVENT:
      case CTRL_CLOSE_EVENT:
        if(CApp::sInstance != nullptr)
          CApp::sInstance->close();
        // CEventEmmiter::emit(new CCloseEvent());
      break;
    }
    return TRUE;
  }
}

#endif // __core_capp_hpp__
