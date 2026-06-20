# Windows MIDI Services API Type Reference Compile Experiment

## Goal

Explicar que `v4.10.0` testa se um arquivo C++ isolado consegue referenciar o SDK/header detectado em modo compile-only.

## What changed in v4.10.0

* nova flag `ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT`.
* novo `WindowsMidiServicesTypeReferenceProbe`.
* experimento OFF por padrão.
* só compila quando header candidato foi detectado.
* não chama runtime API.

## Why this is not endpoint listing yet

* endpoint listing exige chamadas reais ao SDK/runtime.
* esta versão só valida compile-time compatibility.
* nenhuma sessão é criada.
* nenhum endpoint é enumerado.
* nenhum UMP é capturado.

## Build command

Usar somente a build experimental existente:

```powershell
cmake -B build-wms-sdk-root -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON -DENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT=ON -DENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT=ON -DWINDOWS_MIDI_SERVICES_SDK_ROOT="C:\vcpkg\installed\x64-windows"

cmake --build build-wms-sdk-root --config Release
```

## Expected result

* Se header candidato existe:
  * type reference requested: Yes
  * type reference compiled: Yes, se o header for compatível
* Se header candidato não existe:
  * type reference requested: Yes
  * type reference compiled: No
  * build continua non-fatal
* Nos dois casos:
  * endpoint listing permanece No
  * UMP capture permanece No

## What is still not implemented

* endpoint listing real;
* UMP capture real;
* backend selector UI;
* MIDI-CI;
* Property Exchange;
* runtime API call.

## Future work

* **v4.11.0**: isolated runtime initialization research, if safe.
* **v4.12.0**: endpoint listing prototype only after initialization strategy is understood.
* **v4.13.0**: backend selector UI only after endpoint listing stabilizes.
