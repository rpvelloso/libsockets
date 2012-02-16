CXX = g++
CXXFLAGS = -Wall -I../include -O0 -g3 -fmessage-length=0 -fPIC -DDEVEL_ENV -c
LDFLAGS = -shared -lpthread
LD = g++
TARGET = libsockets.so
RM = rm
RMFLAGS = -rf
CP = cp
MKDIR = mkdir
INSTALL_PATH_HDR = /usr/local/include/libsockets
INSTALL_PATH_LIB = /usr/local/lib
OBJS = tclientsocket.o tlogger.o tmutex.o \
tobject.o tserversocket.o tsocket.o tthread.o utils.o \
xmlParser.o
SRCS = tclientsocket.cpp tlogger.cpp tmutex.cpp tobject.cpp \
tserversocket.cpp tsocket.cpp tthread.cpp utils.cpp xmlParser.cpp

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
install:
	if [ ! -d "$(INSTALL_PATH_HDR)" ]; then $(MKDIR) $(INSTALL_PATH_HDR); fi
	$(CP) ../include/*.h $(INSTALL_PATH_HDR)
	$(CP) $(TARGET) $(INSTALL_PATH_LIB)
