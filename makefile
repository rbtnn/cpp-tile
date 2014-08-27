
BIN_DIR = ./bin
SRC_DIR = ./src
EXECNAME = ${BIN_DIR}/tile.exe
SRC = ${SRC_DIR}/tile.cpp
LOG = ./log.txt
CFLAGS = -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing -s -mwindows
CC = g++.exe

all: build

${EXECNAME}: ${SRC}
	${CC} ${SRC} ${CFLAGS} -o ${EXECNAME}

build: ${EXECNAME}

run: ${EXECNAME}
	${EXECNAME}

clean:
	rm -f ${LOG} ${EXECNAME}

.PHONY: all clean

