class CSocket
{
  public:
  virtual void connect() = 0;
};

class CUdpSocket : public CSocket
{
  
};

class CTcpSocket : public CSocket
{
  
};

class CEventManager : public CSingleton<CEventManager>
{ 
  friend class CSingleton<CEventManager>;
  
  protected:
  std::vector<CEventListener*> mListeners;
  
  public:
  static bool attach(CEventListener* pListener)
  {
    CEventManager::getIstance()->mListeners.push_back(pListener);
  }
  
  static bool trigger(CEvent* pEvent)
  {
    // LIFO
    for(auto it = mListeners.rbegin(); it != mListeners.rend(); ++it)
    {
      if(*it->getType() == pEvent->getType())
      {
        (*it->trigger(pEvent);
      }
    }
  }
};

class CEventListener 
{ 
  virtual trigger(CEvent*) = 0;
};

class COnMessageEventListener : public CEventListener
{
  public:
  COnMessageEventListener() : CEventListener()
  {
    CEventManager::attach(this);
  }
  
  public:
  bool trigger(CEvent* pEvent)
  {
    CRecvEvent* pRecvEvent = dynamic_cast<CRecvEvent*>(pEvent);
    return onRecv(pRecvEvent);
  }
  
  protected:
  virtual bool onRecv(CRecvEvent*) = 0;
};


class CConnection
{
  public:
  enum EType
  {
    SERVER,
    CLIENT
  };
};

class CServer : public COnSendEventListener
{
  public:
  int listen(int timeout = 1000) throw CException
  {
    // TODO: replace by... with try catch
    // mSocket->connect();
    connect();
    
    timeval_t tv = { timeout / 1000, timeout % 1000 };
    
    int _status = ::select(mSocket+1, &_readfd, NULL, NULL, &tv);
    if(_status == -1)
    {
      // error
    }
    else if(_status == 0)
    {
      // message
      _bytes = ::recvfrom(mSocket, _input, MAXBUFLEN - 1, 0, (sockaddr_t*)(&_addr), &_addrlen);
      if(_bytes == -1)
      {
        // error
      }
      else
      {
        // input
        _input[_bytes] = '\n';
        
        CMessageEvent* pEvent = new CMessageEvent;
        pEvent->setMessage(new CMessage(_input));
        pEvent->getMessage()->setConnection(new CConnection(_sockaddr, UDP));
        CEventManager::trigger(pEvent);
        delete pEvent;
        
        return 0;
      }
    }
    else
    {
      // timeout
      
      CTimeoutEvent* pEvent = new CTimeoutEvent;
      CEventManager::trigger(pEvent);
      delete pEvent;
      
      return 1;
    }
    
    return -1;
  }
  
  void send(CMessage* pMessage)
  {
    CSendEvent* pEvent = new CSendEvent;
    pEvent->setMessage(pMessage);
    CEventManager::trigger(pEvent);
  }
  
  protected:
  bool onSend(CSendEvent* pEvent)
  {
    CMessage* pMessage = pEvent->getMessage();
    
    // TODO: replace by
    // mSocket->send(pMessage);
    
    ::sendto(mSocket,
             pMessage->getMessage().c_str(),
             pMessage->getMessage().size(), 
             pMessage->getConnection().getSockaddr(),
             pMessage->getConnection().getSockaddr().size());
  }
};

class CClient { };

class CNode { };

class CMain : public COnRecvEventListener // COnRecvServerListener
{
  public:
  void run()
  {
    // single server
    mServer = new CServer(LOCALHOST, 3213, UDP);
    mServer->connect();
    
    while(true)
    {
      mServer->listen(1000);
    }
    
    // for multiple servers
    while(true)
    {
      if(i % 3 == 0)
        mServer0->listen();
      else if(i % 3 == 1)
        mServer1->listen();
      else
        mServer3->listen();
    }
    
    // multi-threading - this requires mutex on onRecv
    std::thread t0([&mServer0] () {
      while(true)
        mServer0->listen();
    });
    std::thread t1([&mServer1] () {
      while(true)
        mServer1->listen();
    });
    
    t0.join();
    t1.join();
  }
  
  protected:
  bool onRecv(CRecvEvent* pEvent)
  {
    CMessage* pMessage = new CMessage;
    pMessage->setConnection(new CConnection(*(pEvent->getMessage()->getConnection())));
    pMessage->setMessage("some message here");
    mServer->send(pMessage);
    return true;
  }
};

