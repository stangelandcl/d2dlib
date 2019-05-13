@echo off
setlocal


set ROOT=%~dp0


set PLAT=x64
set CONF=Debug
call :build
set CONF=Release
call :build
set PLAT=x86
call :build
set CONF=Debug
call :build

exit /b 0

:build
setlocal
set INC=/I %ROOT%src
set DEFS=/D_WIN32_WINNT=0x0601 /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS/D_WINDOWS /D_USRDLL /D_UNICODE /DUNICODE

if "%CONF%" == "Debug" set DEFS=%DEFS%  /DDEBUG=1 /D_DEBUG
set WARN=
set COMP=/Z7 /nologo /arch:AVX2
set LIBS=d2d1.lib user32.lib ole32.lib dwrite.lib windowscodecs.lib

set ARCH=amd64
if "%PLAT%" == "x86" set ARCH=x86
set OPT=/O2 /GL- /MD
if "%CONF%" == "Debug" set OPT=/Od /GL- /MDd
set COMP=%COMP% %OPT%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" %ARCH% 10.0.16299.0
set DIR=%PLAT%\%CONF%
mkdir %DIR%
pushd %DIR%
del /Q *.dll *.obj *.lib test.exe


cl /c %WARN% %COMP% /clr %DEFS% %INC% %ROOT%src\unity.cpp
link /dll /nologo unity.obj %LIBS% /Out:Direct2D.dll /DEBUG:FULL

mkdir %ROOT%..\work\lib\dotnet\Direct2D\%PLAT%\%CONF%
copy /b /Y Direct2D.dll %ROOT%..\work\lib\dotnet\Direct2D\%PLAT%\%CONF%
copy /b /Y Direct2D.pdb %ROOT%..\work\lib\dotnet\Direct2D\%PLAT%\%CONF%
popd

endlocal
rem return
exit /b 0

:eof
