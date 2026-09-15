# Script Extender de Norbyte pour Baldur's Gate 3

*[English version](./README.md)*

> **À propos de ce fork** : ceci est le fork [ElwinghL](https://github.com/ElwinghL)
> du Script Extender de Norbyte, suivi comme sous-module depuis
> [BG3Tools](https://github.com/ElwinghL/BG3Tools) (`Tools/BG3 Script
> Extender/`). L'essentiel du code ci-dessous est le travail original de
> Norbyte ; les changements ajoutés par-dessus dans ce fork sont
> développés avec un usage important d'assistants IA.
>
> ⚠️ **À propos de l'usage de l'IA** : compte tenu des enjeux éthiques,
> moraux et écologiques que cela soulève, ces changements restent
> expérimentaux et personnels — ils n'ont pas vocation à finir utilisés
> par la communauté du modding/dev, et doivent être jugés comme tels.

[Téléchargements disponibles ici](https://github.com/Norbyte/bg3se/releases)

Le Script Extender ajoute la prise en charge du scripting Lua/Osiris au
jeu.
[Documentation de l'API](https://github.com/Norbyte/bg3se/blob/master/Docs/API.md)

### Configuration

Les variables de configuration suivantes peuvent être définies dans le
fichier `ScriptExtenderSettings.json` :

| Variable | Type | Par défaut | Description |
|--|--|--|--|
| CreateConsole | Booléen | false | Crée une fenêtre console qui journalise le fonctionnement interne de l'extender. Utile principalement pour le débogage. |
| EnableLogging | Booléen | false | Active la journalisation de l'activité Osiris (évaluation des règles, requêtes, etc.) dans un fichier de log. |
| LogRuntime | Booléen | false | Journalise la sortie console et script de l'extender dans un fichier de log. |
| LogCompile | Booléen | false | Journalise la compilation des histoires Osiris dans un fichier de log. |
| LogFailedCompile | Booléen | true | Journalise les erreurs survenues pendant la compilation des histoires Osiris dans un fichier de log. |
| LogDirectory | Chaîne | `My Documents\OsirisLogs` | Répertoire où seront stockés les logs Osiris générés. |
| EnableExtensions | Booléen | true | Rend disponible en jeu ou dans l'éditeur la fonctionnalité d'extension Osiris. |
| SendCrashReports | Booléen | true | Envoie les minidumps au serveur de collecte de rapports de plantage après un crash du jeu. |
| ~~DumpNetworkStrings~~ | Booléen | Pas encore implémenté | Exporte la table NetworkFixedString vers `LogDirectory`. Utile principalement pour déboguer les problèmes de désynchronisation. |
| DeveloperMode | Booléen | false | Active diverses fonctionnalités de débogage à des fins de développement. |
| DisableModValidation | Booléen | true | Désactive le hachage des modules au chargement de ceux-ci. |
| EnableAchievements | Booléen | true | Réactive les succès pour les parties moddées. |
| EnableDebugger | Booléen | false | Active l'interface de débogage Osiris |
| DebuggerPort | Entier | 9999 | Numéro de port sur lequel le débogueur Osiris écoutera |
| EnableLuaDebugger | Booléen | false | Active l'interface de débogage Lua |
| LuaDebuggerPort | Entier | 9998 | Numéro de port sur lequel le débogueur Lua écoutera |

### Instructions de build

Lancez `first-time-setup.bat` depuis la ligne de commande MSVC x64 Native
Tools après avoir cloné le dépôt, pour vous assurer que toutes les
dépendances externes sont correctement installées.
Ensuite, vous pouvez builder/développer la solution avec les outils
Visual Studio habituels.
