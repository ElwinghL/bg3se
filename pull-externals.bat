if "%VCToolsInstallDir%"=="" goto end

pushd .
cd External

rem ##### CURL #####
rem TODO - will need to make a proper build instead of fetching a static one

curl -L http://bg3se-updates.norbyte.dev/Stuff/curl.zip -o curl.zip
tar -xf curl.zip

rem ##### DETOURS #####

git clone https://github.com/microsoft/Detours --branch v4.0.1
cd Detours
nmake
cd ..

rem ##### GLM #####

git clone https://github.com/g-truc/glm --branch 1.0.3

rem ##### IMGUI #####

git clone https://github.com/Norbyte/imgui

rem ##### LUA #####

git clone https://github.com/Norbyte/lua-dos lua

rem ##### NOESIS #####

curl -L http://bg3se-updates.norbyte.dev/Stuff/NoesisGUI-NativeSDK-win-3.1.7-Indie.zip -o NoesisGUI-NativeSDK-win-3.1.7-Indie.zip
tar -xf NoesisGUI-NativeSDK-win-3.1.7-Indie.zip

rem ##### OPTICK #####

git clone https://github.com/Norbyte/optick

rem ##### PROTOBUF #####

curl -L https://github.com/protocolbuffers/protobuf/releases/download/v36.1/protoc-36.1-win64.zip -o protoc.zip
mkdir protoc
tar -xf protoc.zip -C protoc/


git clone https://github.com/protocolbuffers/protobuf --branch v36.1
cmake -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_FLAGS="/DWIN32 /D_WINDOWS /EHsc /D_ITERATOR_DEBUG_LEVEL=0" -S protobuf -B protobuf-build
cd protobuf-build
rem NOTE: protobuf v22+ no longer vendors Abseil as a git submodule; CMake
rem fetches it via FetchContent (cmake/abseil-cpp.cmake) into the same
rem generated solution. Building only /target:libprotobuf-lite skips its
rem Abseil static-lib dependencies, causing LNK2001/LNK1120 (unresolved
rem absl:: externals) when BG3Extender links against libprotobuf-lite.lib.
rem Build the whole solution instead so Abseil's targets are included.
msbuild protobuf.slnx "/p:Configuration=Debug" /m /nologo /consoleloggerparameters:summary
msbuild protobuf.slnx "/p:Configuration=Release" /m /nologo /consoleloggerparameters:summary
cd ..

rem ##### RAPIDJSON #####

git clone https://github.com/tencent/rapidjson
cd rapidjson
git reset --hard 24b5e7a8b27f42fa16b96fc70aade9106cf7102f
cd ..

rem ##### SDL #####
rem NOTE - Use the same version BG3 uses (2.30.6)

curl -L https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-devel-2.30.6-VC.zip -o SDL.zip
mkdir SDL
tar -xf SDL.zip -C SDL/ --strip-components 1

rem ##### TINYCRYPT #####
rem NOTE: Archived repo, no need to lock to specific commit

git clone https://github.com/intel/tinycrypt

rem ##### VK #####

git clone https://github.com/KhronosGroup/Vulkan-Headers Vulkan --branch vulkan-sdk-1.4.357

rem ##### GOOGLE.PROTOBUF (NuGet, LuaDebugger.csproj) #####
rem LuaDebugger.csproj is an old-style project using packages.config (no
rem PackageReference, no `nuget restore`/`msbuild /restore` step in this
rem workflow) : implicit MSBuild restore for packages.config expects a
rem net4x-named lib folder to consider a package compatible with the
rem project's net48 target. Google.Protobuf dropped its `lib/net45/` folder
rem starting with 3.35 (netstandard2.0/net8.0 only) - implicit restore then
rem silently fails for the WHOLE packages.config (not just this entry,
rem confirmed by "could not locate the assembly" MSB3245 warnings on every
rem package.config reference, not just Google.Protobuf, in a failed CI run).
rem Vendored explicitly here (same curl pattern as the other External
rem dependencies above) instead of depending on that restore mechanism -
rem version MUST match protoc's below (currently 36.1 -> NuGet 3.36.1),
rem generated C# code is not binary-compatible across protobuf versions.
curl -L https://api.nuget.org/v3-flatcontainer/google.protobuf/3.36.1/google.protobuf.3.36.1.nupkg -o google.protobuf.3.36.1.nupkg
mkdir ..\packages\Google.Protobuf.3.36.1
tar -xf google.protobuf.3.36.1.nupkg -C ..\packages\Google.Protobuf.3.36.1 lib/netstandard2.0/Google.Protobuf.dll

rem ##### ZIPLIB #####

git clone https://github.com/Norbyte/ZipLib
cd ZipLib
msbuild ZipLib.sln "/p:Configuration=Debug" /target:ZipLib /m /nologo /consoleloggerparameters:summary
msbuild ZipLib.sln "/p:Configuration=Release" /target:ZipLib /m /nologo /consoleloggerparameters:summary
cd ..

popd
exit /b

:no_msvc
echo pull-externals.bat must be invoked from the MSVC x64 Native Tools commandline prompt
pause
exit /b
