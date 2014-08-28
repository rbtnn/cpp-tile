
BIN_DIR = ./bin
SRC_DIR = ./src
EXECNAME = ${BIN_DIR}/tile.exe
SRC = ${SRC_DIR}/tile.cpp
LOG = ./log.txt
CFLAGS = -std=c++0x -pedantic -Wall -Os -fno-strict-aliasing -s -mwindows
CC = g++.exe

all: build

${EXECNAME}: ${SRC}
	@mkdir ${BIN_DIR}
	${CC} ${SRC} ${CFLAGS} -o ${EXECNAME}

build: ${EXECNAME}

run: ${EXECNAME}
	${EXECNAME}

clean:
	rm -f ${LOG}
	rm -rf ${BIN_DIR}

.PHONY: all clean

