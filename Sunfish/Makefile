GPP:=g++
GCC:=gcc

PROGRAM:=sunfish
SOURCES:=$(wildcard ./src/*.cpp) \
$(wildcard ./src/Shogi/*.cpp) \
$(wildcard ./src/Debug/*.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)

.PHONY: all
all: $(PROGRAM)
$(PROGRAM): $(OBJECTS)
	$(GPP) -o $(PROGRAM) $(CPPFLAGS) $^

.cpp.o:
	$(GPP) $(CFLAGS) -o $@ -c $<
