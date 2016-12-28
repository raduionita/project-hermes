#ifndef __http_cserver_hpp__
#define __http_cserver_hpp__

#include <core/CEvent.hpp>
#include <core/CEventEmiter.hpp>

#include <log/CLogger.hpp>

#include <net/CServer.hpp>
#include <net/CSocket.hpp>
#include <net/CMessage.hpp>
#include <net/CError.hpp>

#include <watch.hpp>

#include <http.hpp>
#include <http/CRequest.hpp>
#include <http/CResponse.hpp>
#include <http/CRequestEvent.hpp>

#include <functional>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>

// @todo replace ::WSAGetLastError() with a define or something...

namespace http
{
  using namespace core;

  class CServer : public net::AServer, public CEventEmiter
  {
    typedef std::shared_ptr<CRequest>        request_t;
    typedef std::shared_ptr<CResponse>       response_t;
    typedef std::pair<request_t, response_t> client_t;
    typedef net::CServer                     self;
    typedef net::AServer                     parent;

    protected:
    net::CSocket*                mServer;  // ptr::shared<CSocket> mServer;
    hashmap<socket_t, client_t>  mClients; // @todo Need shared pointers

    public:
    port_t         mPort;      // 80, 8080
    host_t         mHost;      // NULL, localhost, 127.0.0.1 or sub.domain.tld
    watch::seconds mTime;      // seconds
    bool           mRunning;   // false stops the loop
    size_t         mBuffering; // 4096

    public:
    CServer()
    : net::AServer(), mServer(nullptr), mPort(8080), mTime(10), mRunning(false), mBuffering(4096)
    {
      log::debug << "http::CServer::CServer()" << log::endl;
    }

    CServer(port_t port, host_t host)
    : net::AServer(), mServer(nullptr), mPort(port), mHost(host), mTime(10), mRunning(false), mBuffering(4096)
    {
      log::debug << "http::CServer::CServer(port, host)" << log::endl;
    }

    ~CServer()
    {
      log::debug << "http::CServer::~CServer()" << log::endl;

      while(mClients.size() > 0) // safely close client connections
      {
        log::info << "> Clients pending: " << mClients.size() << log::endl;

        auto then = watch::millitime();
        then -= mTime;
        bool wait = false;

        for(auto it = mClients.begin(); it != mClients.end();/* ++it */)
        {
          if(it->second.first == nullptr || it->second.second == nullptr)
          {
            ::closesocket(it->first);
            mClients.erase(it++);
          }
          else
          {
            if(it->second.second->is(EState::FLUSH) || it->second.second->is(EState::DONE)) // ready for erase: FLUSH or DONE
            {
              log::info << "> Removing client... " << log::endl;

              // /* CResponse* */     delete it->second.second; it->second.second = nullptr;
              // /* CRequest* */      delete it->second.first;  it->second.first  = nullptr;
              ::closesocket(it->first);

              mClients.erase(it++);
            }
            else
            {
              log::info << "> Time: " << it->second.second->time()  << " < " << then << log::endl;

              if(it->second.second->time() < then) // out of time
              {
                log::info << "> Ending client..." << log::endl;

                it->second.second->set(EState::DONE);

                // @todo Try to send the response before closing
              }
              else                                           // wait another second
              {
                log::info << "> Not ready yet, I should wait..." << log::endl;
                wait = true;
              }
              ++it;
            }
          }
        }

        if(wait) // does it still need to wait?
        {
          log::info << "> Sleeping for 1 sec... " << log::endl;
          std::this_thread::sleep_for(watch::seconds(1));
        }
      }

      mServer->close();
      delete mServer; mServer = nullptr;
    }

    public:
    void bind()
    {
      log::debug << "http::CServer::bind()" << log::endl;

      try // listening/server socket
      {
        mServer = new net::CSocket(net::CSocket::SERVER, mPort, mHost, EProtocol::TCP, EAddressType::UNSPEC);
      }
      catch (core::CException& e)
      {
        close();
        throw e;
      }
    }

    void close()
    {
      log::debug << "http::CServer::close()" << log::endl;
      mRunning = false;
    }

    CServer& listen(port_t port, std::function<void(void)> callback = noop)
    {
      mPort = port;
      return listen(callback);
    }

    CServer& listen(port_t port, const host_t& host, std::function<void(void)> callback = core::noop)
    {
      mPort = port;
      mHost = host;
      return listen(callback);
    }

