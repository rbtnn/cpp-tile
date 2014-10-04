
SRC_DIR = ./src
LOG = ./log.txt
CC = g++.exe
CPPFlAGS = -std=c++11 -pedantic -Wall -Os -fno-strict-aliasing -Wno-unused-local-typedefs -I${BOOST_ROOT}


.PHONY: all clean clean_objects clean_modules clean_log


all: tile.exe arrange.dll arrange_twin.dll arrange_maximal.dll arrange_cross.dll arrange_square.dll arrange_manual.dll


clean: clean_objects clean_modules clean_log

clean_objects:
	rm -f *.o

clean_modules:
	rm -f tile.exe
	rm -f arrange.dll
	rm -f arrange_twin.dll
	rm -f arrange_maximal.dll
	rm -f arrange_cross.dll
	rm -f arrange_square.dll
	rm -f arrange_manual.dll

clean_log:
	rm -f ${LOG}

resource.o: tile.rc tile.ico
	windres tile.rc -o resource.o


${SRC_DIR}/common_functions.cpp: ${SRC_DIR}/common_headers.h
${SRC_DIR}/layout_methods/arrange.cpp: ${SRC_DIR}/layout_method.h
${SRC_DIR}/layout_methods/arrange_cross.cpp: ${SRC_DIR}/layout_method.h
${SRC_DIR}/layout_methods/arrange_manual.cpp: ${SRC_DIR}/layout_method.h
${SRC_DIR}/layout_methods/arrange_maximal.cpp: ${SRC_DIR}/layout_method.h
${SRC_DIR}/layout_methods/arrange_square.cpp: ${SRC_DIR}/layout_method.h
${SRC_DIR}/layout_methods/arrange_twin.cpp: ${SRC_DIR}/layout_method.h

${SRC_DIR}/main.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/TilingWindowManager.h \
  ${SRC_DIR}/tile/Layout.h

${SRC_DIR}/tile/ConfigReader.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/ConfigReader.h \
  ${SRC_DIR}/tile/NotApplyStyleToClassNames.h \
  ${SRC_DIR}/tile/IgnoreClassNamesArranged.h

${SRC_DIR}/tile/Key.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/Key.h

${SRC_DIR}/tile/Layout.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/Layout.h

${SRC_DIR}/tile/Recovery.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/Recovery.h

${SRC_DIR}/tile/TilingWindowManager.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/TilingWindowManager.h \
  ${SRC_DIR}/tile/ConfigReader.h \
  ${SRC_DIR}/tile/Workspace.h

${SRC_DIR}/tile/Workspace.cpp: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/wndproc_functions.h \
  ${SRC_DIR}/tile/Workspace.h \
  ${SRC_DIR}/tile/IgnoreClassNamesArranged.h \
  ${SRC_DIR}/tile/NotApplyStyleToClassNames.h

${SRC_DIR}/common_functions.h: ${SRC_DIR}/common_headers.h

${SRC_DIR}/layout_method.h: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/common_functions.h \
  ${SRC_DIR}/tile/Layout.h

${SRC_DIR}/tile/ConfigReader.h: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/tile/NotApplyStyleToClassNames.h \
  ${SRC_DIR}/tile/IgnoreClassNamesArranged.h \
  ${SRC_DIR}/tile/LayoutMethodNames.h

${SRC_DIR}/tile/IgnoreClassNamesArranged.h: ${SRC_DIR}/tile/NewType.h
${SRC_DIR}/tile/Key.h: ${SRC_DIR}/common_headers.h
${SRC_DIR}/tile/Layout.h: ${SRC_DIR}/common_headers.h
${SRC_DIR}/tile/LayoutMethodNames.h: ${SRC_DIR}/tile/NewType.h
${SRC_DIR}/tile/NotApplyStyleToClassNames.h: ${SRC_DIR}/tile/NewType.h
${SRC_DIR}/tile/Recovery.h: ${SRC_DIR}/common_headers.h

${SRC_DIR}/tile/TilingWindowManager.h: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/tile/ConfigReader.h \
  ${SRC_DIR}/tile/Key.h \
  ${SRC_DIR}/tile/Workspace.h \
  ${SRC_DIR}/tile/Layout.h \
  ${SRC_DIR}/tile/Recovery.h

${SRC_DIR}/tile/Workspace.h: \
  ${SRC_DIR}/common_headers.h \
  ${SRC_DIR}/tile/Layout.h \
  ${SRC_DIR}/tile/IgnoreClassNamesArranged.h \
  ${SRC_DIR}/tile/NotApplyStyleToClassNames.h

${SRC_DIR}/wndproc_functions.h: ${SRC_DIR}/common_headers.h



main.o: ${SRC_DIR}/main.cpp
	${CC} -c $< ${CPPFlAGS}

common_functions.o: ${SRC_DIR}/common_functions.cpp
	${CC} -c $< ${CPPFlAGS}

Key.o: ${SRC_DIR}/tile/Key.cpp
	${CC} -c $< ${CPPFlAGS}

ConfigReader.o: ${SRC_DIR}/tile/ConfigReader.cpp
	${CC} -c $< ${CPPFlAGS}

Layout.o: ${SRC_DIR}/tile/Layout.cpp
	${CC} -c $< ${CPPFlAGS}

Workspace.o: ${SRC_DIR}/tile/Workspace.cpp
	${CC} -c $< ${CPPFlAGS}

Recovery.o: ${SRC_DIR}/tile/Recovery.cpp
	${CC} -c $< ${CPPFlAGS}

HotKey.o: ${SRC_DIR}/tile/HotKey.cpp
	${CC} -c $< ${CPPFlAGS}

TilingWindowManager.o: ${SRC_DIR}/tile/TilingWindowManager.cpp
	${CC} -c $< ${CPPFlAGS}

arrange.o: ${SRC_DIR}/layout_methods/arrange.cpp
	${CC} -c $< ${CPPFlAGS}

arrange_twin.o: ${SRC_DIR}/layout_methods/arrange_twin.cpp
	${CC} -c $< ${CPPFlAGS}

arrange_maximal.o: ${SRC_DIR}/layout_methods/arrange_maximal.cpp
	${CC} -c $< ${CPPFlAGS}

arrange_cross.o: ${SRC_DIR}/layout_methods/arrange_cross.cpp
	${CC} -c $< ${CPPFlAGS}

arrange_square.o: ${SRC_DIR}/layout_methods/arrange_square.cpp
	${CC} -c $< ${CPPFlAGS}

arrange_manual.o: ${SRC_DIR}/layout_methods/arrange_manual.cpp
	${CC} -c $< ${CPPFlAGS}




tile.exe: common_functions.o Key.o ConfigReader.o Layout.o Workspace.o Recovery.o TilingWindowManager.o main.o resource.o HotKey.o
	${CC} $^ -o tile.exe -mwindows -s

arrange.dll: arrange.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

arrange_twin.dll: arrange_twin.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

arrange_maximal.dll: arrange_maximal.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

arrange_cross.dll: arrange_cross.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

arrange_square.dll: arrange_square.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

arrange_manual.dll: arrange_manual.o common_functions.o
	${CC} $^ -o $@ -mwindows -shared -Wl,--add-stdcall-alias

