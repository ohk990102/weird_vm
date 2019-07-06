CC	=g++
SRC := src
INCLUDE := include
LIB := lib
LIBS := $(LIB)/socket.o $(LIB)/logger.o
SOLUTION := solution
CFLAGS := -static -s

TARGETS := executor logger_client server mounthide

all: $(LIB) $(SOLUTION) $(TARGETS) weird_vm.tar.gz

$(LIB):
	$(MAKE) -C $@

$(SOLUTION):
	$(MAKE) -C $@

executor: $(SRC)/executor.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o bin/$@ -lseccomp

logger_client: $(SRC)/logger_client.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o bin/$@

server: $(SRC)/server.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o bin/$@ -lpthread

mounthide: $(SRC)/mounthide.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o bin/$@ -lpthread

weird_vm.tar.gz: bin/ script.sh flag
	tar -czvf $@ $^

clean:
	rm -f bin/*
	rm -f weird_vm.tar.gz
	$(MAKE) -C $(LIB) clean
	$(MAKE) -C $(SOLUTION) clean

.PHONY: all $(LIB) $(SOLUTION)
