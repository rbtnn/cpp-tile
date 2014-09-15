
SRC_DIR = ./src
LOG = ./log.txt
CC = g++.exe

.PHONY: run all clean

all: tile.exe test.dll

run: tile.exe test.dll
	tile.exe

clean:
	rm -f ${LOG}
	rm -f main.o
	rm -f tile.o
	rm -f common_functions.o
	rm -f tile.exe
	rm -f test.o
	rm -f test.dll

HEADERS= ${SRC_DIR}/tile.h ${SRC_DIR}/common_functions.h ${SRC_DIR}/common_headers.h ${SRC_DIR}/wndproc_functions.h

main.o: ${SRC_DIR}/main.cpp ${HEADERS}
	${CC} -c $^ -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing

tile.o: ${SRC_DIR}/tile.cpp ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing

common_functions.o: ${SRC_DIR}/common_functions.cpp ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing

tile.exe: common_functions.o tile.o main.o
	${CC} $^ -o $@ -mwindows -s


test.o: ${SRC_DIR}/test.cpp ${SRC_DIR}/common_functions.h
	${CC} -c $^ -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing

test.dll: test.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

