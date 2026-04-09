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
#ifdef _WIN32
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

Font& Globals::GetGameFont() noexcept {
    return gameFont;
}

void Globals::SetGameFont(Font f) {
    gameFont = f;
}

void Globals::SetKeyState(int key, bool isPressed) {
    inputMap[key] = isPressed;
}

bool Globals::GetKeyState(int key) const {
    auto it = inputMap.find(key);
    if (it != inputMap.end()) {
        return it->second;
    }
    return false;
}

void Globals::SetMouseState(int button, bool isPressed) {
    mouseMap[button] = isPressed;
}

bool Globals::GetMouseState(int button) const {
    auto it = mouseMap.find(button);
    if (it != mouseMap.end()) {
        return it->second;
    }
    return false;
}

void Globals::SetMousePos(float x, float y) {
    mouseX = x;
    mouseY = y;
}

float Globals::GetMouseX() const {
    return mouseX;
}

float Globals::GetMouseY() const {
    return mouseY;
}

void Globals::SetGamepadBtnState(int button, bool isPressed) {
    gamepadBtnMap[button] = isPressed;
}

bool Globals::GetGamepadBtnState(int button) const {
    auto it = gamepadBtnMap.find(button);
    if (it != gamepadBtnMap.end()) {
        return it->second;
    }
    return false;
}

void Globals::SetGamepadAxisState(int axis, float value) {
    gamepadAxisMap[axis] = value;
}

float Globals::GetGamepadAxisState(int axis) const {
    auto it = gamepadAxisMap.find(axis);
    if (it != gamepadAxisMap.end()) {
        return it->second;
    }
    return 0.0f;
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