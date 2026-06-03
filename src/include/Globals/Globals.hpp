#ifndef FE_GLOBALS
#define FE_GLOBALS

#include "include/Renderer/Font.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_map>

class AudioManager;
class WorldStateManager;
class SceneManager;
class CardManager;
class IOManager;
class Renderer;

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

    Font gameFont;

	std::unordered_map<int, bool> inputMap;
    std::unordered_map<int, bool> mouseMap;
	std::unordered_map<int, bool> gamepadBtnMap;
	std::unordered_map<int, float> gamepadAxisMap;

	float mouseX = 0.0f;
	float mouseY = 0.0f;

    float deltaTime = 0.0f;
    double physicsTime = 0.0;

    inline path GetExecPathInternal();

    public:
    std::shared_ptr<AudioManager> audioManager;
    std::shared_ptr<WorldStateManager> worldStateManager;
    std::shared_ptr<SceneManager> sceneManager;
    std::shared_ptr<CardManager> cardManager;
    std::shared_ptr<IOManager> ioManager;
    std::shared_ptr<Renderer> renderer;

    bool wantsToSave = false;
    bool wantsToLoad = false;

    std::string activeLevelName = "";

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

    /*
    @brief Returns font used by engine.
    @return const Font& - font
    */
    Font& GetGameFont() noexcept;

    /*
    @brief Sets font used by engine.
    @param1 const Font& - font
    @return void
    */
    void SetGameFont(Font);

	void SetKeyState(int key, bool isPressed);
	bool GetKeyState(int key) const;

	void SetMouseState(int button, bool isPressed);
	bool GetMouseState(int button) const;
	void SetMousePos(float x, float y);
	float GetMouseX() const;
	float GetMouseY() const;

	void SetGamepadBtnState(int button, bool isPressed);
	bool GetGamepadBtnState(int button) const;
	void SetGamepadAxisState(int axis, float value);
	float GetGamepadAxisState(int axis) const;

    void SetDeltaTime(float dt);
    float GetDeltaTime() const;

    void SetPhysicsTime(double t);
    double GetPhysicsTime() const;

    Globals(const Globals&) = delete;
    Globals();
    ~Globals();
};

#endif