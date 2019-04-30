#pragma once

#include "TcpServer.h"
#include "base/noncopyable.h"

class HttpRequest;
class HttpResponse;

class HttpServer: noncopyable {
public:
    typedef std::function<void (const HttpRequest&,
                                HttpResponse*)> HttpCallback;

    HttpServer(EventLoop* loop,
              const int port,
              const string& name);

    EventLoop* getLoop() const {return server_.getLoop();}

    void setHttpCallback(const HttpCallback& cb) {
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads) {
        server_.setThreadNum(numThreads);
    }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, std::string& buf);
    void onRequest(const TpConnectionPtr&, const HttpRequest&);

    TcpServer server_;
    httpCallback httpCallback_;
};
