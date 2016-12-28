#ifndef __net_cerror_hpp__
#define __net_cerror_hpp__

#include <net.hpp>
#include <core/CError.hpp>

namespace net
{
  class AServer;

  class CError : public core::CError
  {
    friend CError& getError();

    public:
    using core::CError::CError;

    enum EErrno
    {
      ADDRINUSE       = NETEADDRINUSE,
      AGAIN           = NETEAGAIN,
      WOULDBLOCK      = NETEWOULDBLOCK,
      ALREADY         = NETEALREADY,
      ISCONN          = NETEISCONN,
      INVAL           = NETEINVAL,
      INPROGRESS      = NETEINPROGRESS,
      CONNRESET       = NETECONNRESET,
      ADDRNOTAVAIL    = NETEADDRNOTAVAIL,
      AFNOSUPPORT     = NETEAFNOSUPPORT,
      CONNABORTED     = NETECONNABORTED,
      CONNREFUSED     = NETECONNREFUSED,
      DESTADDRREQ     = NETEDESTADDRREQ,
      FAULT           = NETEFAULT,
      HOSTDOWN        = NETEHOSTDOWN,
      HOSTUNREACH     = NETEHOSTUNREACH,
      MFILE           = NETEMFILE,
      MSGSIZE         = NETEMSGSIZE,
      NETDOWN         = NETENETDOWN,
      NETRESET        = NETENETRESET,
      NETUNREACH      = NETENETUNREACH,
      NOBUFS          = NETENOBUFS,
      NOTCONN         = NETENOTCONN,
      NOTSOCK         = NETENOTSOCK,
      PFNOSUPPORT     = NETEPFNOSUPPORT,
      SHUTDOWN        = NETESHUTDOWN,
      TIMEDOUT        = NETETIMEDOUT,
#undef HOST_NOT_FOUND
      HOST_NOT_FOUND  = NETEHOST_NOT_FOUND,
#undef NO_RECOVERY
      NO_RECOVERY     = NETENO_RECOVERY,
#undef TRY_AGAIN
      TRY_AGAIN       = NETETRY_AGAIN,
      // windows specific
      PROCLIM         = NETEPROCLIM,
      NOTINITIALISED  = NETENOTINITIALISED,
      SYSNOTREADY     = NETESYSNOTREADY,
      VERNOTSUPPORTED = NETEVERNOTSUPPORTED
    };
  };

  inline CError& getError() // careful with CError&, the reference might get lost or something...
  {
    static CError error;

#ifdef _WIN32_WINNT
    int   code    = ::WSAGetLastError();
    char* message = ::gai_strerror(code);
#else // LINUX
    int   code    = errno;
    char* message = ::strerror(code);
#endif // _WIN32_WINNT

    error.mCode    = code;
    error.mMessage = message;

    return error;
  }
}


#endif // __net_cerror_hpp__
