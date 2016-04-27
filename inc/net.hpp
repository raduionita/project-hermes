#ifndef __net_hpp__
#define __net_hpp__

#include <core.hpp>

#include <ws2tcpip.h>
#include <errno.h>
#include <string>
#include <cstring>

void* get_in_addr(struct sockaddr* sa);
char* inet_ntop(int af, const void *src, char *dst, socklen_t size);
char* inet_ntop4(const u_char *src, char *dst, socklen_t size);
char* inet_ntop6(const u_char *src, char *dst, socklen_t size);

#define SOCKET_OK     0
#define STATUS_OK     0
#define STATUS_CLOSED 0
#define RESULT_BAD   -1
#define RESULT_OK     0
#define RESULT_CLOSED 0
#define RESULT_EMPTY  0
#define NOTHING       0
#define ZERO          0

namespace net
{
  using namespace core;

  using port_t  = ushort;
  using host_t  = std::string;

  using status_t   = int;
  using result_t   = int;
  using error_t    = int;
  
  using socket_t           = SOCKET;                    // unsigned int
  using addrinfo_t         = struct addrinfo;
  using sockaddr_t         = struct sockaddr;
  using sockaddr_in_t      = struct sockaddr_in;
  using sockaddr_in6_t     = struct sockaddr_in6;
  using sockaddr_storage_t = struct sockaddr_storage;
  using in_addr_t          = struct in_addr;
  using in_addr6_t         = struct in_addr6;
  using hostent_t          = struct hostent;
  using timeval_t          = struct timeval;

  enum class EProtocol : int
  {
    NONE   = IPPROTO_NONE,
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

  enum class EAddressType : int // address familly
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
}

#endif //__net_hpp__
