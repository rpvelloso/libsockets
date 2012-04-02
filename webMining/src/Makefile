CXX = g++
CXXFLAGS = -Wall -I../include -O0 -g3 -fmessage-length=0 -fPIC -c
LDFLAGS = 
LD = g++
TARGET = htmltool
RM = rm
RMFLAGS = -rf
CP = cp
OBJS = main.o misc.o tnode.o tdom.o
SRCS = main.cpp misc.cpp tnode.cpp tdom.cpp

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
