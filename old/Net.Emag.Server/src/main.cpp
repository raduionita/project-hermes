#include <iostream>
#include <string>
#include <vector>

#include "net/net.server.hpp"

namespace app
{
  class CMain /* public net::COnRecvEventListener, net::COnSendEventListener */
  {
    protected:
    net::CServer* mServer;
    
    public:
    void run()
    {
      std::cout << "CMain::run()" << std::endl;
      
//      mClient = new net::CClient("127.0.0.1", "3242", UDP);
//      while(1)
//      {
//        mClient->
//      }
      
      mServer = new net::CServer(net::CAddress::LOCALHOST, "3242", net::EProtocol::UDP);
      
      // mServer->onAck(); UDP
      // mServer->onPing(); UDP
      // mServer->onMessage(CMain::onMessage);
      // mServer->onInit() when constructor is run
      // mServer->onBind() after successful bind
      // mServer->onConnect() for TCP on new client connect to server
      // mServer->onDisconnect(); TCP
      
      // mServer->onLoop() or onEmpty() or onTimeout()
      
      // mServer->onSend(CMain::send);
      // mServer->on(net::EEventType::SEND, CMain::send);
      
      try {
        mServer->run();
      } catch(net::CException& e) {
        std::cerr << e.what() << std::endl;
      }
      
      delete mServer;
    }
    
    bool onMessage(net::CEvent* pEvent)
    {
      // mServer->send(pMessage);
      return true;
    }
  };
}

int main(int argc, char** argv)
{
  app::CMain main;
  main.run();
  return 0;
}
