#define _DEBUG_
#define WIN32_LEAN_AND_MEAN

#include <net/CServer.hpp>
#include <net/CMessage.hpp>
#include <net/CRequest.hpp>

#include <iostream>
#include <functional>

void testtcp()
{
  net::CTcpServer srv;
  
  srv.on("connection", [&srv](net::CConnection& con) {
    con.on("message", [&con, &srv](net::CMessage& msg) {
      // @todo: keep-alive tcp machanism
      for(auto it = srv.mConnections.begin(); it != srv.mConnections.end(); ++it) 
      {
        net::CConnection*& con = it->second;
        con->write(msg);
        con->flush();
        con->end("\n"); // send data to client(w/ flush), client is disconnected 
      }
    });
    con.on("close", []() {
      std::cout << "> closed" << std::endl;
    });
  });
  srv.on("message", [](net::CMessage& msg) {
    std::cout << "Srv:Message..." << std::endl;
  });
  srv.on("error", [](net::CError& err) {
    std::cout << err << std::endl;
  });
  srv.listen([](void) {
    std::cout << "> Listening..." << std::endl;
  });
}

void testhttp()
{
  net::CHttpServer srv;
  srv.on("request", [&srv](net::CRequest& req, net::CResponse& res) {
    res.write("something");
    // res.status(200, "OK");
    res.end();
  });
  srv.listen(80, []() {
    std::cout << "> Listening..." << std::endl;
  });
}

int main()
{
  testtcp();

  return 0;
}

//net::CTransport;
//net::CTcpTransport;
//net::CUdpTransport;
//
//net::CServer
//net::CServer<CTcpTransport> /* vs */ net::CTcpServer
  //CServer* srv = new CServer(EProtocol::TCP, EAddress::IPV4);
//net::CServer<CUdpTransport> /* vs */ net::CUdpServer
//
//http::CServer srv
  //srv.on("request", []() { });
//rtsp::CServer
//smtp::CServer
//ldap::CServer
//dhcp::CServer
//imap::CServer


// Buffer

// Socket

// Agent

// Server : Agent
  // Server::onOutput()
    // 
  // Server::onInput()
    // -> Server::onMessage()
    // OR
    // -> Server::onDisconnect() // if empty input

// UdpServer : Client

// TcpServer : Server

// HttpServer : TcpServer

// Client : Agent
  // Client::onInput() from Server::onOutput

// UdpClient : Client

// TcpClient : Client

// Message

// TcpMessage : Message
  // Socket mSrc
  // Socket mDst

// UdpMessage : Message

// Stream 
  // Stream::flush()
    // EventManager::trigger(flush)
  // Stream::write()
  // 
