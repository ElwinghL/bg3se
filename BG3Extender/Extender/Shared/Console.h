#pragma once

#include <GameDefinitions/Base/Base.h>
#include <functional>
#include <mutex>
#include <WinSock2.h>

BEGIN_SE()

extern char const* BuildDate;

// Interactive Lua REPL console. Unlike the base Console class (CoreLib,
// shared with BG3Updater), this does NOT use a native Win32 console
// (AllocConsole/ReadConsoleW) - laggy under Wine/Proton's GUI rendering
// (11a). Instead it listens on a TCP loopback socket (ExtenderConfig::
// ConsolePort) and speaks plain text + ANSI color codes, so any terminal
// client (socat/nc) connected to 127.0.0.1:<ConsolePort> can be used
// in place of the game's own console window. Only DebugConsole is
// affected - CoreLib::Console (BG3Updater's plain console) is untouched.
class DebugConsole : public Console
{
public:
    void Create() override;
    void HandleCommand(std::string const& cmd);
    void Print(DebugMessageType type, char const* msg) override;
    void LocalPrint(DebugMessageType type, char const* msg) override;

private:
    bool consoleRunning_{ false };
    bool serverContext_{ true };
    bool multiLineMode_{ false };
    std::thread* acceptThread_{ nullptr };
    std::string multiLineCommand_;

    SOCKET listenSocket_{ INVALID_SOCKET };
    SOCKET clientSocket_{ INVALID_SOCKET };
    std::mutex sendMutex_;

    void SocketAcceptThread();
    void SendRaw(char const* buf, std::size_t length);
    bool RecvLine(std::string& line);
    void InputLoop();
    void SubmitTaskAndWait(bool server, std::function<void()> fun);
    void PrintHelp();
    void ResetLua();
    void ResetLuaClient();
    void ResetLuaServer();
    void ExecLuaCommand(std::string const& cmd);
    void ClearFromReset();
};

END_SE()
