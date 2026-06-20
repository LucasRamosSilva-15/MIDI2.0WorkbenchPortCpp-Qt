# Windows MIDI Services User-Provided SDK Root Research

## Goal
Explicar que a versão `v4.6.0` introduz uma variável CMake opcional permitindo que o usuário informe manualmente um possível SDK root.

## CMake variable
`WINDOWS_MIDI_SERVICES_SDK_ROOT`

## Why manual root first
- Evita varredura agressiva no disco;
- Evita acesso intrusivo ao Registro do Windows;
- Evita dependência de uma instalação específica fixa da MS;
- Evita quebrar o Continuous Integration (CI);
- Mantém a build didática de TCC absolutamente estável.

## What happens when empty
- A build continua normal;
- Nenhum SDK é usado;
- O relatório formatado confirma expressamente que o SDK root não foi configurado.

## What happens when provided
- O CMake registra com precisão o caminho.
- O CMake afere passivamente (`if(EXISTS ...)`) se o caminho existe.
- O resultado da aferição é **non-fatal**. Falhas de diretório apenas engatilham Warnings, não quebram o fluxo de máquina;
- Nenhum Header de C++ é invocado;
- Nenhuma Biblioteca de C++ é Linkada;
- Nenhum endpoint é listado no Windows.

## Build examples

**Normal:**
```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```

**SDK experiment without root:**
```powershell
cmake -B build-wms-sdk -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON
cmake --build build-wms-sdk --config Release
```

**SDK experiment with user-provided root:**
```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\Path\To\WindowsMidiServicesSdk"
cmake --build build-wms-sdk-root --config Release
```

## Expected result in v4.6.0
- Root path is accepted for research only;
- No real SDK headers are consumed;
- No endpoint listing;
- No UMP capture.

## Optional Header Detection - v4.7.0
- A partir da `v4.7.0`, o caminho manual é repassado ao `find_path` no CMake.
- A restrição `NO_DEFAULT_PATH` garante que somente essa raiz manual é varrida.
- A condicionalidade do processo refuta crashes sistêmicos caso o diretório fique obsoleto ou falhe (Non-fatal).
- Nativos identificados na raiz ainda não são `#included` pela compilação.

**Nota (v4.9.0):** Para amparar a extração robusta via `vcpkg`, a malha de detecção aprofundou a busca passiva isolando a checagem de Metadados (`Microsoft.Windows.Devices.Midi2.winmd`) e C++ Headers (`Microsoft.Windows.Devices.Midi2.h`). WinMD detectado não é o mesmo que Header compilável.

## Future work
- **v4.7.0**: optional header detection with `find_path` using SDK root.
- **v4.8.0**: endpoint listing prototype only if headers are detected.
- **v4.9.0**: backend selector UI only after endpoint listing stabilizes.
