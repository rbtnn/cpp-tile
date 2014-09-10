
BIN_DIR = ./bin
SRC_DIR = ./src
EXECNAME = ${BIN_DIR}/tile.exe
DLLNAME = ${BIN_DIR}/test.dll
SRC = 
LOG = ./log.txt
CC = g++.exe

all: build

${EXECNAME}: ${SRC_DIR}/tile.cpp
	${CC} $< -o $@ -mwindows -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing -s

${DLLNAME}: ${SRC_DIR}/test.cpp
	${CC} $< -o $@ -mwindows -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing -shared -Wl,--add-stdcall-alias

build: ${EXECNAME} ${DLLNAME}

run: ${EXECNAME}
	${EXECNAME}

clean:
	rm -f ${LOG}
	rm -rf ${BIN_DIR}

.PHONY: all clean

