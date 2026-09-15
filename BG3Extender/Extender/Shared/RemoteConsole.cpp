#include <stdafx.h>
#include <Extender/Shared/RemoteConsole.h>
#include <Extender/Shared/Console.h>
#include <Extender/ScriptExtender.h>

BEGIN_SE()

RemoteConsoleInterface::RemoteConsoleInterface(uint16_t port)
    : SocketInterface(port)
{}

void RemoteConsoleInterface::SendFrame(char tag, std::string const& payload)
{
    std::vector<uint8_t> buf;
    buf.reserve(payload.size() + 1);
    buf.push_back((uint8_t)tag);
    buf.insert(buf.end(), payload.begin(), payload.end());
    // SendProtobufMessage n'a rien de spécifique à protobuf : elle
    // préfixe juste `buf` de sa longueur (header inclus) et l'envoie --
    // exactement le framing attendu ici (voir RemoteConsole.h).
    SendProtobufMessage(buf.data(), (uint32_t)buf.size());
}

void RemoteConsoleInterface::SendLine(std::string const& line)
{
    SendFrame('L', line);
}

bool RemoteConsoleInterface::ProcessMessage(uint8_t* buf, uint32_t length)
{
    if (length == 0) {
        return true;
    }

    char tag = (char)buf[0];
    std::string payload(reinterpret_cast<char const*>(buf + 1), length - 1);

    // gCoreLibPlatformInterface.GlobalConsole est toujours un DebugConsole
    // (voir ScriptExtender::ScriptExtender()) -- HandleCommand/
    // HandleCompletionRequest ne sont définis que sur ce type dérivé.
    auto console = static_cast<DebugConsole*>(gCoreLibPlatformInterface.GlobalConsole);

    switch (tag) {
    case 'C':
        console->HandleCommand(payload);
        break;

    case 'T':
    {
        auto candidates = console->HandleCompletionRequest(payload);
        std::string joined;
        for (std::size_t i = 0; i < candidates.size(); i++) {
            if (i > 0) joined += '\x1f';
            joined += candidates[i];
        }
        SendFrame('R', joined);
        break;
    }

    default:
        WARN_LOCAL("RemoteConsoleInterface::ProcessMessage(): unknown tag '%c'", tag);
        break;
    }

    return true;
}

END_SE()
