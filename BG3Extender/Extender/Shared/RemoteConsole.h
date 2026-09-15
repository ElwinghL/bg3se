#pragma once

#include <Osiris/Debugger/DebugInterface.h>
#include <string>

BEGIN_SE()

// Pont TCP "console distante" pour outils externes (typiquement
// bg3_mod_tui/bg3se_remote_console.py) : réutilise le socket bas niveau de
// SocketInterface (accept loop, framing longueur 4 octets little-endian
// header inclus -- voir DebugInterface.h/.cpp) avec un framing tag+texte au
// lieu de protobuf, pour rester lisible/débogable sans dépendance externe
// côté client.
//
// Format d'une trame (une fois le header de longueur consommé par
// SocketInterface) : 1 octet de tag ASCII, puis le payload UTF-8.
//   Client -> serveur :
//     'C' : commande à exécuter, comme si elle était tapée dans la console
//           native (voir DebugConsole::HandleCommand -- "server"/"client"/
//           "reset"/"silence on|off"/"clear"/"help"/Lua brut).
//     'T' : requête de complétion ; le payload est le préfixe partiel (voir
//           DebugConsole::HandleCompletionRequest).
//   Serveur -> client :
//     'L' : une ligne de log (miroir de DebugConsole::Print).
//     'R' : réponse de complétion -- candidats séparés par '\x1f'.
class RemoteConsoleInterface : public SocketInterface
{
public:
    RemoteConsoleInterface(uint16_t port);

    void SendLine(std::string const& line);

protected:
    bool ProcessMessage(uint8_t* buf, uint32_t length) override;

private:
    void SendFrame(char tag, std::string const& payload);
};

END_SE()
