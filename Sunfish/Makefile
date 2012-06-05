GPP:=g++
GCC:=gcc
RM:=rm

PROGRAM:=sunfish
SOURCES:=$(wildcard ./src/*.cpp) \
$(wildcard ./src/Shogi/*.cpp) \
$(wildcard ./src/Debug/*.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)
DEPENDS=$(SOURCES:.cpp=.d)

.PHONY: all
all: $(PROGRAM)
$(PROGRAM): $(OBJECTS)
	$(GPP) -o $(PROGRAM) $(CPPFLAGS) $^

.cpp.o:
	$(GPP) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJECTS) $(DEPENDS)

%.d: %.cpp
	@$(SHELL) -c 'echo $*.o $@ `$(CC) -MM $(CPPFLAGS) $< | sed "s/^[^ ]*\.o[ :]*/: /g"` | sed "s/\\\\//g" > $@'

-include $(DEPENDS)
