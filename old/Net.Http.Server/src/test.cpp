
#include <string>

namespace http
{
  class CConnection
  {
    CServer& mServer;
    CClient  mClient;
  };

  class CRequest
  {
    CConnection& mConnection;
    CServer&     mServer;
  };

  class CResponse
  {
    protected:
    CConnection& mConnection;
    CServer&     mServer;
    bool         mComplete;
    bool         mFinished;
    bool         mHeadersSent;
    
    std::string  mData;
  
    public:
    CResponse& set(const std::string& header, const std::string& value);
    
    CResponse& status(ushort status, const std::string& message = "");
    
    CResponse& append(const std::string& data);
    
    CResponse& send(const std::string& data); // first send headers
    
    CResponse& end(const std::string& data, std::function<void(void)> callback)
    {
      mData.append(data);
      end(callback);
    }
    CResponse& end(std::function<void(void)> callback)
    {
      end();
      callback();
    }
    CResponse& end()
    {
      mComplete = true;
      //CEventManager::trigger(CResponseEvent(*this));
    }
  };

  class CServer
  {
    protected:
    std::function<void(void)> mCallbacks;
    bool                      mListening;
    
    CConnection mConnection;
    CRequest    mRequest;
    CResponse   mResponse;
    
    public:
    CServer& on(const std::string& label, std::function<void(void)> callback)
    {
      mCallbacks.push_back(std::make_pair([&label](CEvent& e) {
        callback();
      }, label));
    }
    CServer& on(const std::string& label, std::function<void(CConnection&)> callback)
    {
      mCallbacks.push_back(std::make_pair([&callback, this](CEvent& e) {
        CConnectionEvent& ce = dynamic_cast<CErrorEvent&>(e);
        callback(ce.mConnection);
      }, label));
    }
    CServer& on(const std::string& label, std::function<void(CError&, CRequest&, CResponse&)> callback)
    {
      mCallbacks.push_back(std::make_pair([&callback, this](CEvent& e) {
        CErrorEvent& ee = dynamic_cast<CErrorEvent&>(e);
        callback(ee.mError, ee.mConnection.mRequest, ee.mConnection.mResponse);
      }, label));
    }
    CServer& on(const std::string& label, std::function<void(CRequest&, CResponse&)> callback)
    {
      mCallbacks.push_back(std::make_pair([&callback, this](CEvent& e) {
        CRequestEvent& re = dynamic_cast<CErrorEvent&>(e);
        callback(ee.mConnection.mRequest, ee.mConnection.mResponse);
      }, label));
    }
    
    CServer& timeout(uint timeout);
    
    CServer& listen(ushort port, std::function<void(void)> callback)
    {
      mCallbacks.push_back(std::make_pair([&callback](CEvent& e) {
        CConnectionEvent& ce = dynamic_cast<CConnectionEvent&>(e);
        callback(ce.mConnection);
      }, "connection"));
      
      while(true)
      {
        // poll for connections
          // pConnection = new CConnection(server, CClient(socket)); // w/ socket
          // mConnections.push_back(pConnection);
          // CEventManager::trigger(new CConnectionEvent(pConnection));
        
        // read client requests (recv)
          // data = recv();
          // pConnection = find_connection_by_socket(socket)
          // pConnection->mRequest = new CRequest(data);
          // CEventManager::trigger(CRequestEvent(*pConnection));
        
        // read exception fds
          // CEventManager::trigger(CErrorEvent(*pConnection, error))
        
        // send - respond with data (send)
          // for each connection
            // if the request is complete
              // start sending data // send this to another thread
              // CEventManager::trigger(CFinishEvent(*pConnection));
      }
    }
    
    protected:
    void onConnection(CConnectionEvent& event) // new connection(tcp)
    {
      for(auto it = mCallbacks.begin(), it != mCallbacks.end(); ++it)
        if((it->second == event.getLabel())
          (it->first)(event);
    }
    void onError(CErrorEvent& event) // error
    {
      for(auto it = mCallbacks.begin(), it != mCallbacks.end(); ++it)
        if((it->second == event.getLabel())
          (it->first)(event);
    }
    void onRequest(CRequestEvent& event) // request complete
    {
      mRequest&  = event.mConnection.mRequest;
      mResponse& = event.mConnection.mResponse;
      for(auto it = mCallbacks.begin(), it != mCallbacks.end(); ++it)
        if((it->second == event.getLabel())
          (it->first)();
    }
    void onClose(CCloseEvent& event) // server close
    {
      for(auto it = mCallbacks.begin(), it != mCallbacks.end(); ++it)
        if((it->second == event.getLabel())
          (it->first)(event);
    }
    void onConnect(CConnectEvent& event) // proxy to another server
    {
      for(auto it = mCallbacks.begin(), it != mCallbacks.end(); ++it)
        if((it->second == event.getLabel())
          (it->first)(event);
    }
  };
}

int main(int argc, char** argv)
{
  using namespace http;

  CServer srv;
  
  srv.on("connection", [](CConnection& conn) {
    
  });
  srv.on("request", [](CRequest& req, CResponse& res) {
    req.on("abort", []() {
      // request aborted by the client
    });
    req.abort(); // marks request for aborting
  
    res.on("close", []() { 
      // this connection was terminated before .end() event
    });
    res.on("finish", []() {
      // when the last chunk is given to the os
    });
    res.set("Content-Type", "text/plain");
    res.status(200, "message");
    res.send("some data...");
    res.end();
  });
  srv.on("error", [](CError& err, CRequest& req, CResponse& res) {
    // error...
  });
  srv.on("close", []() {
    // when server closes
  });
  srv.on("connect", []() {
    // on CONNECT header request for poxying to site
    // ex: CONNECT www.google.com:433
  });
  srv.on("upgrade", [](CRequest& req, CConnection& conn, CBuffer& head) {
    // switch protocol
  });
  srv.listen(8080, []() {
    // listening...
  });
}
