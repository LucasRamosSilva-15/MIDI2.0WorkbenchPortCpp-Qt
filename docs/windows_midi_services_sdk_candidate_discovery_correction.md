# Windows MIDI Services SDK Candidate Discovery Correction

## Goal

Explicar que v4.9.0 corrige a descoberta de candidatos do SDK para procurar nomes reais mais prováveis do pacote `Microsoft.Windows.Devices.Midi2`.

## Why this was needed

Versões anteriores procuravam principalmente `Windows.Devices.Midi2.h`, mas instalações reais via pacotes como `vcpkg` contêm `Microsoft.Windows.Devices.Midi2.winmd` e/ou `Microsoft.Windows.Devices.Midi2.h` ou sob pastas `winrt/`.

## What is detected

* Header candidates.
* WinMD metadata candidates.

## What is not done

* No real API calls.
* No endpoint listing.
* No UMP capture.
* No mandatory SDK dependency.
* No UI integration.

## Safe search rules

* Only inside `WINDOWS_MIDI_SERVICES_SDK_ROOT`.
* `NO_DEFAULT_PATH`.
* Non-fatal if not found.
* No global scan.
* No registry.
* No PowerShell.
* No vcpkg/NuGet invocation.

## Build command

Usar somente a build experimental existente:

```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\vcpkg\installed\x64-windows"
cmake --build build-wms-sdk-root --config Release
```

## Expected result

* If WinMD is found: report `WinMD Candidate Detected: Yes`.
* If header is found: report `Header Candidate Detected: Yes`.
* If neither is found: build still passes.
* Endpoint listing remains unavailable.

## Future work

* **v4.10.0**: optional WinMD/header compile strategy research.
* **v4.11.0**: isolated API type reference compile experiment.
* **v4.12.0**: endpoint listing prototype only if compile strategy succeeds.
