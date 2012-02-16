CXX = g++
CXXFLAGS = -Wall -I../include -O0 -g3 -fmessage-length=0 -fPIC -c
LDFLAGS = -lpthread -lsockets
LD = g++
TARGET = echod
RM = rm
RMFLAGS = -rf
CP = cp
OBJS = main.o techoclientsocket.o techolog.o techoserver.o \
techoserversocket.o techothread.o
SRCS = main.cpp techoclientsocket.cpp techolog.cpp techoserver.cpp \
techoserversocket.cpp techothread.cpp

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

all:	$(TARGET)
$(TARGET):	$(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)
dep:	$(SRCS)
	$(CXX) -MM $(CXXFLAGS) $(SRCS) > .dep
include .dep
clean:
	$(RM) $(RMFLAGS) $(OBJS) $(TARGET)
