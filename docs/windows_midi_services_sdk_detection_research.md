# Windows MIDI Services SDK Detection Research

## Goal
A versão v4.4.0 dá o próximo passo no estudo do Kernel do Windows melhorando a capacidade de diagnóstico e extração de relatório da nossa malha C++, sem atrelar agressivamente as exigências do pacote real.

## What detection means in v4.4.0
Nesta versão, "Detecção" atesta a saúde das nossas Flags no *Pipeline*:
- Compile-time flag detection.
- Documentation-level readiness.
- No real SDK probing yet (Ainda não há injestão do header `<winrt/Windows.Devices.Midi2.h>`).
- No endpoint listing.
- No UMP capture.

## Why runtime detection is deferred
Por que decidimos evitar buscar as pastas de instalação ou os registros do Windows MIDI Services em disco em tempo de execução?
- Avoid registry/disk assumptions (Evitar falso-positivos buscando pastas locais).
- Avoid depending on SDK installation paths.
- Avoid breaking CI (Nuvem Microsoft).
- Avoid making TCC builds depend on experimental SDK.

## Detection report fields
O `WindowsMidiServicesSdkProbe` C++ nativo agora expele uma *Struct* densa com o raio-x exato do ambiente de compilação C++ local:
- `experimentCompileFlagEnabled`: Aponta se o CMake repassou a diretiva de experimentação pro MSVC.
- `realSdkHeadersUsed`: Mantido em `false` até a v4.5+.
- `realEndpointListingAvailable`: Mantido em `false` até a porta real estar exposta na tabela.
- `realUmpCaptureAvailable`: Mantido em `false` até ouvirmos fisicamente bytes em RAM.
- `notes`: Diagnóstico imutável de compilação contendo restrições.
- `nextSteps`: Guia recomendatório para o próximo passo (instalar WMS/WinRT).

## Build commands
Normal:
```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```

SDK experiment:
```powershell
cmake -B build-wms-sdk -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON
cmake --build build-wms-sdk --config Release
```

## Expected result
- Normal build reports experiment disabled.
- SDK experiment build reports flag enabled.
- Both report no real endpoint listing and no real UMP capture.

## Future work
- **v4.6.0**: user-provided SDK root variable research.
- **v4.7.0**: optional header detection with find_path.
- **v4.8.0**: endpoint listing prototype only when headers are detected.

## Optional Package Detection - v4.5.0
- O relatório em `v4.5.0` ganhou campos específicos para *Package/Header*.
- Todos os campos são preparatórios (nascem configurados para `false`).
- A detecção invasiva foi deliberadamente adiada em favor da estabilidade.

## Optional Header Detection with SDK Root - v4.7.0
- Com a injeção do caminho fixo em `v4.6.0`, a `v4.7.0` agora ativa os campos `optionalHeaderDetectionAttempted` e `optionalHeadersDetected`.
- Se o diretório alvo conter assinaturas de Headers Microsoft (Ex: `winrt/Windows.Devices.Midi2.h`), os campos são comutados confirmando a aptidão do C++ de extrair aquela API.
- Reitera-se que `realSdkHeadersUsed` permanece irredutivelmente travado em `false`.

**Nota (v4.9.0):** A estrutura de Report (`WindowsMidiServicesSdkProbe`) foi drasticamente enrijecida com os campos `sdkCandidateDiscoveryAttempted`, `headerCandidateDetected` e `winmdCandidateDetected`, propiciando um diagnóstico altamente refinado sobre as pastas fornecidas pelo desenvolvedor, sem contudo vazar para APIs reais.
