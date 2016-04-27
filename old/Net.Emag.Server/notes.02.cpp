class CMessage
{
  public:
  CConnection mConnection;
  CData       mData;
  TIMESTAMP   mTimestamp;
  
  public:
  CMessage()
  {
    
  }
};

class CConnection
{
  protected:
  ADDRESS  mAddress;
  PORT     mPort;
  PROTOCOL mProtocol;
};

class CSocket
{
  protected:
  CConnection mConnection;
  
  public:
  CSocket(ADDRESS, PORT, PROTOCOL);
  
  public:
  virtual CMessage* recv() = 0;
  virtual bool send(CMessage*) = 0;
};

class CServerSocket : public CSocket
{
  
};

class CApp
{
  void run()
  {
    mServer = new CServerSocket(LOCALHOST, 3242, UDP);
    pServer->connect();
    
    while(true)
    {
      // input
      CMessage* pMessage = mServer->recv();
      
      // update
      
      
      // output
      mServer->send(new CMessage(*pMessage));
    }
    
    delete mServer;
    
    return 0;
  }
};

// UDP

server 
  send message 0
client
  read input
  process message 0
  send ACK 0
server
  wait for ACK 0
  if ACK
    send message 1
  else
    send message 0
client
  read input
  if input in messages
    send ACK 0
  else
    process message 1



















