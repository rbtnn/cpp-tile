
#include <windows.h>

#define EXPORT extern "C" __declspec(dllexport)

EXPORT void MyFunction(int a){
  ::MessageBox(NULL, "hi", "", MB_OK);
}

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

