# Windows MIDI Services Optional Package Detection Experiment

## Goal
Explicar que a versão `v4.5.0` organiza e estrutura a etapa de detecção opcional de pacote/header para o Windows MIDI Services.

## What is detected in v4.5.0
- O status da *compile flag* C++ local.
- Novos campos estruturais dentro do `WindowsMidiServicesSdkDetectionReport` atestando a inatividade deliberada do MSVC.
- Declaração explícita formatada em texto comprovando que pacotes agressivos e headers locais (`<winrt/...>`) não estão sendo executados nesta versão.

## What is not detected yet
- Microsoft.Windows.Devices.Midi2 package real installation.
- C++/WinRT projection.
- Actual SDK headers localizados no HD do usuário.
- Runtime/Tools installation path.
- Endpoints reais.
- UMP capture.

## Why actual detection is deferred
- Avoid machine-specific assumptions.
- Avoid CI instability no GitHub Actions.
- Avoid mandatory SDK dependency prejudicando compiladores educacionais de TCC.
- Avoid invasive filesystem/registry scanning por scripts indesejados.

## Future detection candidates
- CMake option pointing to SDK root (Ajuste por flag explícita).
- CMake cache variable `WINDOWS_MIDI_SERVICES_SDK_ROOT`.
- Optional `find_path` for known headers (Abordagem silenciosa do CMake).
- Optional `vcpkg` toolchain path documentado caso o usuário ative.
- Documented user-provided path via CLI.
- Nenhuma busca agressiva ou não catalogada.

## Recommended future approach
- **v4.6.0**: User-provided SDK root variable research.
- **v4.7.0**: Optional header detection with `find_path`.
- **v4.8.0**: Endpoint listing prototype only when headers are detected.

## Optional Header Detection - v4.7.0
- O pacote CMake engatilha pela primeira vez um teste de verificação opcional cruzado em ambiente real, tentando parear e atestar a veracidade da pasta injetada pelo usuário através da existência de Headers essenciais.
- Ainda não há pacote real exigido.
- Ainda não há endpoint listing ativo.
