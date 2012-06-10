GPP:=g++
GCC:=gcc
RM:=rm

PROGRAM:=sunfish
SOURCES:=$(wildcard ./src/*.cpp) \
$(wildcard ./src/Shogi/*.cpp) \
$(wildcard ./src/Csa/*.cpp) \
$(wildcard ./src/Util/*.cpp) \
$(wildcard ./src/Tools/*.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)
DEPENDS:=$(SOURCES:.cpp=.d)

override CFLAGS+=-Wall
override CFLAGS+=-W
#override CFLAGS+=-O2
#override CFLAGS+=-NDEBUG
override CFLAGS+=-g

.PHONY: all
all: $(PROGRAM)
$(PROGRAM): $(OBJECTS)
	$(GPP) -o $(PROGRAM) $(CFLAGS) $^

.cpp.o:
	$(GPP) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJECTS) $(DEPENDS)

%.d: %.cpp
	@$(SHELL) -c '$(CC) -MM $(CFLAGS) $< | sed "s|^.*:|$*.o $@:|g" > $@; [ -s $@ ] || rm -f $@'

-include $(DEPENDS)
