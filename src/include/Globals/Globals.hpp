#ifndef FE_GLOBALS
#define FE_GLOBALS

#include <filesystem>
#include <fstream>
#include <memory>

using namespace std;
using namespace filesystem;

/*
Singleton that is responsible for storing al game data that needs to be
accessible for all modules during runtime. Obtain it's instance via GetGlobals() method.
*/
class Globals {
    private: 
    fstream log;

    path execPath;
    path execDir;
    path userDir;

    inline path GetExecPathInternal();

    public:
    /*
    @brief Returns instance of Globals class.
    @return Globals& - globals instance
    */
    static Globals& GetGlobals();

    /*
    @brief Saves log message into file created in exec location.
    @param1 const string& - message to save
    */
    void Log(const string&);

    /*
    @brief Returns path to executable location. Includes exec name.
    @return path - exec path
    */
    path GetExecPath();

    /*
    @brief Returns path to executable location. It is path to folder in which exec is running.
    @return path - directory exec path
    */
    path GetExecDir();

    /*
    @brief Returns path to user directory location.
    @return path - user directory path
    */
    path GetUserDir();

    Globals(const Globals&) = delete;
    Globals();
    ~Globals();
};

#endif