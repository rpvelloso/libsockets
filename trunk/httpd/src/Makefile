CXX = g++
CXXFLAGS = -Wall -I../include -O0 -g3 -fmessage-length=0 -fPIC -c
LDFLAGS = -lpthread -lsockets
LD = g++
TARGET = httpd
RM = rm
RMFLAGS = -rf
CP = cp
OBJS = main.o thttpclientsocket.o thttplog.o thttpserver.o \
thttpserversocket.o thttpthread.o
SRCS = main.cpp thttpclientsocket.cpp thttplog.cpp thttpserver.cpp \
thttpserversocket.cpp thttpthread.cpp

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