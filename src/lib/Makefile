
CC=g++
SRC := src
INCLUDE := ../include
SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, %.o, $(SOURCES))

all: $(OBJECTS)

%.o: $(SRC)/%.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
