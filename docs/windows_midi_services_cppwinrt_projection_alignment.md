# Windows MIDI Services C++/WinRT Projection Alignment Research

## Goal

Explicar que v4.11.0 documenta e isola o problema de alinhamento entre C++/WinRT, headers gerados e o pacote Microsoft.Windows.Devices.Midi2.

## Background

A v4.10.0 detectou:

* candidate header found through vcpkg;
* CPPWINRT_VERSION mismatch (MSVC Visual Studio nativo vs Vcpkg projection);
* missing generated impl header, such as `winrt/impl/Microsoft.Windows.Devices.Midi2.2.h`.

## What this means

* Detectar header não basta.
* Incluir header não basta.
* O projeto precisa de uma projeção C++/WinRT consistente.
* O uso correto provavelmente exige toolchain vcpkg e configuração C++/WinRT alinhada de modo coeso.

## What changed in v4.11.0

* adiciona modo de pesquisa de alinhamento C++/WinRT.
* adiciona diagnóstico sobre toolchain.
* impede que o TypeReferenceProbe quebre a build experimental por padrão (Safe Mode).
* separa tentativa real de include em flag explícita (`ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT`).

## Flags

* `ENABLE_WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH`
* `ENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT`

Explicar:

* CPPWINRT_ALIGNMENT_RESEARCH é diagnóstico seguro.
* REAL_CPPWINRT_INCLUDE_ATTEMPT pode quebrar build e não deve ser usado em release/CI.

## Safe build command

Usar somente build-wms-sdk-root:

```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\vcpkg\installed\x64-windows"

cmake --build build-wms-sdk-root --config Release
```

## Dangerous/local diagnostic command

**Atenção:** Esta compilação quebrará caso a toolchain não alinhe os Headers impl do C++/WinRT.

```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH=ON -DENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\vcpkg\installed\x64-windows"

cmake --build build-wms-sdk-root --config Release
```

## Expected result

* Safe mode should compile without triggering the mismatch.
* Real include attempt may fail.
* Failure is useful diagnostic evidence.
* No endpoint listing is implemented.

## What is still not implemented

* endpoint listing real.
* UMP capture real.
* runtime initialization.
* backend selector UI.
* MIDI-CI.
* Property Exchange.

## Future work

* **v4.12.0**: C++/WinRT generated projection strategy research.
* **v4.13.0**: isolated runtime initialization research only after projection alignment.
* **v4.14.0**: endpoint listing prototype only after initialization strategy succeeds.
