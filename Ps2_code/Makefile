CXX = g++
PROG = taskscheduler

CXXFLAGS += -g -O0 -std=c++11

CPPFLAGS += \
	-I. \
	-I/usr/include/jsoncpp

LDLIBS = \
		 -ljsoncpp

LDFLAGS += -L/usr/local/lib $(LDLIBS)

all: $(PROG)
	@echo $(PROG) Task Scheduler successfully compiled!

SRCS = \
	TaskScheduler.cpp
OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

$(PROG): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJS) $(PROG) ./.depend

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend