
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT      0x0500

#include <windows.h>
#include <winuser.h>
#include <shellapi.h>

// #define DEBUG

#ifdef DEBUG
#  include <cstdio>
#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <deque>
#include <map>
#include <functional>
#include <algorithm>

#include <boost/optional.hpp>

#include <stdlib.h>
#include <time.h>

#define buffer_size 1024

