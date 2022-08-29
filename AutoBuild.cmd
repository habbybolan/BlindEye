@echo off
::
:: Build script for "Blind Eye"
::
:: Copyright (C)2021 Scott Henshaw, All rights reserved
:: USAGE:
::   AutoBuild [-debug] [-nopull] [-norelease] [-releaseonly]
::
SETLOCAL ENABLEEXTENSIONS

:: Parse Command Line Options
:getopts
set OPT-DEBUG=true
if /I "%1"=="-h"    goto :usage
if /I "%1"=="--help" goto :usage
if /I "%1"=="--nodebug"     set OPT-DEBUG=false & shift
if /I "%1"=="--nopull"    set OPT-NOPULL=true & shift
if /I "%1"=="--release:no" set OPT-NOREL=true & shift
if /I "%1"=="--release:network" set OPT-NETREL=true & shift
if /I "%1"=="--release:only" set OPT-RELEASE=true & shift
shift
if not "%1" == "" goto getopts
if DEFINED OPT-DEBUG (
    @echo Debug: %OPT-DEBUG%
    @echo NoPull: %OPT-NOPULL%
    @echo NoRelease: %OPT-NOREL%
)
goto :start


:usage
::
:: AutoBuild
@echo USAGE:
@echo   AutoBuild [--nodebug] [--nopull] [--release:no!network!only] [ProjectName]
goto end


:start
:: generate timestamp
call :SetNow now
set TIMESTAMP=%date:~0,3%-%date:~4,2%-%date:~7,2%@%now%
if DEFINED OPT-DEBUG (
    @echo Build started at: %TIMESTAMP%
)


::----------------------------------------------------------
::
:: The environment  -- EDIT THESE
::
:: set PROJECT to your uproject file name without the .uproject extension
::
set COHORT=GD66PG22
set PROJECT=BlindEye

:: GIT:  Edit the line below to set your git repo if you use the clone option
:: edit your URL
set GIT_REPO="https://github.com/vfs-sct/%COHORT%FP-%PROJECT%.git"


::  DONT TOUCH ANYTHING BELOW HERE
::----------------------------------------------------------

:: SRC Folder, a folder to use for a clean build,
:: this gets the full working path from the current working folder
for %%d in (.) do (
    set SRC=%%~fd
)

:: DEST Folder is where the final build goes, after all things not required
set DEST=%SRC%\Build
if not exist %DEST%\ mkdir %DEST%

:: This is where your build goes.
if DEFINED OPT-NETREL goto :netrelease
set RELEASE=%SRC%\Release
goto :makerelease

:netrelease
set RELEASE="\\vfsstorage10\dropbox\GDPGSD\Builds\%COHORT%\%PROJECT%\Builds"

:makerelease
if not exist %RELEASE%\ mkdir %RELEASE%


:: This identifies a log file for every build, what happened is listed
:: if a build doesn't work, find out why, start here.
if not exist AutoBuildLogs\ mkdir AutoBuildLogs
set LOGFILE=%SRC%\AutoBuildLogs\Build-%TIMESTAMP%.log


:: Start Logging
call :SetNow now
@echo %Project% Build Started at %now% >%LOGFILE%
@echo %Project% Build Started at %now%

:: Edit these paths to where they exist on your build computer
set UNITY="C:\Program Files\Unity\Hub\Editor\2021.1.24f1\Editor\Unity.exe"
set UNREAL="C:\Program Files\Epic Games\UE_4.27\Engine"
set UCC=%UNREAL%\Binaries\DotNET\UnrealBuildTool.exe
set UNREAL_PRE=%UNREAL%\Build\BatchFiles\RunUAT

if DEFINED OPT-RELEASE goto :releaseonly

:: MAIN - Start the Build Process Here!
@echo Building from: %SRC%  >>%LOGFILE%
@echo Building to:   %DEST%  >>%LOGFILE%
@echo Release to:    %RELEASE%  >>%LOGFILE%
@echo -------------------------------------------- >>%LOGFILE%
if DEFINED OPT-DEBUG (
    @echo Building from: %SRC%
    @echo Building to:   %DEST%
    @echo Release to:    %RELEASE%
    @echo --------------------------------------------
)

:: Get the Source control setup
:: go get a fresh copy of your repo - main branch
if DEFINED OPT-CLONE (
    git clone --branch develop %GIT_REPO%
) else (
    if NOT DEFINED OPT-NOPULL (
        git switch develop
        git pull %GIT_REPO%
    )
)


:: -------------------------------------------------------
:: Build the project
@echo:
@echo Starting %UCC% at %time:~0,8%:
:: Build the code with UNREAL, uncomment the lines below
%UCC% %PROJECT% -ModuleWithSuffix %PROJECT% 5202 Win64 Development -editorrecompile -canskiplink %SRC%\\%PROJECT%.uproject >>%LOGFILE%

@echo:
@echo Starting %UNREAL_PRE% at %time:~0,8%
call %UNREAL_PRE% BuildCookRun -project=%SRC%\%PROJECT%.uproject -noP4 -platform=Win64 -clientconfig=Shipping -serverconfig=Shipping -cook -allmaps -build -stage -pak -archive -archivedirectory=%DEST% >>%LOGFILE%


:: Build the code with Unity, comment out the unreal build and uncomment this
:: %UNITY% -projectPath %SRC%\%PROJECT% -quit -batchmode -buildWindows64Player %SRC%\%PROJECT%\Build.exe -logFile

:releaseonly

@echo Build complete >>%LOGFILE%
@echo -------------------------------------------- >>%LOGFILE%
@echo: >>%LOGFILE%
if DEFINED OPT-DEBUG (
    @echo Build complete
    @echo --------------------------------------------
    @echo:
)

:: Generate the release
@echo Generating release  >>%LOGFILE%
if DEFINED OPT-DEBUG @echo Generating release

:: Clean the oldest folders
if DEFINED OPT-DEBUG @echo Cleaning %RELEASE%\oldest
if exist %RELEASE%\oldest rmdir /S /Q %RELEASE%\oldest >>%LOGFILE%
if DEFINED OPT-DEBUG @echo Moving %RELEASE%\yesterday build to oldest
if exist %RELEASE%\yesterday ( move /Y %RELEASE%\yesterday %RELEASE%\oldest >>%LOGFILE% )
if DEFINED OPT-DEBUG @echo Moving %RELEASE%\today to yesterday
if exist %RELEASE%\today     ( move /Y %RELEASE%\today %RELEASE%\yesterday >>%LOGFILE% )

:: Migrate the build to the release folder
if DEFINED OPT-DEBUG @echo Copying Test build to today's build
xcopy /S /I /Q /Y /F %DEST%\WindowsNoEditor %RELEASE%\today >>%LOGFILE%

if DEFINED OPT-DEBUG @echo Test build is here:    %TEST%
if DEFINED OPT-DEBUG @echo Release build is here: %RELEASE%
:: This is where we would create a zip of today's build
goto end


:SetNow
    set %~1=%time:~0,2%.%time:~3,2%.%time:~6,2%
EXIT /B 0

:end
:: Unset all the temp env variables we used
ENDLOCAL
