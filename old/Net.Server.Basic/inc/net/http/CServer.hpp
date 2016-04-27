#ifndef __net_http_cserver_hpp__
#define __net_http_cserver_hpp__

#include <net.hpp>
#include <net/CServer.hpp>
#include <net/http/CEvent.hpp>
#include <net/http/CRequestEvent.hpp>
#include <net/http/CRequest.hpp>
#include <net/http/CResponse.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <utility>
#include <string>

namespace net
{
  namespace http
  {
    class CServer : public net::CServer
    {
      using super_t     = net::CServer;
      
      public:
      CServer(port_t port = 80, const host_t& host = "localhost") 
      : net::CServer(port, host, EProtocol::TCP)
      {
        std::cout << "net::http::CServer::CServer(port, host)" << std::endl;
      }
      
      virtual ~CServer()
      {
        // close socket
      }
      
      protected:
      void bind() throw (core::CException)
      {
        super_t::bind();
      }
      
      void onMessage(CMessage& msg)
      {
        // @todo
        // http: build CRequest, trigger onRequest()
      }
      
      public:
      CServer& on(const label_t& label, std::function<void(CRequest&, CResponse&)> callback)
      {
        mCallbacks.push_back(std::make_pair([&callback](CEvent& e) {
          CRequestEvent& re = dynamic_cast<CRequestEvent&>(e);
          callback(re.mRequest, re.mResponse);
        }, label));
        return *this;
      }
      
      void listen(port_t port = 80, std::function<void(void)> callback = core::noop) throw(core::CException)
      {
        std::cout << "net::http::CServer::listen(port, callback)" << std::endl;
        
        mPort = port;
        
        bind();
        
        callback();
        
        while(true)
        {
          //::recv();
        
          // ::shutdown(socket, SD_SEND); // close connection when done
        
          // ::send();
          break;
        }
      }
    };
  }
}

#endif // __net_http_cserver_hpp__
