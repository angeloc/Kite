include ../Makefile.in

CXXFLAGS += -g

BINARIES = \
	   dns \
	   httpd \
	   timer \
	   ssl \
	   stdio \
	   mqtt \
	   udpechoserver \
	   http \
	   process \
	   shell \
	   wget \
	   miniterm \
	   crawler \
	   publish_subscribe

.PHONY: all
all: $(BINARIES)

%.o: $(SOURCES)
	$(CXX) $(CXXFLAGS) -c $* -o $@ $(LDFLAGS)

$(BINARIES): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $@.cpp -o $@ $(LDFLAGS)

clean:
	rm -f  $(OBJECTS) *.o $(BINARIES)
