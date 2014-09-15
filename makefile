
SRC_DIR = ./src
LOG = ./log.txt
CC = g++.exe

.PHONY: run all clean clean_objects clean_modules clean_log

all: tile.exe test.dll clean_objects

run: tile.exe test.dll
	tile.exe

clean: clean_objects clean_modules clean_log

clean_objects:
	rm -f *.o

clean_modules:
	rm -f tile.exe
	rm -f test.dll

clean_log:
	rm -f ${LOG}

HEADERS= ${SRC_DIR}/common_functions.h ${SRC_DIR}/common_headers.h ${SRC_DIR}/wndproc_functions.h

main.o: ${SRC_DIR}/main.cpp ${HEADERS}
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

common_functions.o: ${SRC_DIR}/common_functions.cpp ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

Key.o: ${SRC_DIR}/tile/Key.cpp ${SRC_DIR}/tile/Key.h ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

ConfigReader.o: ${SRC_DIR}/tile/ConfigReader.cpp ${SRC_DIR}/tile/ConfigReader.h ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

Layout.o: ${SRC_DIR}/tile/Layout.cpp ${SRC_DIR}/tile/Layout.h ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

Workspace.o: ${SRC_DIR}/tile/Workspace.cpp ${SRC_DIR}/tile/Workspace.h ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

TilingWindowManager.o: ${SRC_DIR}/tile/TilingWindowManager.cpp ${SRC_DIR}/tile/TilingWindowManager.h ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

tile.exe: common_functions.o Key.o ConfigReader.o Layout.o Workspace.o TilingWindowManager.o main.o
	${CC} $^ -o $@ -mwindows -s

test.o: ${SRC_DIR}/test.cpp ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing

test.dll: test.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

