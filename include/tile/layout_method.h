
#ifndef TILE_LAYOUT_METHOD_H
#define TILE_LAYOUT_METHOD_H

#include "./common_headers.h"
#include "./common_functions.h"
#include "./Layout.h"

#define EXPORT extern "C" __declspec(dllexport)

BOOL WINAPI DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
  switch(ul_reason_for_call){
    case DLL_PROCESS_ATTACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
  }
  return TRUE;
}

#endif

