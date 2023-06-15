set PROJ_DIR=%~dp0..\..
set BUILD_DIR=build

cd /D %PROJ_DIR%

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

doxygen doc/doxygen.cfg

if "%1"=="nopause" goto ignorepause
pause
:ignorepause
