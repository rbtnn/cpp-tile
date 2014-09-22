
SRC_DIR = ./src
LOG = ./log.txt
CC = g++.exe
CPPFlAGS = -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing -Wno-unused-local-typedefs -I${BOOST_ROOT}
COMMON_HEADERS = ${SRC_DIR}/common_headers.h ${SRC_DIR}/common_functions.h ${SRC_DIR}/wndproc_functions.h
OBJECTS = common_functions.o Key.o ConfigReader.o Layout.o Workspace.o Recovery.o TilingWindowManager.o main.o resource.o



.PHONY: run all clean clean_objects clean_modules clean_log



all: tile.exe arrange.dll arrange_twin.dll arrange_maximal.dll arrange_cross.dll

run: tile.exe arrange.dll arrange_twin.dll arrange_maximal.dll arrange_cross.dll
	tile.exe

clean: clean_objects clean_modules clean_log

clean_objects:
	rm -f *.o

clean_modules:
	rm -f tile.exe
	rm -f arrange.dll
	rm -f arrange_twin.dll
	rm -f arrange_maximal.dll
	rm -f arrange_cross.dll

clean_log:
	rm -f ${LOG}

resource.o: tile.rc tile.ico
	windres tile.rc -o resource.o

main.o: ${SRC_DIR}/main.cpp ${COMMON_HEADERS} ${SRC_DIR}/tile/TilingWindowManager.h ${SRC_DIR}/tile/Layout.h ${SRC_DIR}/tile/ConfigReader.h
	${CC} -c $< ${CPPFlAGS}

common_functions.o: ${SRC_DIR}/common_functions.cpp ${SRC_DIR}/common_functions.h
	${CC} -c $< ${CPPFlAGS}

Key.o: ${SRC_DIR}/tile/Key.cpp ${SRC_DIR}/tile/Key.h ${COMMON_HEADERS}
	${CC} -c $< ${CPPFlAGS}

ConfigReader.o: ${SRC_DIR}/tile/ConfigReader.cpp ${SRC_DIR}/tile/ConfigReader.h ${COMMON_HEADERS}
	${CC} -c $< ${CPPFlAGS}

Layout.o: ${SRC_DIR}/tile/Layout.cpp ${SRC_DIR}/tile/Layout.h ${COMMON_HEADERS}
	${CC} -c $< ${CPPFlAGS}

Workspace.o: ${SRC_DIR}/tile/Workspace.cpp ${SRC_DIR}/tile/Workspace.h ${COMMON_HEADERS}
	${CC} -c $< ${CPPFlAGS}

Recovery.o: ${SRC_DIR}/tile/Recovery.cpp ${SRC_DIR}/tile/Recovery.h ${COMMON_HEADERS}
	${CC} -c $< ${CPPFlAGS}

TilingWindowManager.o: ${SRC_DIR}/tile/TilingWindowManager.cpp ${SRC_DIR}/tile/TilingWindowManager.h \
  ${SRC_DIR}/tile/Workspace.h ${SRC_DIR}/tile/ConfigReader.h ${SRC_DIR}/tile/Recovery.h ${COMMON_HEADERS}
	${CC} -c $< ${CPPFlAGS}

tile.exe: ${OBJECTS}
	${CC} $^ -o tile.exe -mwindows -s



arrange.o: ${SRC_DIR}/layout_methods/arrange.cpp ${SRC_DIR}/common_functions.h ${SRC_DIR}/layout_method.h
	${CC} -c $< ${CPPFlAGS}

arrange.dll: arrange.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias



arrange_twin.o: ${SRC_DIR}/layout_methods/arrange_twin.cpp ${SRC_DIR}/common_functions.h ${SRC_DIR}/layout_method.h
	${CC} -c $< ${CPPFlAGS}

arrange_twin.dll: arrange_twin.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias



arrange_maximal.o: ${SRC_DIR}/layout_methods/arrange_maximal.cpp ${SRC_DIR}/common_functions.h ${SRC_DIR}/layout_method.h
	${CC} -c $< ${CPPFlAGS}

arrange_maximal.dll: arrange_maximal.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias



arrange_cross.o: ${SRC_DIR}/layout_methods/arrange_cross.cpp ${SRC_DIR}/common_functions.h ${SRC_DIR}/layout_method.h
	${CC} -c $< ${CPPFlAGS}

arrange_cross.dll: arrange_cross.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

