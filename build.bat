@echo off
rem Построитель для MounRiver Studio 1.92: MRS вызывает этот файл как "make" для Build, Clean и Build Targets.
rem Автор: Дмитрий (GitHub: mamkincoderr, https://github.com/mamkincoderr)
rem Telegram: https://t.me/oDeXteRo
rem
rem Цель берётся из аргументов (Eclipse добавляет свои ключи вроде -j24, они пропускаются):
rem   all    сборка обоих ядер и склейки
rem   clean  очистка obj
rem   flash  сборка и прошивка obj\H417_Merge.hex через WCH-Link
rem   reset  сброс и пуск чипа
rem   erase  стирание всей флеш
rem   info   CHIPID, объём флеша, UID
setlocal EnableExtensions

set "TARGET=all"
for %%A in (%*) do call :scan "%%~A"
goto :scanned

:scan
rem Аргумент может прийти одной строкой вида "-j24 all": разбираем по словам
for %%B in (%~1) do call :word "%%~B"
exit /b 0

:word
set "W=%~1"
if not "%W:~0,1%"=="-" set "TARGET=%W%"
exit /b 0

:scanned
set "CMD=%TARGET%"
if /i "%CMD%"=="all" set "CMD=build"

rem CMake и Ninja: MRS их не содержит, а PATH, унаследованный средой, мог устареть.
for %%D in ("%ProgramFiles%\CMake\bin" "%ProgramW6432%\CMake\bin" "%LOCALAPPDATA%\Programs\CMake\bin" "%LOCALAPPDATA%\Microsoft\WinGet\Links" "%ProgramFiles%\Ninja") do (
    if exist "%%~D\" set "PATH=%PATH%;%%~D"
)
for /d %%P in ("%LOCALAPPDATA%\Microsoft\WinGet\Packages\Ninja-build.Ninja*") do set "PATH=%PATH%;%%~P"

echo [H417] target=%CMD%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\run.ps1" %CMD%
exit /b %ERRORLEVEL%
