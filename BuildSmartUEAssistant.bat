@echo off
setlocal

REM Auto-detect Unreal Engine path (edit if different)
set UE_ENGINE_DIR=D:\Ue\UE\UE_5.6\Engine

REM Project path
set PROJ_DIR=%~dp0
set UPROJECT=%PROJ_DIR%MySmartUEAssistant.uproject

REM Target and config
set TARGET=MySmartUEAssistantEditor
set PLATFORM=Win64
set CONFIG=Development

echo Building %TARGET% %PLATFORM% %CONFIG% for project: %UPROJECT%
"%UE_ENGINE_DIR%\Build\BatchFiles\Build.bat" %TARGET% %PLATFORM% %CONFIG% -Project="%UPROJECT%" -WaitMutex -NoHotReload
set ERR=%ERRORLEVEL%

if %ERR% NEQ 0 (
  echo Build failed with exit code %ERR%.
  exit /b %ERR%
) else (
  echo Build succeeded.
)

endlocal