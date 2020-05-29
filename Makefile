CXX=gcc
CFLAGS= -c
CXXFLAGS= -std=gnu99 -Wall -Wextra
OUTPUT= webserver
all: webserver

webserver: webserver.o request_handling.o
	$(CXX) $(CXXFLAGS) webserver.o request_handling.o -o $(OUTPUT)
webserver.o: webserver.c
	$(CXX) $(CFLAGS) webserver.c http_headers.h
sliding_window.o: request_handling.c
	$(CXX) $(CFLAGS) request_handling.c request_handling.h 

clean:
	rm -rf *.o  *.gch

distclean:
	rm -rf *.o *.gch $(OUTPUT)
	