    CServer& listen(std::function<void(void)> callback = core::noop)
    {
      log::debug << "http::CServer::listen(callback)" << log::endl;

      bind();     // server socket created

      callback(); // ready for loop

      result_t  result = STATUS_OK;
      timeval_t timeout = { 0, 000000 }; // no timeout

      fd_set ifdset; FD_ZERO(&ifdset); // incoming fd sockets
      fd_set rfdset; FD_ZERO(&rfdset); // reading  fd sockets
      fd_set wfdset; FD_ZERO(&wfdset); // writing  fd sockets
      fd_set efdset; FD_ZERO(&efdset); // error    fd sockets

      socket_t srvfd = (socket_t)(*mServer);
      socket_t minfd = srvfd;
      socket_t maxfd = minfd;

      CMessage            msg;
      watch::milliseconds now;

      mRunning = true;
      while(mRunning)
      {
        now    = watch::millitime();

        rfdset = ifdset;
        wfdset = ifdset;
        efdset = ifdset;
        FD_SET(srvfd, &rfdset); // add server to read
      //FD_SET(STDIN_FILENO, &rfdset); // not in win32
        FD_SET(srvfd, &efdset); // add server to except
      //FD_SET(STDIN_FILENO, &efdset); // not in win32

        // @todo Normalize maxfd
        // @todo Check for unclosed clients

        result = ::select(maxfd + 1, &rfdset, &wfdset, &efdset, &timeout);
        if(result < RESULT_OK)       // error   // close server
        {
          log::error << "> Error select() " << log::endl;
          //CEventEmiter::emit(new CErrorEvent(error));
          close();
        }
        else if(result == RESULT_OK) // timeout // spinner
        {
          log::info << log::spinner; // log::warn << "> Timeout! " << log::endl;
        }
        else                         // data    // result > RESULT_OK, result = num_sockets
        {
          for(socket_t curfd = minfd; curfd < (maxfd + 1); ++curfd) // try to run through all fds
          {
            if(FD_ISSET(curfd, &efdset)) // has exception
            {
              // @todo Close client/response
              log::error << "> Exception on socket " << curfd << log::endl;
              // @todo Remove from maxfd if you can
              FD_CLR(curfd, &ifdset);
              continue; // continue for
            }

            if(FD_ISSET(curfd, &wfdset)) // ready for write
            {
              result = 0;
              auto it = mClients.find(curfd);
              if(it != mClients.end() && it->second.second != nullptr) // find client, that as a response object
              {
                request_t&   req = it->second.first;
                response_t&  res = it->second.second;
                socket_t   curfd = res->socket();
                bool         oot = res->time() < now - mTime; // out of time

                // @todo Add send/flush when buffer is full

                if(oot || res->is(EState::FLUSH)) // if ready for sending // send & check error
                {
                  std::string msg = res->read();
                  result = ::send(curfd, msg.c_str(), msg.size(), 0); // tcp::send() -> send all
                  // check if the message was sent
                  if(result < RESULT_OK) // if error sending data // log error
                  {
                    log::error << "> Error sending data for socket " << curfd << "." << log::endl;
                    log::error << ::gai_strerror(::WSAGetLastError()) << log::endl;
                    // close connection
                    mClients.erase(curfd);
                    FD_CLR(curfd, &ifdset);
                    ::closesocket(curfd);
                  }
                  else                   // if ok // log info
                  {
                    log::info << "> Response sent on socket " << curfd << "." << log::endl;

                    res->set(EState::DONE);

                    // @todo What if another thread(event action) needs this
                    //       Maybe this should be marked for delete, but not just yet...only after everyone is done

                    // close only if requested
                    // @todo Maybe I should use strcmp() == 0
                    if(req->http() == EHttp::HTTP09)      // HTTP 0.9 always close
                    {
                      mClients.erase(curfd); // remove old req & res
                      FD_CLR(curfd, &ifdset);
                      ::closesocket(curfd);
                      log::info << "> Clossing connection on socket " << curfd << log::endl;
                    }
                    else if(req->http() == EHttp::HTTP10) // HTTP 1.0 close UNLESS:  Connection: keep-alive
                    {
                      mClients.erase(curfd); // remove old req & res
                      if(req->head(EHead::CONNECTION) == "keep-alive")
                      {
                        mClients[curfd]; // touch/keep alive
                      }
                      else
                      {
                        FD_CLR(curfd, &ifdset);
                        ::closesocket(curfd);
                        log::info << "> Clossing connection on socket " << curfd << log::endl;
                        // @todo emit -> new CCloseEvent()
                      }
                    }
                    else                                // HTTP 1.1 clone ONLY ON: Connection: close
                    {
                      mClients.erase(curfd); // remove old req & res
                      if(req->head(EHead::CONNECTION) == "close")
                      {
                        FD_CLR(curfd, &ifdset);
                        ::closesocket(curfd);
                        log::info << "> Clossing connection on socket " << curfd << log::endl;
                        // @todo emit -> new CCloseEvent()
                      }
                      else
                      {
                        mClients[curfd]; // touch
                      }
                    }
                  }

                  continue; // continue for
                }
              }

              log::info << log::spinner;
            }

            if(FD_ISSET(curfd, &rfdset)) // ready for read
            {
              if(curfd == srvfd) // new connection
              {
                // @todo Move this to a method and surrounde it with a try catch

                socket_t newfd = ::accept(srvfd, NULL, NULL);
                if(newfd == INVALID_SOCKET) // if connection error
                {
                  log::error << "Connection error!" << log::endl;
                  // @todo emit -> new CErrorEvent(error)
                }
                else                        // if new connection // add new client
                {
                  log::info << "> New connection on socket " << newfd << log::endl;

                  minfd = std::min(newfd, minfd);
                  maxfd = std::max(newfd, maxfd);

                  FD_SET(newfd, &ifdset);
                  mClients[newfd]; // touch
                  // mClients.insert(std::make_pair(newfd, std::make_pair(nullptr, nullptr)));

                  // @todo Trigger a new connection event
                  // CEventEmiter::emit(new CConnectionEvent(con));
                }
              }
              else               // incoming data
              {
                log::info << "> Incoming message from socket " << curfd << "." << log::endl;

                // @todo This whole section could/should be spawned on another thread - fist lock the fd

                msg.clear();

                char chunk[4096 + 1];            // +1 for \0 // chunk[16+1]
                int  length = sizeof(chunk) - 1; //           // length = 16
                while(true)
                {
                  result = ::recv(curfd, chunk, length, 0);
                  if(result < 0)       // error
                  {
                    net::CError& err = net::getError();
                    if(err != net::CError::WOULDBLOCK && err != net::CError::AGAIN) // generic errors // close socket
                    {
                      log::error << "> Error receiving data for socket " << curfd << "." << log::endl;
                      log::error << err << log::endl;
                      log::warn << "> Closing connection on socket " << curfd << "." << log::endl;
                      // close socket, remove client
                      FD_CLR(curfd, &ifdset);
                      ::closesocket(curfd);
                      mClients.erase(curfd);

                      // @todo throw exception!?
                      // @todo emit -> new CErrorEvent(error)
                      // @todo emit -> new CCloseEvent()
                    }
                    else // WOULDBLOCK || AGAIN // continue, don't stop
                    {
                      log::warn << "< " << err << log::endl;
                      log::warn << "< chunk: " << chunk << log::endl;
                    }

                    break; // break while
                  }
                  else if(result == 0) // close connection
                  {
                    log::warn << "> Closing connection on socket " << curfd << "." << log::endl;
                    // close socket, remove client
                    FD_CLR(curfd, &ifdset);
                    ::closesocket(curfd);
                    mClients.erase(curfd); // remove client

                    // @todo emit -> new CCloseEvent()

                    break; // break while
                  }
                  else                 // chunk or complete
                  {
                    chunk[result] = '\0';  // ending of the current chunk
                    msg.append(chunk);     // don't worry out \0, string takes care of it on append
                    if(result < length || result > length)  // complete // output message
                    {
                      log::info << log::nl << log::nl << msg << log::nl << log::endl;
                      // create <request, response> pair for this client
                      mClients[curfd] = std::make_pair(std::make_shared<CRequest>(curfd, msg), std::make_shared<CResponse>(curfd));
                      // trigger request event
                      CEventEmiter::emit(new CRequestEvent(mClients[curfd].first));
                      break; // break while
                    }
                  }
                }
              }
            }
          }
        }
      }

      return *this;
    }

    CServer& on(const label_t& label, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      log::debug << "http::CServer::on('" << label << "', callback)" << log::endl;
      // add callback to event emiter/manager
      CEventEmiter::add(label, [this, callback](const CEvent* e) {
        const CRequestEvent* re  = dynamic_cast<const CRequestEvent*>(e);
        const request_t&     req = re->mRequest;
        if(req)
        {
          const response_t&  res = mClients[req->socket()].second;
          if(res && !res->is(EState::DONE) && !res->is(EState::FLUSH))
          {
            synchronized(*res) // prevent threads with same response from overwriting each other
            {
              callback(*req, *res);
            }
          }
        }
      });

      return *this;
    }
  };
}

#endif // __http_cserver_hpp__
