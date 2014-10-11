
VPATH = src src/tile src/layout_methods include
CPPFLAGS = -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing -Wno-unused-local-typedefs -I$(BOOST_ROOT) -Iinclude
SOURCES = main.cpp \
  common_functions.cpp \
  Key.cpp \
  ConfigReader.cpp \
  Layout.cpp \
  Workspace.cpp \
  Recovery.cpp \
  HotKey.cpp \
  TilingWindowManager.cpp \
  arrange.cpp \
  arrange_twin.cpp \
  arrange_maximal.cpp \
  arrange_cross.cpp \
  arrange_square.cpp \
  arrange_manual.cpp

OBJECTS = $(foreach obj,$(subst .cpp,.o,$(SOURCES)),$(notdir $(obj)))
DEPENDS = $(foreach obj,$(subst .cpp,.d,$(SOURCES)),$(notdir $(obj)))

.PHONY: all depend clean

all: depend tile.exe arrange.dll arrange_twin.dll arrange_maximal.dll arrange_cross.dll arrange_square.dll arrange_manual.dll

depend: $(DEPENDS)

clean:
	rm -f *.o
	rm -f *.d
	rm -f tile.exe
	rm -f arrange_.exe
	rm -f arrange*.dll


-include *.d

.cpp.o:
	$(CXX) -o $@ $< -c $(CPPFLAGS)

%.d: %.cpp
	$(CXX) -MM $^ -c $(CPPFLAGS) > $@

%.dll: %.o

%.dll: %.o common_functions.o
	${CXX} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

resource.o: tile.rc tile.ico
	windres tile.rc -o resource.o

tile.exe: common_functions.o Key.o ConfigReader.o Layout.o Workspace.o Recovery.o TilingWindowManager.o main.o resource.o HotKey.o
	${CXX} $^ -o $@ -mwindows -s

