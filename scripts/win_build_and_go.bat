
@echo off

set EXE_NAME="chunky"

IF NOT EXIST bin mkdir bin
pushd bin

set VC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
if [%VisualStudioVersion%] == [] echo WARNING: VC cold. && call %VC_PATH%"\vcvarsall.bat" x64 >NUL

set COMPILER_FLAGS=-nologo -Fe%EXE_NAME% -fp:fast -EHa- -Gm- -GR- -Oi -WX -W4 -wd4996 -wd4100 -wd4189 -wd4127 -wd4201 -wd4101 -wd4505 -wd4204 -wd4221 -wd4200
set COMPILER_FLAGS=%COMPILER_FLAGS% -MTd -Od -Z7
rem set COMPILER_FLAGS=%COMPILER_FLAGS% -MT -O2 -Z7

set LINKER_FLAGS=-incremental:no -opt:ref

cl %COMPILER_FLAGS% -I "../../include/" -I "../../external/utest.h/" "../../src/lifetime.c" "../../src/entities.c"  "../../src/components.c" "../../test/utest.c" "../../test/utest_lifetime.c" "../../test/utest_components.c" "../../test/utest_entities.c" -link %LINKER_FLAGS%

if [%ERRORLEVEL%] EQU [0] %EXE_NAME%.exe

popd
