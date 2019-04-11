@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" > nul
if not exist ..\bin mkdir ..\bin

set CommonCompilerFlags=-nologo -W4 -Od -wd4996
cl %CommonCompilerFlags% -Fe..\bin\main.exe main.cpp
