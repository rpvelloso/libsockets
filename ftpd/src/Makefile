CXX = g++
CXXFLAGS = -Wall -I../include -O0 -g3 -fmessage-length=0 -fPIC -c
LDFLAGS = -lpthread -lsockets
LD = g++
TARGET = ftpd
RM = rm
RMFLAGS = -rf
CP = cp
OBJS = main.o tftpclientsocket.o tftpdatasocket.o tftplog.o \
tftppassivesocket.o tftpserver.o tftpserversocket.o tftpthread.o
SRCS = main.cpp tftpclientsocket.cpp tftpdatasocket.cpp \
tftplog.cpp tftppassivesocket.cpp tftpserver.cpp tftpserversocket.cpp \
tftpthread.cpp

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
