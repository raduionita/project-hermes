#ifndef __os_hpp__
#define __os_hpp__

#include <core.hpp>

#include <conio.h>

namespace os
{
  inline int pause(void)
  {
    printf("Press any key to continue...");
    _getch();
    printf("\n");
    return 0;
  }
  
  std::string getcwd()
  {
#ifdef _WIN32_WINNT
    char path[MAX_PATH+1];
    ::GetCurrentDirectory(sizeof(path), path);
    //path[MAX_PATH] = '\0';
    return path;
#elif // LINUX
    // @todo...for LINUX...duh
#endif // _WIN32_WINNT
  }
}

#endif // __os_hpp__
