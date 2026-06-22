# v4 Native UMP Backend Roadmap

## v4.0.0
Native UMP backend feasibility research.
(Release atual estritamente bibliográfica. Apenas arquiteturas documentais e notas de transição, sem nenhuma alteração funcional nas matrizes *Fake* e *Live* do TCC.)

## v4.1.0
Windows MIDI Services backend skeleton.
- Cimento arquitetural estruturado em `.h` e `.cpp` na malha de compilação protegido pela CMake flag `ENABLE_WINDOWS_MIDI_SERVICES`.
- Nenhum link efetivo com SDK Microsoft executado nesta etapa; retornando coleções de endpoints vazias (mock de segurança).

## v4.2.0 - Windows MIDI Services endpoint listing research
- Endpoint listing researched and documented na base textual `windows_midi_services_feasibility.md`.
- Esqueleto preparado com um *endpoint query stub* privado puramente virtual/isolado.
- Listagem nativa e real no kernel postergada por segurança.
- UI backend selector postergado.

## v4.3.0 - Windows MIDI Services SDK build experiment
- Added optional SDK experiment flag (`ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT`).
- Added documentation for SDK build path.
- No SDK dependency enabled by default.
- No endpoint listing or capture yet.

## v4.4.0 - Windows MIDI Services SDK detection research
- Added SDK detection report.
- Added compile flag awareness.
- No real SDK integration yet.
- No endpoint listing or capture yet.

## v4.5.0 - Windows MIDI Services optional package detection experiment
- Separated Windows MIDI Services tests into their own test file (`WindowsMidiServicesBackendTests.cpp`).
- Added package/header detection readiness fields in `WindowsMidiServicesSdkProbe`.
- Documented optional package detection path.
- No real package/header probing yet.
- No endpoint listing or UMP capture yet.

## v4.6.0 - Windows MIDI Services user-provided SDK root research
- Added optional CMake variable `WINDOWS_MIDI_SERVICES_SDK_ROOT`.
- Added SDK root readiness fields to the detection report.
- Accepted user-provided SDK root for research only.
- No header detection yet.
- No endpoint listing or UMP capture yet.

## v4.7.0 - Windows MIDI Services optional header detection with SDK root
- Added optional `find_path`-based header detection using user-provided SDK root.
- Restricted search to `WINDOWS_MIDI_SERVICES_SDK_ROOT` with `NO_DEFAULT_PATH`.
- Kept detection non-fatal.
- Did not include or consume headers.
- No endpoint listing or UMP capture yet.

## v4.8.0 - Windows MIDI Services optional header include compile experiment
- Added isolated include experiment flag `ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT`.
- Created isolated `WindowsMidiServicesHeaderIncludeProbe` logic.
- Included headers conditionally strictly without side-effects or API calls.
- Preserved stability without real UMP capture or Endpoint mapping.

## v4.9.0 - Windows MIDI Services SDK candidate discovery correction
- Refined discovery to search for `Microsoft.Windows.Devices.Midi2.winmd` and header files.
- Separated Header detection from WinMD detection.
- Expanded detection capabilities without invoking real API or mapping Endpoints.

## v4.10.0 - Windows MIDI Services isolated API type reference compile experiment
- Optional compile strategy validation referencing SDK structures directly in C++.
- Requires verified Candidate SDK path (v4.9.0).
- Fully abstracted, runtime safe, zero endpoint listings.

## v4.11.0 - Windows MIDI Services C++/WinRT projection alignment research
- Isolates C++/WinRT version conflicts (`CPPWINRT_VERSION`).
- Provides safe boundaries preventing experimental probes from corrupting the C++ compiler build logic.
- Awaiting resolution of the `winrt/impl` projection architecture.

## v4.12.0
C++/WinRT generated projection strategy research.

## v4.20.0
Isolated runtime initialization research only after projection alignment.

## v4.20.0
Endpoint listing prototype only after initialization strategy succeeds.

## v4.11.0
Experimental UMP receive prototype.
- Cativar pacotes UMP em C++ com MSVC puro.
