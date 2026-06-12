# Windows MIDI Services SDK Build Experiment

## Goal
A versão v4.3.0 introduz e testa uma trilha de build totalmente opcional focada na futura integração com o SDK oficial da Microsoft para o Windows MIDI Services. O objetivo atual é estruturar o *Pipeline C++* via CMake para recepcionar de forma limpa os componentes `<winrt>` quando decidirmos ativá-los.

## Current status
- **No real endpoint listing yet.** (Ainda não rastreamos portas com o SDK).
- **No real UMP capture yet.** (Nenhum pacote físico cruzou a porta serial).
- **No UI integration yet.** (Sem comboBox visual selecionável).
- **The build experiment is disabled by default.** (Sua compilação diária do TCC não tentará compilar esse ambiente).

## Required external components for future work
Para que as ramificações de Kernel (v4.4.0 e afins) operem num futuro real, a máquina hospedeira possivelmente exigirá:
- Windows MIDI Services enabled/available on the system.
- Windows MIDI Services SDK Runtime/Tools installed separately.
- C++/WinRT setup if required pela malha da linguagem.
- Possible `Microsoft.Windows.Devices.Midi2` package configuration injetada localmente.
- Possible vcpkg/CMake integration, depending on official guidance da Microsoft.

## Why not enabled by default
Por que protegemos o TCC desativando esse SDK via `OFF`?
- SDK availability varies by machine (A máquina da sua banca pode não ter os binários Runtime de Kernel instalados).
- CI should remain stable (Nossas Actions do GitHub empacotam o *Core* offline sem precisar baixar gigabytes de bibliotecas visuais).
- TCC-ready builds must not depend on experimental SDK components.
- The fake backend (`FakeUmpInputBackend`) remains the safe, offline, robust fallback.

## Build commands

**Normal (Offline / TCC-Ready):**
```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```

**RtMidi (MIDI 1.0 Live Tracker):**
```powershell
cmake -B build-rtmidi -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_RTMIDI=ON
cmake --build build-rtmidi --config Release
```

**WMS skeleton (Ativa apenas a casca atômica de v4.1.0):**
```powershell
cmake -B build-wms -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON
cmake --build build-wms --config Release
```

**SDK experiment (Força as condutivas e flag do Experimento v4.3.0):**
```powershell
cmake -B build-wms-sdk -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON
cmake --build build-wms-sdk --config Release
```

## Expected result in v4.3.0
- The SDK experiment flag compiles only a safe probe/stub (`WindowsMidiServicesSdkProbe`).
- It does not list endpoints (Retornos e mock continuam vazios).
- It does not capture UMP (Thread serial não inicializada).
- It does not require SDK yet (O build MSVC passará limpo).

## v4.4.0 Detection Report
- A versão `v4.4.0` adicionou o `WindowsMidiServicesSdkDetectionReport` que expele um diagnóstico textual em tempo de compilação atestando as capacidades do projeto.
- O diagnóstico **ainda não** usa SDK real.
- O diagnóstico **ainda não** detecta pacote real, focando na integridade da *Flag*.

## Future work
- **v4.6.0**: user-provided SDK root variable research.
- **v4.7.0**: optional header detection with find_path.
- **v4.8.0**: endpoint listing prototype if headers are detected.

## Relationship to optional package detection
- O *build experiment* atual atesta a tolerância nativa à *flag* de compilação.
- A *package detection* futura deverá testar a integridade dos cabeçalhos do SDK instalados na máquina.
- A versão `v4.5.0` instanciou essa malha de aferição passiva, atestando em relatório que a caça local ainda não está ativa.
