#include "include/Globals/Globals.hpp"

#ifdef __linux__
#include <unistd.h>
#include <sys/param.h>
    path Globals::GetExecPath() {
        char pBuf[512];
        int bytes = MIN(readlink("/proc/self/exe", pBuf, 512), 511);
        if(bytes >= 0)
            pBuf[bytes] = '\0';
        return path(pBuf);
    }
#endif
#ifdef __WIN32 // REQUIRES WINDOWS TESTS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    path Globals::GetExecPath()() {
        char pBuf[512]
        DWORD result = GetModuleFileNameA(NULL, pBuf, 512);
        if (result == 0 || result >= 512) {
            pBuf[0] = '\0';
        }
        return path(pBuf);
    }
#endif

Globals& Globals::GetGlobals() {
    static Globals instance;
    return instance;
}

void Globals::Initialize() {

}

path Globals::GetUserDir() {

}