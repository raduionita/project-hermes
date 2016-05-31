#ifndef __http__
#define __http__

#include <unordered_map>

namespace http
{
  template <typename K, typename V>
  using hashmap = std::unordered_map<K, V>;

  enum EVerb
  {
    __VERB__ = 0x0000,
    GET      = 0x0001, 
    POST     = 0x0002, 
    PUT      = 0x0004, 
    #undef DELETE
    DELETE   = 0x0008, 
    OPTIONS  = 0x0010,
    HEAD     = 0x0020,
    TRACE    = 0x0040,
    CONNECT  = 0x0080,
    PATCH    = 0x0100,
    ALL      = GET | POST | PUT | DELETE | OPTIONS | HEAD | TRACE | CONNECT | PATCH
  };
  
  enum EState
  {
    __STATE__ = 0x00,
    IDLE      = 0x01,
    FLUSH     = 0x02,
    STARTED   = 0x03,
    DONE      = 0x04
  };
  
  enum EStatus
  {
    __STATUS__  = 0,
    OK          = 200,
    NOCONTENT   = 204,
    MOVED       = 301,
    NOTMODIFIED = 304,
    BADREQUEST  = 400,
    FORBIDDEN   = 403,
    NOTFOUND    = 404,
    SERVERERROR = 500,
    UNAVAILABLE = 503,
    LOOP        = 508
  };
  
  enum EType
  {
    __TYPE__ = 0x00,
    TEXT     = 0x01,  // text/plain
    HTML     = 0x02,  // text/html
    JSON     = 0x04,
    BYTES    = 0x08,
    STREAM   = BYTES,
    FORM     = 0x10,
    ICON     = 0x11,  // image/x-icon, image/x-icon
    PNG      = 0x12,  // image/png
    JPEG     = 0x13,  // image/jpeg, image/pjpeg
    GIF      = 0x14,  // image/gif
    JS       = 0x15,  // application/javascript
    CSS      = 0x16,  // text/css
    XML      = 0x17,  // application/xml, text/xml
    
    MP3      = 0x18,  // audio/mpeg3, audio/x-mpeg-3
    MP4      = 0x19,  // video/mp4, video/mpeg
    OGG      = 0x20,  // audio/ogg
    OGV      = 0x21,  // video/ogg
  };
  
  enum EHead // @todo Needs change to enum class
  {
    __HEAD__       = 0x00, 
    ACCEPT         = 0x01, // Accept: text/plain
    CHARSET        = 0x02, // Accept-Charset: utf-8
    ENCODING       = 0x03, // Accept-Encoding: gzip OR deflate
    LANGUAGE       = 0x04, // Accept-Language: en-US,en;q=0.8
    CACHE          = 0x05, // Cache-Control: no-cache
    CONNECTION     = 0x06, // Connection: keep-alive OR Connection: upgrade
    COOKIE         = 0x07, // Cookie: $Version=1; Skin=new; OR Set-Cookie: UserID=JohnDoe; Max-Age=3600; Version=1
    LENGTH         = 0x08, // Content-Length: 234
    TYPE           = 0x09, // Content-Type: application/x-www-form-urlencoded
    DATE           = 0x10, // Date: Tue, 15 Nov 1994 02:32:43 GMT
    EXPECT         = 0x11, // Expect: 100-contunue
    FORWARDED      = 0x12, // Forwarded: for=192.0.2.55;proto=http;by=203.54.12.42
    FROM           = 0x13, // From: user@site.com
    HOST           = 0x14, // Host: en.wikipedia.org:80
    ORIGIN         = 0x15, // Origin: http://www.example.com
    PRAGMA         = 0x16, // Pragm: no-cache
    PROXY          = 0x17, // Proxy: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
    RANGE          = 0x18, // Range: bytes=500-999
    REFERER        = 0x19, // Referer: http://en.wikipedia.org/wiki/Main_Page
    USERAGENT      = 0x20, // User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0
    UPGRADE        = 0x21, // Upgrade: HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11
    
    ETAG           = 0x22, // ETag: 737060cd8c284d8af7ad3082f209582d
    AGE            = 0x23, // Age: 12
    EXPIRES        = 0x24, // Expires: Thu, 01 Dec 1994 16:00:00 GMT
    MODIFIED       = 0x25, // Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
    SERVER         = 0x26, // Server: Apache/2.4.1 (Unix)
    STATUS         = 0x27, // Status: 200 OK
    
    CSRF           = 0x60  // X-Csrf-Token: i8XNjC4b8KVok4uw5RftR38Wgp2BFwql
  };

  inline constexpr bool operator == (int i, EVerb a) { return i == static_cast<int>(a); }
  inline constexpr bool operator == (int i, EState a) { return i == static_cast<int>(a); }
  inline constexpr bool operator == (int i, EStatus a) { return i == static_cast<int>(a); }
  inline constexpr bool operator == (int i, EType a) { return i == static_cast<int>(a); }
  inline constexpr bool operator == (int i, EHead a) { return i == static_cast<int>(a); }
  
  inline std::string getType(EType type)
  {
    switch(type)
    {
      default:
      case EType::TEXT:  return std::string("text/plain;charset=UTF-8");
      case EType::HTML:  return std::string("text/html;charset=UTF-8");
      case EType::JSON:  return std::string("application/json;charset=UTF-8");
      case EType::BYTES: return std::string("application/octet-stream");       // EType::STREAM
      case EType::FORM:  return std::string("multipart/form-data");
      case EType::ICON:  return std::string("image/x-icon");
      case EType::PNG:   return std::string("image/png");
      case EType::JPEG:  return std::string("image/jpeg");
      case EType::GIF:   return std::string("image/gif");
      case EType::JS:    return std::string("application/javascript");
      case EType::CSS:   return std::string("text/css");
      case EType::XML:   return std::string("text/xml");
      case EType::MP3:   return std::string("audio/mpeg3");
      case EType::MP4:   return std::string("video/mp4");
      case EType::OGG:   return std::string("audio/ogg");
      case EType::OGV:   return std::string("video/ogg");
    }
  }
  
  inline std::string getVerb(EVerb verb)
  {
    switch(verb)
    {
      case EVerb::GET    : return "GET";
      case EVerb::POST   : return "POST";
      case EVerb::PUT    : return "PUT";
      case EVerb::DELETE : return "DELETE";
      case EVerb::OPTIONS: return "OPTIONS";
      case EVerb::HEAD   : return "HEAD";
      case EVerb::TRACE  : return "TRACE";
      case EVerb::CONNECT: return "CONNECT";
      case EVerb::PATCH  : return "PATCH";
      default:
      case EVerb::ALL    : return "UNKNOWN";
    }
  }
}

#endif // __http__
