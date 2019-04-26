#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>
#include <string>

using namespace std;

typedef TcpConnection::TcpConnectionPtr TcpConnectionPtr;
char buf[1024];

void onConnection(const TcpConnectionPtr& conn) {
        sockaddr_in addr = conn->getAddress();
    printf("New Connection from %s port %d\n",
          inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof buf),
           ntohs(addr.sin_port));
}

void onMessage(const TcpConnectionPtr &conn, string message) {
    printf("Received %d bytes from connection \n", static_cast<int>(message.size()));
}

int main() {
    printf("main(): pid = %d\n", getpid());
    EventLoop loop;
    TcpServer server(&loop, 9981, 1);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();

    return 0;
}
