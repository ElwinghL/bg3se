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
