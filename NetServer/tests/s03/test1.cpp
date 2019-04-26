#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>

using namespace std;

typedef TcpConnection::TcpConnectionPtr TcpConnectionPtr;
char buf[2014];

void onConnection(const TcpConnectionPtr& conn) {
    sockaddr_in add = conn->getAddress();
    printf("on Connection: new connection from [%s:%d]\n",
           inet_ntop(AF_INET, &add.sin_addr, buf, 1024),
           ntohs(add.sin_port)
          );
}

void onMessage(const TcpConnectionPtr& conn, string& inputBuffer) {
    //sockaddr_in add = conn->getAddress();
    printf("on Messgae(): received %d bytes from connection\n",
          (int)inputBuffer.size());

    conn->send(inputBuffer);
    inputBuffer.clear();

}


int main(int argc, char* argv[]) {
    printf("main(): pid = %d\n", getpid());

    EventLoop loop;
    TcpServer server(&loop, 9981);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();

    return 0;
}
