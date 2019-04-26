VPATH = ../../:../../base/
SOURCES = EventLoop.o Socket.o Channel.o Epoll.o Thread.o CountDownLatch.o TcpServer.o TcpConnection.o EventLoopThread.o EventLoopThreadPool.o Timestamp.o
CC = g++
CXXFLAGS = -std=c++11 -O0 -g -Wall
INCLUDE = -I../../
%.o : %.cpp
	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ -c $< -lpthread

test : test1.o $(SOURCES)
	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ test1.cpp $(SOURCES) -lpthread

.PHONY : clean
clean:
	rm -rf test ./*.o
