#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

HttpServer::HttpServer(EventLoop* loop,
        const int port, const std::string& name):
 :  server_(loop, port),
    httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(
            std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
            std::bind(&HttpServer::onMessage, this, _1, _2, _3));

}

void HttpServer::start() {
    std::cout << "HttpServer starts listening on 9981" << std::endl;
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    conn->setContext(HttpContext());
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, std::string& buf) 
{
    HttpContext* context = conn->getMutableContext;
    if (!context->parseRequest(buf)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    const std::string& connection = req.getHeader("connection");
    bool close = connection == "close" ||
        (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection()) {
        conn->shutdown();
    }
}
