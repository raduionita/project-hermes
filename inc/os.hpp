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
}

#endif // __os_hpp__
