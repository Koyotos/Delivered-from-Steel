#ifndef FE_GLOBALS
#define FE_GLOBALS

#include <filesystem>
#include <memory>

using namespace std;
using namespace filesystem;

class Globals {
    private: 

    public:
    static Globals& GetGlobals();
    void Initialize();

    path GetExecPath();
    path GetUserDir();

    Globals(const Globals&) = delete;
    Globals();
    ~Globals();
};

#endif