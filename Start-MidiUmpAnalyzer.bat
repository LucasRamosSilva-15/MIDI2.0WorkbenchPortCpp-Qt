@echo off
title MIDI 2.0 Workbench Port Launcher

set EXE_FILE=MidiUmpAnalyzer.exe

if exist "%~dp0%EXE_FILE%" (
    echo Iniciando MIDI UMP Analyzer...
    start "" "%~dp0%EXE_FILE%"
) else (
    echo [ERRO] O executavel %EXE_FILE% nao foi encontrado nesta pasta!
    echo Por favor, certifique-se de extrair todos os arquivos do arquivo .zip antes de executar.
    pause
)
