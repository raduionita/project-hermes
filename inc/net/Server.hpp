#ifndef __net_server_hpp__
#define __net_server_hpp__

namespace net
{
  class Server
  { 
    
  };
  
  class TcpServer : public Server
  {
    Socket                     mServer;
    std::map<socket_t, Stream> mClients;
  
    public:
    TcpServer& on(const label_t& label, std::function<void(Message&)> callback)
    {
      mActions[label] = [](Event& event) {
        MessageEvent& messageevent = dynamic_cast<MessageEvent&>(event);
        callback(messageevent.mMesssage);
      };
    }
    
    TcpServer& listen()
    {
      
    }
  };
}

#endif // __net_server_hpp__
