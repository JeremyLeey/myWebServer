CC := g++
TARGET := Main
INCLUDE := -I../
LIBS := -lpthread
VPATH := ../ 

CCXXFLAGS := -std=c++11 -g -Wall -D_REENTRANT
# OBJECTS := ThreadPool.o ThreadPoll_test.o

%.o : %.cpp
	$(CC) $(CCXXFLAGS) $(INCLUDE) -o $@ -c $< $(LIBS)

SOURCES=$(wildcard ../*.cpp)
OBJS=$(patsubst %.cpp,%.o, $(SOURCES))

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) test.cpp $(OBJS) -o $(TARGET) $(LIBS)

.PHONY : clean
clean:
	rm -f $(OBJECTS) $(TARGET)
