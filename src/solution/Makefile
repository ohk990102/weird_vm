CC	=g++
INCLUDE := ../include
LIB := ../lib
LIBS := $(LIB)/socket.o $(LIB)/logger.o
SOURCES := $(wildcard $(SRC)/*.cpp)
TARGETS := test1 test2 test3
CFLAGS := -static

all: $(TARGETS)

test1: test1.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o $@

test2: test2.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o $@

test3: test3.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) $< -o $@

clean:
	rm -f $(TARGETS)

