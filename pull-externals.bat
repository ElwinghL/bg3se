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

rem ##### NUGET PACKAGES FOR LuaDebugger.csproj (packages.config) #####
rem LuaDebugger.csproj is an old-style project using packages.config (no
rem PackageReference, no `nuget restore`/`msbuild /restore` step in this
rem workflow). Its implicit MSBuild restore turned out unreliable in CI :
rem it silently fails for the WHOLE packages.config as soon as ONE
rem referenced package has no net4x-named lib folder (confirmed by
rem "could not locate the assembly" MSB3245 warnings on every single
rem packages.config reference, not just the one at fault, in a failed CI
rem run) - a regression triggered here by bumping Google.Protobuf to 3.36.1
rem (dropped its lib/net45/ folder, netstandard2.0/net8.0 only from 3.35).
rem Rather than depend on that fragile implicit-restore mechanism at all,
rem every packages.config entry of LuaDebugger.csproj is vendored
rem explicitly below (same curl pattern as the other External dependencies
rem above), matching the exact id/version/HintPath already declared in
rem LuaDebugger.csproj/packages.config. Google.Protobuf's version MUST
rem match protoc's below (currently 36.1 -> NuGet 3.36.1) - generated C#
rem code is not binary-compatible across protobuf versions; the other 4
rem packages keep the versions already pinned in packages.config.
curl -L https://api.nuget.org/v3-flatcontainer/google.protobuf/3.36.1/google.protobuf.3.36.1.nupkg -o google.protobuf.3.36.1.nupkg
mkdir ..\packages\Google.Protobuf.3.36.1
tar -xf google.protobuf.3.36.1.nupkg -C ..\packages\Google.Protobuf.3.36.1 lib/netstandard2.0/Google.Protobuf.dll

curl -L https://api.nuget.org/v3-flatcontainer/newtonsoft.json/13.0.4/newtonsoft.json.13.0.4.nupkg -o newtonsoft.json.13.0.4.nupkg
mkdir ..\packages\Newtonsoft.Json.13.0.4
tar -xf newtonsoft.json.13.0.4.nupkg -C ..\packages\Newtonsoft.Json.13.0.4 lib/net45/Newtonsoft.Json.dll

curl -L https://api.nuget.org/v3-flatcontainer/system.buffers/4.4.0/system.buffers.4.4.0.nupkg -o system.buffers.4.4.0.nupkg
mkdir ..\packages\System.Buffers.4.4.0
tar -xf system.buffers.4.4.0.nupkg -C ..\packages\System.Buffers.4.4.0 lib/netstandard2.0/System.Buffers.dll

curl -L https://api.nuget.org/v3-flatcontainer/system.memory/4.5.3/system.memory.4.5.3.nupkg -o system.memory.4.5.3.nupkg
mkdir ..\packages\System.Memory.4.5.3
tar -xf system.memory.4.5.3.nupkg -C ..\packages\System.Memory.4.5.3 lib/netstandard2.0/System.Memory.dll

curl -L https://api.nuget.org/v3-flatcontainer/system.numerics.vectors/4.4.0/system.numerics.vectors.4.4.0.nupkg -o system.numerics.vectors.4.4.0.nupkg
mkdir ..\packages\System.Numerics.Vectors.4.4.0
tar -xf system.numerics.vectors.4.4.0.nupkg -C ..\packages\System.Numerics.Vectors.4.4.0 lib/net46/System.Numerics.Vectors.dll

curl -L https://api.nuget.org/v3-flatcontainer/system.runtime.compilerservices.unsafe/4.5.2/system.runtime.compilerservices.unsafe.4.5.2.nupkg -o system.runtime.compilerservices.unsafe.4.5.2.nupkg
mkdir ..\packages\System.Runtime.CompilerServices.Unsafe.4.5.2
tar -xf system.runtime.compilerservices.unsafe.4.5.2.nupkg -C ..\packages\System.Runtime.CompilerServices.Unsafe.4.5.2 lib/netstandard2.0/System.Runtime.CompilerServices.Unsafe.dll

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
