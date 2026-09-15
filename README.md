# Norbyte's Baldur's Gate 3 Script Extender

*[Version française](./README.fr.md)*

> **About this fork**: this is [ElwinghL](https://github.com/ElwinghL)'s
> fork of Norbyte's upstream Script Extender, tracked as a submodule from
> [BG3Tools](https://github.com/ElwinghL/BG3Tools) (`Tools/BG3 Script
> Extender/`). The bulk of the code below is Norbyte's own upstream work;
> the changes added on top in this fork are developed with heavy use of AI
> coding assistants.
>
> ⚠️ **About the use of AI**: given the ethical, moral, and ecological
> concerns this raises, those changes remain experimental and personal —
> they are not intended to end up used by the wider modding/dev
> community, and should be judged as such.

[Downloads available here](https://github.com/Norbyte/bg3se/releases)

The Script Extender adds Lua/Osiris scripting support to the game.
[API Documentation](https://github.com/Norbyte/bg3se/blob/master/Docs/API.md)

### Configuration

The following configuration variables can be set in the `ScriptExtenderSettings.json` file:

| Variable | Type | Default | Description |
|--|--|--|--|
| CreateConsole | Boolean | false | Creates a console window that logs extender internals. Mainly useful for debugging. |
| EnableLogging | Boolean | false | Enable logging of Osiris activity (rule evaluation, queries, etc.) to a log file. |
| LogRuntime | Boolean | false | Log extender console and script output to a log file. |
| LogCompile | Boolean | false | Log Osiris story compilation to a log file. |
| LogFailedCompile | Boolean | true | Log errors during Osiris story compilation to a log file. |
| LogDirectory | String | `My Documents\OsirisLogs` | Directory where the generated Osiris logs will be stored. |
| EnableExtensions | Boolean | true | Make the Osiris extension functionality available ingame or in the editor. |
| SendCrashReports | Boolean | true | Upload minidumps to the crash report collection server after a game crash. |
| ~~DumpNetworkStrings~~ | Boolean | Not implemented yet | Dumps the NetworkFixedString table to `LogDirectory`. Mainly useful for debugging desync issues. |
| DeveloperMode | Boolean | false | Enables various debug functionality for development purposes. |
| DisableModValidation | Boolean | true | Disable module hashing when loading modules. |
| EnableAchievements | Boolean | true | Re-enable achievements for modded games. |
| EnableDebugger | Boolean | false | Enables the Osiris debugger interface |
| DebuggerPort | Integer | 9999 | Port number the Osiris debugger will listen on |
| EnableLuaDebugger | Boolean | false | Enables the Lua debugger interface |
| LuaDebuggerPort | Integer | 9998 | Port number the Lua debugger will listen on  |

### Build Instructions

Run `first-time-setup.bat` from the MSVC x64 Native Tools cmdline after cloning the repo ensure that all external dependencies are set up correctly.
Afterwards you can build/develop the solution using normal Visual Studio tools.
