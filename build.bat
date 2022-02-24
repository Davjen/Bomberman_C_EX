@REM @echo off
@REM set BAT_BASE_PATH=%~dp0
@REM set PRJ_BASE_PATH=%BAT_BASE_PATH%
@REM set BIN_PATH=%PRJ_BASE_PATH%\bin
@REM set PRJ_PATH=%PRJ_BASE_PATH%\core
@REM set PRJ_CLIENT_PATH=%PRJ_BASE_PATH%\client

@REM MD "%BIN_PATH%"
@REM clang -I"%PRJ_PATH%\include" -I"%PRJ_CLIENT_PATH%\include" -o "%BIN_PATH%\core.lib" "%PRJ_PATH%\src\*.c" "%PRJ_CLIENT_PATH%\src\*.c" -fuse-ld=llvm-lib


clang.exe -o .\bin\bomberman.exe -I .\core\include -I .\SDL2-2.0.20\include\ -I .\client\include -I .\zlib\ -L .\SDL2-2.0.20\lib\x64\ -L .\zlib\ .\app\src\main.c .\core\src\*.c .\client\src\*.c -Xlinker /subsystem:console -lSDL2main -lSDL2 -lshell32 -lws2_32.lib