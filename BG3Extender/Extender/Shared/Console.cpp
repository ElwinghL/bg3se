#include <stdafx.h>
#include <Extender/Version.h>
#include <Extender/Shared/Console.h>
#include <Extender/ScriptExtender.h>
#include <WS2tcpip.h>

BEGIN_SE()

char const* BuildDate = __DATE__ " " __TIME__;

namespace
{
    // ANSI SGR color codes, replacing SetConsoleTextAttribute (no Win32
    // console anymore) - any real terminal client (socat/nc into a
    // terminal, or a Python client rendering these) understands these.
    char const* AnsiColorFor(DebugMessageType type)
    {
        switch (type) {
        case DebugMessageType::Error: return "\x1b[91m";
        case DebugMessageType::Warning: return "\x1b[93m";
        case DebugMessageType::Osiris: return "\x1b[96m";
        case DebugMessageType::Info: return "\x1b[97m";
        case DebugMessageType::Debug:
        default: return "\x1b[37m";
        }
    }
}

void DebugConsole::SubmitTaskAndWait(bool server, std::function<void()> task)
{
    if (server) {
        auto state = GetStaticSymbols().GetServerState();
        if (!state) {
            ERR("Cannot queue server commands when the server state machine is not initialized");
        } else if (*state == esv::GameState::Paused || *state == esv::GameState::Running) {
            gExtender->GetServer().SubmitTaskAndWait(task);
        } else {
            ERR("Cannot queue server commands in game state %s", EnumInfo<esv::GameState>::Find(*state).GetString());
        }
    } else {
        auto state = GetStaticSymbols().GetClientState();
        if (!state) {
            ERR("Cannot queue client commands when the client state machine is not initialized");
        } else if (*state == ecl::GameState::Menu 
            || *state == ecl::GameState::Lobby 
            || *state == ecl::GameState::Paused 
            || *state == ecl::GameState::Running) {
            gExtender->GetClient().SubmitTaskAndWait(task);
        } else {
            ERR("Cannot queue client commands in game state %s", EnumInfo<ecl::GameState>::Find(*state).GetString());
        }
    }
}

void DebugConsole::PrintHelp()
{
    DEBUG("Anything typed in will be executed as Lua code except the following special commands:");
    DEBUG("  server - Switch to server context");
    DEBUG("  client - Switch to client context");
    DEBUG("  reset client - Reset client Lua state");
    DEBUG("  reset server - Reset server Lua state");
    DEBUG("  reset - Reset client and server Lua states");
    DEBUG("  silence <on|off> - Enable/disable silent mode (log output when in input mode)");
    DEBUG("  clear - Clear the console");
    DEBUG("  exit - Leave console mode");
    DEBUG("  !<cmd> <arg1> ... <argN> - Trigger Lua \"ConsoleCommand\" event with arguments cmd, arg1, ..., argN");
}

void DebugConsole::ClearFromReset()
{
    // Clear console if the setting is enabled
    if (gExtender->GetConfig().ClearOnReset)
    {
        gCoreLibPlatformInterface.GlobalConsole->Clear();
    }
}

void DebugConsole::ResetLua()
{
    ClearFromReset();

    DEBUG("Resetting Lua states.");
    SubmitTaskAndWait(true, []() {
        gExtender->GetServer().ResetLuaState();
    });

    SubmitTaskAndWait(false, []() {
        if (!gExtender->GetServer().RequestResetClientLuaState()) {
            gExtender->GetClient().ResetLuaState();
        }
    });
}

void DebugConsole::ResetLuaClient()
{
    ClearFromReset();

    DEBUG("Resetting client Lua state.");
    SubmitTaskAndWait(false, []() {
        if (!gExtender->GetServer().RequestResetClientLuaState()) {
            gExtender->GetClient().ResetLuaState();
        }
    });
}

void DebugConsole::ResetLuaServer()
{
    ClearFromReset();

    DEBUG("Resetting server Lua state.");
    SubmitTaskAndWait(true, []() {
        gExtender->GetServer().ResetLuaState();
    });
}

