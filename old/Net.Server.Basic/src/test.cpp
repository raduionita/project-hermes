#include <vector>
#include <utility>
#include <map>

class CServer 
{ 
  protected:
  EProtocol mProtocol;
  CSocket   mSocket;
  
  public:
  virtual void listen() = 0;
};

class CMessage
{
  public:
  std::string mMessage;
};

class CRequest : public CIncomingMessage
{
  
}

class CResponse : public COutgoingMessage
{

}

class Connection
{
  public:
  CSocket    mSocket;
  CRequest*  mRequest;
  CResponse* mResponse;
};

class CEvent
{
  public:
  label_t        mLabel;
  CEvent&        mOriginalEvent;
  CEventManager* mTarget;
};

class CEventManager
{
  protected:
  std::map<std::string, std::function<void(CEvent&)>> mCallbacks;
  
  public:
  virtual void trigger(CEvent& e)
  {
    for(auto it = mCallbacks.begin(); it != mCallbacks.end(); ++it)
    {
      e.mTarget = *this;
      if(it->first == e.getLabel())
        it->second(e);
    }
  }
};

class CUdpServer : public CServer, CEventManager
{
  public:
  CUdpServer() : CServer() { }
  
  public:
  void listen()
  {
    // create socket
      
    while(true) 
    {
      // select
    }
  }
};

class CTcpServer : public CServer, CEventManager
{
  protected:
  std::map<socket_t, Connection*>                    mConnections;
  
  public:
  CTcpServer() : CServer(), CEventManager() { }
  
  virtual void on(label_t label, std::function<void(CMessage&)> callback)
  {
    mCallbacks[label] = [](CEvent& e) {
      CMessageEvent& me = dynamic_cast<CMessageEvent&>(e);
      callback(me.mMessage);
      
      // if has COMMAND header/message and is SPECIAL stream(admin or stdin)
      this->trigger(CCommandEvent(comm));
    };
  }
  
  void listen()
  {
    // create socket
      // set to listen
      
    while(true) 
    {
      // before
    
      // add sockets to readfds
      // add sockets to writefds
      // add sockets to exceptfds
      // select 
      // for all reads
        // if new connection
          socket_t sfd = accept();
          Connection* conn = new CConnection(sfd);
          mConnections.insert(std::make_pair(sfd, conn);
          this->trigger(CConnectionEvent(conn.mSocket));
          
        // if close connection
          Connection conn = mConnections.pop(sfd);
          // if mStream.mResponse.mFinished != true 
          conn.mResponse->trigger(CCloseEvent()); // response close
        
        // if read data available
          data = recv();
          CMessage msg(data);
          this->trigger(CMessageEvent(msg));
          
          
      // for all exceptions
        conn.mResponse->trigger(CAbortEvent()); // abort by the client
      
      // for all writes
        // if socket ready for write
          // for all responses
            // if response complete
              send(res);
              conn.mResponse->trigger(CFinishEvent()); // response finished
              // delete request, response
              // if connection alive
          
          
      // after
    }
    
    this->trigger(CCloseEvent()); // server close
  }
};

class CHttpServer : public CTcpServer
{
  public:
  CHttpServer() : CTcpServer() { }
  
  public:
  virtual void on(label_t label, std::function<void(CRequest&, CResponse&)> callback)
  {
    mCallbacks[lable] = [this](CEvent& e) {
      CRequestEvent& re = dynamic_cast<CRequestEvent&>(e);
      CRequest&  req = re.mRequest;
      CResponse& res = req.mStream.mResonse;
      callback(req, res);
      
      if(req.verb("UPGRADE")) // if has UPGRADE header
        this->trigger(CUpgradeEvent(req, conn, head));
    };
  }
  
  virtual void on(label_t label, std::function<void(CMessage&)> callback)
  {
    mCallbacks[label] = [this](CEvent& e) {
      CMessageEvent& me = dynamic_cast<CMessageEvent&>(e);
      callback(me.mMessage);
      
      // handle request event
      CRequest req(e.mMessage);
      this->trigger(CRequestEvent(e, req));
      
      // if has COMMAND header/message and is SPECIAL stream(admin or stdin)
      if(req.verb("COMMAND"))
        this->trigger(CCommandEvent(e, comm));
    };
  }
};





int main(int argc, char** argv) 
{
  CUdpServer  srv;
  CTcpServer  srv;
  CHttpServer srv;
  
  srv.listen([] {
    // listening...
  });
  
  return 0;
}
