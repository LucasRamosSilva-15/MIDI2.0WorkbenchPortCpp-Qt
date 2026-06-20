# Windows MIDI Services Optional Header Include Compile Experiment

## Goal
A versão `v4.8.0` executa um "Include Compile Experiment" puramente isolado para atestar se o compilador C++ (MSVC) local consegue ingerir corretamente os Headers C++/WinRT de MIDI sem disparar dependências fatais no ambiente.

## Architecture
- O projeto adicionou a variável `ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT` (`OFF` por padrão).
- O módulo `WindowsMidiServicesHeaderIncludeProbe` lida exclusivamente com a tentativa cirúrgica de realizar `#include <Windows.Devices.Midi2.h>`.
- O probe só avança com a inclusão se `WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED` sinalizar a presença dos arquivos via CMake (`v4.7.0`).
- Sem chamadas de API reais (Não há C++ runtime side-effects).

## Rules
- **Non-fatal:** Qualquer desvio neste escopo apenas gera notas de "Not Attempted" ou "Failed" nos testes; o aplicativo base permanece ileso.
- Nenhuma varredura global foi instaurada.
- O projeto contínua com seu build *offline* (MIDI 1.0 e UMP parser) protegido.

**Nota (v4.9.0):** A lógica de detecção de diretórios que possibilita este *include experiment* foi aprimorada para detectar e tolerar os nomes corretos do pacote Microsoft (`Microsoft.Windows.Devices.Midi2.h`). O experiment só compila seu payload C++ se o SDK informar que os cabeçalhos (headers) existem (e não apenas o WinMD). Nenhuma API real é chamada e o endpoint listing segue irredutível (bloqueado).

**Nota (v4.10.0):** Esse escopo de Header Include abriu o alicerce para o recém chegado `WindowsMidiServicesTypeReferenceProbe`, o qual referencia estritamente os tipos do SDK na linguagem (Compile Only). Type Reference não significa API Funcional. Endpoint listing segue adiado.

## CMake Tests
```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\Path\To\WindowsMidiServicesSdk"
cmake --build build-wms-sdk-root --config Release
```