void DebugConsole::ExecLuaCommand(std::string const& cmd)
{
    auto task = [cmd]() {
        auto state = gExtender->GetCurrentExtensionState();

        if (!state) {
            ERR("Extensions not initialized!");
            return;
        }

        LuaVirtualPin pin(*state);
        if (!pin) {
            ERR("Lua state not initialized!");
            return;
        }

        if (cmd[0] == '!') {
            lua::DoConsoleCommandEvent params;
            params.Command = cmd.substr(1);
            pin->ThrowEvent("DoConsoleCommand", params, false);
        } else {
            auto L = pin->GetState();
            lua::StaticLifetimeStackPin _(L, lua::LifetimeHandle{});
            lua::ProfilerStackGuard _p(&*pin);
            if (luaL_loadstring(L, cmd.c_str()) || lua::CallWithTraceback(L, 0, 0)) { // stack: errmsg
                ERR("%s", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
    };

    SubmitTaskAndWait(serverContext_, task);
}

void DebugConsole::HandleCommand(std::string const& cmd)
{
    if (cmd.empty()) {
    } else if (cmd == "server") {
        DEBUG("Switching to server context.");
        serverContext_ = true;
    } else if (cmd == "client") {
        DEBUG("Switching to client context.");
        serverContext_ = false;
#if defined(_DEBUG)
    } else if (cmd == "debugbreak") {
        gCoreLibPlatformInterface.EnableDebugBreak = !gCoreLibPlatformInterface.EnableDebugBreak;
        if (gCoreLibPlatformInterface.EnableDebugBreak) {
            DEBUG("Debug breaks ON");
        } else {
            DEBUG("Debug breaks OFF");
        }
#endif
    } else if (cmd == "reset") {
        ResetLua();
    } else if (cmd == "reset server") {
        ResetLuaServer();
    } else if (cmd == "reset client") {
        ResetLuaClient();
    } else if (cmd == "silence on") {
        DEBUG("Silent mode ON");
        silence_ = true;
    } else if (cmd == "silence off") {
        DEBUG("Silent mode OFF");
        silence_ = false;
    } else if (cmd == "clear") {
        Clear();
    } else if (cmd == "help") {
        PrintHelp();
    } else {
        ExecLuaCommand(cmd);
    }
}

void DebugConsole::Print(DebugMessageType type, char const* msg)
{
    Console::Print(type, msg);

    if (!silence_ && gExtender) {
        auto debugger = gExtender->GetLuaDebugger();
        if (debugger && debugger->IsDebuggerReady()) {
            debugger->OnLogMessage(type, msg);
        }
    }
}

void DebugConsole::SendRaw(char const* buf, std::size_t length)
{
    if (clientSocket_ == INVALID_SOCKET) return;

    std::lock_guard<std::mutex> _(sendMutex_);
    std::size_t sent = 0;
    while (sent < length) {
        int n = send(clientSocket_, buf + sent, (int)(length - sent), 0);
        if (n <= 0) {
            // Peer gone - let the accept thread's InputLoop notice via RecvLine
            // and close/reset clientSocket_ itself (single writer/reader here).
            return;
        }
        sent += (std::size_t)n;
    }
}

void DebugConsole::LocalPrint(DebugMessageType type, char const* msg)
{
    if (enabled_ && (!inputEnabled_ || !silence_)) {
        SendRaw(AnsiColorFor(type), strlen(AnsiColorFor(type)));
        SendRaw(msg, strlen(msg));
        SendRaw("\x1b[0m\r\n", 6);
    }

    if (logToFile_) {
        logFile_.write(msg, strlen(msg));
        logFile_.write("\r\n", 2);
        logFile_.flush();
    }
}

bool DebugConsole::RecvLine(std::string& line)
{
    line.clear();
    for (;;) {
        char ch;
        int n = recv(clientSocket_, &ch, 1, 0);
        if (n <= 0) {
            return false;
        }

        if (ch == '\n') {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            return true;
        }

        line += ch;
    }
}

void DebugConsole::InputLoop()
{
    std::string line;
    while (consoleRunning_) {
        inputEnabled_ = true;
        std::string prompt = serverContext_ ? "S" : "C";
        prompt += multiLineMode_ ? " -->> " : " >> ";
        SendRaw(prompt.data(), prompt.size());

        bool got = RecvLine(line);
        inputEnabled_ = false;

        if (!got) {
            // Client disconnected - go back to waiting for a new connection.
            break;
        }

        if (!multiLineMode_) {
            if (line == "exit") {
                break;
            }

            if (line == "--[[") {
                multiLineMode_ = true;
                multiLineCommand_.clear();
                continue;
            }

            HandleCommand(line);
        } else {
            if (line == "]]--") {
                multiLineMode_ = false;
                HandleCommand(multiLineCommand_);
            } else {
                multiLineCommand_ += line;
                multiLineCommand_ += '\n';
            }
        }
    }
}

void DebugConsole::SocketAcceptThread()
{
    while (consoleRunning_ && listenSocket_ != INVALID_SOCKET) {
        sockaddr_in addr;
        int addrlen = sizeof(addr);
        clientSocket_ = accept(listenSocket_, (sockaddr*)&addr, &addrlen);
        if (clientSocket_ == INVALID_SOCKET) {
            continue;
        }

        DEBUG("******************************************************************************");
        DEBUG("*                                                                            *");
        DEBUG("*                     BG3 Script Extender Debug Console                      *");
        DEBUG("*                                                                            *");
        DEBUG("******************************************************************************");
        DEBUG("");
        DEBUG("BG3Ext v%d built on %s", CurrentVersion, BuildDate);

        InputLoop();

        closesocket(clientSocket_);
        clientSocket_ = INVALID_SOCKET;
    }
}

void DebugConsole::Create()
{
    // NOTE: deliberately does NOT call Console::Create() - that path
    // (AllocConsole/freopen_s) is what 11a replaces. CoreLib::Console
    // (also used standalone by BG3Updater) is left untouched.
    EnableOutput(true);
    created_ = true;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    uint32_t ip;
    inet_pton(AF_INET, "127.0.0.1", &ip);
    listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = ip;
    addr.sin_port = htons((uint16_t)gExtender->GetConfig().ConsolePort);
    if (listenSocket_ == INVALID_SOCKET
        || bind(listenSocket_, (sockaddr*)&addr, sizeof(addr)) != 0
        || listen(listenSocket_, 1) != 0) {
        ERR("Could not start console socket on port %d: %d", gExtender->GetConfig().ConsolePort, WSAGetLastError());
        if (listenSocket_ != INVALID_SOCKET) {
            closesocket(listenSocket_);
            listenSocket_ = INVALID_SOCKET;
        }
        return;
    }

    consoleRunning_ = true;
    serverContext_ = !gExtender->GetConfig().DefaultToClientConsole;

    acceptThread_ = new std::thread(&DebugConsole::SocketAcceptThread, this);
}


END_SE()
