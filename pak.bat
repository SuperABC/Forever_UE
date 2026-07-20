@echo off
set ENGINE=C:\Workspace\UE5\Engine\UE_5.7
set PROJECT=C:\Workspace\UE5\Projects\Forever_UE\Forever.uproject
set ARCHIVE=C:\Workspace\UE5\Projects\Forever_UE\Saved\Archive
set BINDIR=C:\Workspace\UE5\Projects\Forever_UE\Binaries\Win64

if exist "%ARCHIVE%" rmdir /s /q "%ARCHIVE%"
if exist "%~dp0Saved\Cooked\Windows" rmdir /s /q "%~dp0Saved\Cooked\Windows"

del /f /q "%BINDIR%\Forever.exe"    2>nul
del /f /q "%BINDIR%\Forever.target" 2>nul
del /f /q "%BINDIR%\Forever.lib"    2>nul
del /f /q "%BINDIR%\Forever.exp"    2>nul

call "%ENGINE%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
  -project="%PROJECT%" -noP4 -platform=Win64 -clientconfig=Development ^
  -build -cook -allmaps -stage -pak -archive ^
  -archivedirectory="%ARCHIVE%"
if errorlevel 1 goto :fail

robocopy "%~dp0Source\Resources" "%ARCHIVE%\Windows\Forever\Source\Resources" /E /IS /IT
if errorlevel 8 goto :fail

echo Done.
goto :end
:fail
echo Failed.
exit /b 1
:end
