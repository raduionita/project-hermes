#ifndef __net_hpp__
#define __net_hpp__

#include <cstdio>
#include <exception>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif
void WSAAPI freeaddrinfo( struct addrinfo* );
int WSAAPI getaddrinfo( const char*, const char*, const struct addrinfo*, struct addrinfo** );
int WSAAPI getnameinfo( const struct sockaddr*, socklen_t, char*, DWORD, char*, DWORD, int );
#ifdef __cplusplus
}
#endif

/**
 *
 */
std::size_t strlcpy(char * __restrict dst, const char * __restrict src, size_t siz)
{
  char *d = dst;
  const char *s = src;
  size_t n = siz;

  /* Copy as many bytes as will fit */
  if (n != 0) {
    while (--n != 0) {
      if ((*d++ = *s++) == '\0')
        break;
    }
  }

  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0) {
    if (siz != 0)
      *d = '\0';    /* NUL-terminate dst */
    while (*s++)
      ;
  }

  return(s - src - 1);  /* count does not include NUL */
}

/**
 * Format an IPv4 address
 *
 * @notes  (1) uses no statics (2) takes a u_char* not an in_addr as input
 * @author Paul Vixie, 1996.
 *
 * @param 
 * @param 
 * @param 
 * @return char* `dst' (as a const)
 */
static char* inet_ntop4(const u_char *src, char *dst, socklen_t size)
{
  static const char fmt[128] = "%u.%u.%u.%u";
  char tmp[sizeof "255.255.255.255"];
  int l;

  l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
  // std::sprintf( tmp, fmt, src[0], src[1], src[2], src[3] ); // **** vc++ does not have snprintf
  if (l <= 0 || (socklen_t) l >= size) {
    return (NULL);
  }
  strlcpy(dst, tmp, size);
  return (dst);
}

/**
 * Convert IPv6 binary address into presentation (printable) format
 *
 * @author Paul Vixie, 1996.
 *
 * @param
 * @param
 * @param
 * @return char*
 */
static char* inet_ntop6(const u_char *src, char *dst, socklen_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size.  On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays.  All the world's not a VAX.
   */
  char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
  struct { int base, len; } best, cur;
#define NS_IN6ADDRSZ  16
#define NS_INT16SZ  2
  u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
  int i;

  /*
   * Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(words, '\0', sizeof words);
  for (i = 0; i < NS_IN6ADDRSZ; i++)
    words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
  best.base = -1;
  best.len = 0;
  cur.base = -1;
  cur.len = 0;
  for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
    if (words[i] == 0) {
      if (cur.base == -1)
        cur.base = i, cur.len = 1;
      else
        cur.len++;
    } else {
      if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
          best = cur;
        cur.base = -1;
      }
    }
  }
  if (cur.base != -1) {
    if (best.base == -1 || cur.len > best.len)
      best = cur;
  }
  if (best.base != -1 && best.len < 2)
    best.base = -1;

  /*
   * Format the result.
   */
  tp = tmp;
  for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    if (best.base != -1 && i >= best.base &&
        i < (best.base + best.len)) {
      if (i == best.base)
        *tp++ = ':';
      continue;
    }
    /* Are we following an initial run of 0x00s or any real hex? */
    if (i != 0)
      *tp++ = ':';
    /* Is this address an encapsulated IPv4? */
    if (i == 6 && best.base == 0 && (best.len == 6 ||
        (best.len == 7 && words[7] != 0x0001) ||
        (best.len == 5 && words[5] == 0xffff))) {
      if (!inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp)))
        return (NULL);
      tp += strlen(tp);
      break;
    }
    tp += std::sprintf(tp, "%x", words[i]); // ****
  }
  /* Was it a trailing run of 0x00's? */
  if (best.base != -1 && (best.base + best.len) == 
      (NS_IN6ADDRSZ / NS_INT16SZ))
    *tp++ = ':';
  *tp++ = '\0';

  /*
   * Check for overflow, copy, and we're done.
   */
  if ((socklen_t)(tp - tmp) > size) {
    return (NULL);
  }
  strcpy(dst, tmp);
  return (dst);
}

/**
 * Convert a network format address to presentation format.
 * @return char* pointer to presentation format address (`dst'), or NULL (see errno).
 * @author       Paul Vixie, 1996.
 */
char* inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
  switch (af) 
  {
    case AF_INET : return (inet_ntop4( (unsigned char*)src, (char*)dst, size)); // ****
    case AF_INET6: return (char*)(inet_ntop6( (unsigned char*)src, (char*)dst, size)); // ****
    default      : /* return (NULL); */ return 0;
  }
  /* NOTREACHED */
}

