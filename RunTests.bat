@echo off
rem Builds and runs the unit tests without starting the tray app.
rem Usage: RunTests [Debug|Release] [x64|Win32]   (default: Debug x64)
rem Works from any prompt; MSBuild is located with vswhere.

setlocal
set CONFIG=%~1
if "%CONFIG%"=="" set CONFIG=Debug
set PLATFORM=%~2
if "%PLATFORM%"=="" set PLATFORM=x64

set VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
if not exist "%VSWHERE%" (
    echo vswhere.exe not found. Install Visual Studio with the C++ workload.
    exit /b 1
)

set MSBUILD=
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBUILD=%%i"
if not defined MSBUILD (
    echo MSBuild not found.
    exit /b 1
)

rem The project targets toolset v145 (VS 2026); use v143 with VS 2022
set TOOLSET=v145
for /f "usebackq tokens=1 delims=." %%v in (`"%VSWHERE%" -latest -property installationVersion`) do if %%v LSS 18 set TOOLSET=v143

"%MSBUILD%" "%~dp0Tests\ClipTxtTests.vcxproj" /nologo /v:m /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /p:PlatformToolset=%TOOLSET%
if errorlevel 1 (
    echo.
    echo BUILD FAILED
    exit /b 1
)

echo.
"%~dp0%PLATFORM%\%CONFIG%\ClipTxtTests.exe"
if errorlevel 1 (
    echo.
    echo TESTS FAILED
    exit /b 1
)
echo.
echo TESTS PASSED
