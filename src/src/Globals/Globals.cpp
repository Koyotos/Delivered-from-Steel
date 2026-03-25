#include "include/Globals/Globals.hpp"

#ifdef __linux__
#include <unistd.h>
#include <sys/param.h>
    path Globals::GetExecPathInternal() {
        char pBuf[512];
        int bytes = MIN(readlink("/proc/self/exe", pBuf, 512), 511);
        if(bytes >= 0)
            pBuf[bytes] = '\0';
        return path(pBuf);
    }
#endif
#ifdef __WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    path Globals::GetExecPathInternal() {
        char pBuf[512];
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

void Globals::Log(const string& message) {
    log<<message<<"\n";
}

path Globals::GetExecPath() {
    return execPath;
}

path Globals::GetExecDir() {
    return execDir;
}

path Globals::GetUserDir() {
    return path();
}

Globals::Globals() {

    execPath = GetExecPathInternal();
    execDir = execPath.parent_path();
    //userDir = ; TODO

    log.open(execDir/path("log.txt"), ios::out);
    if(!log.good()) {
        throw runtime_error("Can't open log.");
    }
}

Globals::~Globals() {
    log.flush();
    log.close();
}