void* get_in_addr(struct sockaddr* sa)  // get sockaddr - IPv4 or IPv6
{
  if(sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

namespace net
{
  typedef unsigned short ushort;
  typedef unsigned int   uint;
  
  typedef struct addrinfo         addrinfo_t;
  
  typedef struct sockaddr         sockaddr_t;
  typedef struct sockaddr_in      sockaddr_in_t;
  typedef struct sockaddr_in6     sockaddr_in6_t;
  typedef struct sockaddr_storage sockaddr_storage_t; // large enoght to hold sockaddr_in or sockaddr_in6
  
  typedef SOCKET                  socket_t;
  
  typedef struct timeval timeval_t;
  
  enum EStatus
  {
    BAD  = SOCKET_ERROR,
    GOOD = 0,
  };
  
  enum class EProtocol : int
  {
    ICMP   = IPPROTO_ICMP,    // 1   - SOCK_RAW
    IGMP   = IPPROTO_IGMP,    // 2   - SOCK_RAY
    //BTH  = BTHPROTO_RFCOMM, // 3   - SOCK_STREAM + AF_BTH
    TCP    = IPPROTO_TCP,     // 6   - SOCK_STREAM
    UDP    = IPPROTO_UDP,     // 17  - SOCK_DGRAM
    ICMPV6 = IPPROTO_ICMPV6,  // 58  - SOCK_RAY
    //RM    = IPPROTO_RM,     // 255 - SOCK_RDM / called IPPROTO_PGM on Vista
  };
  
  enum class ESocketType : int
  {
    TCP = SOCK_STREAM,   // 1 - sequenced, reliable, 2-way, connection-based stream w/ OBB data transmission
    UDP = SOCK_DGRAM,    // 2 - connectionless
    RAW = SOCK_RAW,      // 3 - manipulate uppper-layer protocol header - IP/IP6__HDRINCL setsockopt must be included
    RDM = SOCK_RDM,      // 4 - reliable message datagram, eg. Pragmatic General Multicast(PGM) protocol on Win32
    SEQ = SOCK_SEQPACKET // 5 - pseudo-stream packet based on datagrams
  };
  
  enum EAddressType // address familly
  {
    UNSPEC    = AF_UNSPEC,    // 0  - unspecified
    IPV4      = AF_INET,      // 2  - IPv4
    IPX       = AF_IPX,       // 6  - IPX/SPX - NWLink NetBIOS Compatible Transport Protocol
    APPLETALK = AF_APPLETALK, // 16 - AppleTalk protocol
    NETBIOS   = AF_NETBIOS,   // 17 - works with SOCK_DGRAM(UDP)
    IPV6      = AF_INET6,     // 23 - IPv6
    IRDA      = AF_IRDA,      // 26 - infrared
    //BTH       = AF_BTH,     // 32 - bluetooth
  };
  
  class CAddress
  {
    public:
    static constexpr const char* LOCALHOST = NULL;
    
    std::string  mAddress;
    EAddressType mAddressType;
    
    public:
    CAddress(const char* address)
    {
      if(address)
        mAddress = address;
    }
    
    CAddress(std::string& address) : mAddress(address)
    {
      // TODO: address type detection
    }
    
    operator const char* () const
    {
      if(mAddress.empty())
        return NULL;
      return mAddress.c_str();
    }
    
    operator char* ()
    {
      if(mAddress.empty())
        return NULL;
      return const_cast<char*>(mAddress.c_str());
    }
  };

  class CMessage
  {
    public:
    std::string mMessage;
  };
  
  class CInput
  {
    public:
    net::CAddress mAddress;
    net::CMessage mMessage;
    uint          mTimestamp;
  };
  
  class COutput
  {
    public:
    net::CAddress mAddress;
    net::CMessage mMessage;
    uint          mTimestamp;
  };
  
  class CMessageHistory
  {
    friend class CMessage;
    
    protected:
    std::map<CMessage*, int> mMessages; // message and descriptor(user, date...)
    
    public:
    CMessage* pull()
    {
      return nullptr;
    }
    
    void push(CMessage* message)
    {
      // automaticaly save to file after n messages 
    }
    
    protected:
    void save()
    {
      
    }
  };
  
  class CLogger
  {
    
  };

  class CException : public std::exception
  {
    protected:
    std::string mMessage;
    std::string mFile;
    size_t      mLine;
    
    public:
    CException() : std::exception()
    {
    
    }
    
    CException(const std::string& message) 
    : std::exception(), mMessage(message)
    {
      
    }
    
    CException(const std::string& message, const std::string& file, size_t line) 
    : std::exception(), mMessage(message), mFile(file), mLine(line)
    {
      
    }
    
    CException(const std::string& file, size_t line) 
    : std::exception(), mFile(file), mLine(line)
    {
      
    }
    
    CException& operator << (const std::string& message)
    {
      mMessage.append(message);
      return *this;
    }
    
    public:
    virtual const char* what() const throw()
    {
      return mMessage.c_str();
    }
    
    CException& setMessage(const std::string& message)
    {
      mMessage = message;
      return *this;
    }
  
    CException& setFile(const std::string& file)
    {
      mFile = file;
      return *this;
    }
    
    CException& setLine(size_t line)
    {
      mLine = line;
      return *this;
    }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  int getSocketType(int protocol)
  {
    switch(protocol)
    {
      case IPPROTO_ICMP  : return SOCK_RAW;
      case IPPROTO_IGMP  : return SOCK_RAW;
      case IPPROTO_TCP   : return SOCK_STREAM;
      case IPPROTO_UDP   : return SOCK_DGRAM;
      case IPPROTO_ICMPV6: return SOCK_RAW;
      default: return 0;
    }
  }
  
  int getSocketType(net::EProtocol protocol)
  {
    return getSocketType((int)(protocol));
  }
  
  char* getErrorString(int error)
  {
    LPSTR _out = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  _out, 
                  0,
                  NULL);
    return _out;
  }
}

#endif // __net_hpp__
