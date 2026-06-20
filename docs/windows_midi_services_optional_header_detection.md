# Windows MIDI Services Optional Header Detection with SDK Root

## Goal
Explicar que a versão `v4.7.0` faz a primeira detecção opcional de headers candidatos usando estritamente o caminho manual fornecido em `WINDOWS_MIDI_SERVICES_SDK_ROOT`.

## What changed in v4.7.0
- O comando nativo `find_path` é acionado apenas quando um SDK root é ativamente informado pelo Desenvolvedor.
- A flag `NO_DEFAULT_PATH` impede severamente a busca autônoma no Sistema Operacional (Windows).
- O resultado continua sendo estritamente **non-fatal**.
- Headers detectados ainda **não** são incluídos nem consumidos nativamente.
- Endpoint listing real continua bloqueado.

## Why this is safe
- Não há varredura agressiva (Bloqueio via `NO_DEFAULT_PATH`);
- Não há varredura no Registry;
- Não há uso injetado de PowerShell;
- Não exige o pacote Microsoft;
- Nenhuma rotina de `#include` foi colocada no App base;
- Não quebra o Continuous Integration (CI).

## Build command
Usar somente a build experimental base testada em isolamento:
```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\Path\To\WindowsMidiServicesSdk"
cmake --build build-wms-sdk-root --config Release
```

## Expected result
- Se os headers *forem* encontrados: o diagnóstico acusará `Candidate Headers Detected: Yes`.
- Se os headers *não* forem encontrados: o diagnóstico acusará `Candidate Headers Detected: No`.
- Nos dois casos absolutos: a *tag* `Real Headers Used` confirmará `No`.

**Nota (v4.9.0):** A busca por candidatos foi estendida para considerar também os metadados `Microsoft.Windows.Devices.Midi2.winmd` de forma dissociada. A detecção passiva ainda é mantida em ambiente estritamente não-hostil (Non-fatal e restrita a raiz).

**Nota (v4.10.0):** Validou-se estruturalmente que detectar Headers é uma etapa prévia e mandatória antes do referenciamento de tipos de API nativas. Detectar o Header C++ contudo, não autoriza o listing de interfaces nem captura UMP real.

## What is still not implemented
- Endpoint listing real;
- UMP capture real;
- C++/WinRT integration;
- `Microsoft.Windows.Devices.Midi2` package;
- Backend selector UI.

## Future work
- **v4.8.0**: optional header include compile experiment.
- **v4.9.0**: endpoint listing prototype only if compile experiment succeeds.
- **v4.10.0**: backend selector UI only after endpoint listing stabilizes.
