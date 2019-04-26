VPATH=../../:../../base/ 
SOURCES = EventLoop.o Channel.o Epoll.o Thread.o CountDownLatch.o
CC = g++
CXXFLAGS = -std=c++11 -g -Wall -O
INCLUDE = -I../../

%.o : %.cpp
	$(CC) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ -lpthread

#test1 : test1.o $(SOURCES)
#	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ test1.cpp $(SOURCES) -lpthread
#test2 : test2.o $(SOURCES)
#	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ test2.cpp $(SOURCES) -lpthread
test3 : test3.o $(SOURCES)
	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ test3.cpp $(SOURCES) -lpthread

.PHONY : clean
clean:
	rm -rf ./*.o
