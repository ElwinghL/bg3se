#pragma once

#include <GameDefinitions/Base/Base.h>
#include <functional>
#include <string>
#include <vector>

BEGIN_SE()

extern char const* BuildDate;

class DebugConsole : public Console
{
public:
    void Create() override;
    void HandleCommand(std::string const& cmd);
    // Complétion de préfixe pour le pont RemoteConsole (voir
    // Extender/Shared/RemoteConsole.h) : énumère les clés de la table
    // globale Lua (ou d'une sous-table à un niveau, ex. "Osi.Add...")
    // correspondant au préfixe partiel, dans le contexte courant
    // (serveur/client, voir `server`/`client`).
    std::vector<std::string> HandleCompletionRequest(std::string const& partial);
    void Print(DebugMessageType type, char const* msg) override;

private:
    bool consoleRunning_{ false };
    bool serverContext_{ true };
    bool multiLineMode_{ false };
    std::thread* consoleThread_{ nullptr };
    std::string multiLineCommand_;
    uint32_t width_{ 0 };
    uint32_t height_{ 0 };
    uint32_t resized_{ false };

    void ConsoleThread();
    void InputLoop();
    void UpdateConsoleSize();
    void SubmitTaskAndWait(bool server, std::function<void()> fun);
    void PrintHelp();
    void ResetLua();
    void ResetLuaClient();
    void ResetLuaServer();
    void ExecLuaCommand(std::string const& cmd);
    void ClearFromReset();
};

END_SE()
