CC=g++
CFLAGS=-Wall -O -g
TARGET=Main
LIBS=-lpthread
INC=-I ../
VPATH = ../

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $^ $(LIBS)

SOURCES=$(wildcard ../*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))


$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INC)  main.cpp $(OBJS) -o $(TARGET) -lpthread

clean:
	rm -rf ../*.o Main
