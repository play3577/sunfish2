GPP:=g++
GCC:=gcc
RM:=rm

PROGRAM:=sunfish
SOURCES:=src/sunfish.cpp \
$(wildcard ./src/Shogi/*.cpp) \
$(wildcard ./src/Shogi/*/*.cpp) \
$(wildcard ./src/Evaluates/*.cpp) \
$(wildcard ./src/Search/*.cpp) \
$(wildcard ./src/Csa/*.cpp) \
$(wildcard ./src/Records/*.cpp) \
$(wildcard ./src/Cui/*.cpp) \
$(wildcard ./src/Network/*.cpp) \
$(wildcard ./src/Util/*.cpp) \
$(wildcard ./src/Tools/*.cpp) \
$(wildcard ./src/Log/*.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)
DEPENDS:=$(SOURCES:.cpp=.d)

DEV_PROGRAMS:=attackTable checkTable
DEV_SOURCES:=$(wildcard ./src/Shogi/*.cpp) \
$(wildcard ./src/Log/*.cpp)
DEV_OBJECTS:=$(DEV_SOURCES:.cpp=.o)
DEV_DEPENDS:=$(DEV_SOURCES:.cpp=.d)

override CFLAGS+=-Wall
override CFLAGS+=-W
override CFLAGS+=-O2
#override CFLAGS+=-O3
#override CFLAGS+=-NDEBUG
override CFLAGS+=-g
#override CFLAGS+=-pg
#override LIBS+=-lboost_thread-mt
#override LIBS+=-lboost_regex-mt
#override LIBS+=-lboost_program_options-mt
override LIBS+=-lboost_thread
override LIBS+=-lboost_regex
override LIBS+=-lboost_program_options

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): attackTable checkTable $(OBJECTS)
	$(GPP) -o $(PROGRAM) $(CFLAGS) $(OBJECTS) $(LIBS)

src/Shogi/Attack/attackTable.o : attackTable

attackTable : src/attackTable.cpp $(DEV_OBJECTS) src/Develop/attackTableGenerator.o
	$(GPP) -o $@ $(CFLAGS) $^ $(LIBS)
	./$@ src/Shogi/Attack/attackTable.cpp

src/Shogi/Attack/checkTable.o : checkTable

checkTable : src/checkTable.cpp $(DEV_OBJECTS) src/Develop/checkTableGenerator.o src/Shogi/Attack/attackTable.o
	$(GPP) -o $@ $(CFLAGS) $^ $(LIBS)
	./$@ src/Shogi/Attack/checkTable.cpp

.cpp.o:
	$(GPP) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJECTS) $(DEPENDS) $(DEV_PROGRAMS) $(DEV_OBJECTS) $(DEV_DEPENDS) src/*.o src/*.d

%.d: %.cpp
	@$(SHELL) -c '$(CC) -MM $(CFLAGS) $< | sed "s|^.*:|$*.o $@:|g" > $@; [ -s $@ ] || rm -f $@'

-include $(DEPENDS)
