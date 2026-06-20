# Release Summary v4.x

A árvore de versões `v4.x` tem como manifesto principal a migração e exploração orgânica rumo à captação em altíssima resolução de protocolos reais UMP 2.0 pelo Sistema Operacional primário (Windows).

## v4.10.0 - Windows MIDI Services isolated API type reference compile experiment
- Added isolated `WindowsMidiServicesTypeReferenceProbe` to stress test C++ references without executing them.
- Kept UI untouched and stable. Endpoints are strictly locked away.
- Proved CMake's capacity to build upon Windows Native architectures without collapsing standard SDK-Free packages.

## v4.9.0 - Windows MIDI Services SDK candidate discovery correction
- Corrected candidate discovery to detect `Microsoft.Windows.Devices.Midi2` headers and `.winmd` files.
- Restructured `WindowsMidiServicesSdkProbe` to format accurate Header vs WinMD metrics.
- Enforced strict stability checks preventing runtime API invocations despite proper path routing.

## v4.8.0 - Windows MIDI Services optional header include compile experiment
- Introduced `ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT`.
- Added isolated `WindowsMidiServicesHeaderIncludeProbe`.
- Validated conditional compile-time header inclusion without real API usage.
- Tests isolated. No runtime API calls or UI bindings.

## v4.7.0 - Windows MIDI Services optional header detection with SDK root
- Added optional header detection using user-provided SDK root.
- Used restricted `find_path` with `NO_DEFAULT_PATH`.
- Added header detection status to SDK diagnostic report.
- Kept detection non-fatal and research-only.
- No real header consumption, endpoint listing or UMP capture yet.

## v4.6.0 - Windows MIDI Services user-provided SDK root research
- Added `WINDOWS_MIDI_SERVICES_SDK_ROOT` CMake cache variable.
- Added SDK root status fields to `WindowsMidiServicesSdkDetectionReport`.
- Added documentation for manual SDK root research.
- Kept SDK root non-fatal and research-only.
- No real header detection, endpoint listing or UMP capture yet.

## v4.5.0 - Windows MIDI Services optional package detection experiment
- Added optional package/header detection readiness fields.
- Added documentation for future package/header detection.
- Created WindowsMidiServicesBackendTests test file.
- Kept SDK detection non-invasive.
- No real endpoint listing or UMP capture yet.

## v4.4.0 - Windows MIDI Services SDK detection research
- Added WindowsMidiServicesSdkDetectionReport.
- Added formatted SDK detection report.
- Added detection research documentation.
- Preserved normal builds without SDK dependency.
- No real endpoint listing or UMP capture yet.

## v4.3.0 - Windows MIDI Services SDK build experiment
- Added optional SDK experiment build flag.
- Added SDK build experiment documentation.
- Preserved normal and RtMidi builds without SDK dependency.
- No real endpoint listing or UMP capture yet.

## v4.2.0 - Windows MIDI Services endpoint listing research
- Added endpoint listing research documentation.
- Updated README to reflect current project state.
- Prepared WindowsMidiServicesBackend skeleton with endpoint query stub.
- Real endpoint listing and UMP capture are not implemented yet.
- No Windows MIDI Services SDK dependency added.

## v4.1.0 - Windows MIDI Services backend skeleton
- Added WindowsMidiServicesBackend skeleton.
- Added ENABLE_WINDOWS_MIDI_SERVICES CMake option, OFF by default.
- Backend does not list endpoints or capture real UMP yet.
- No Windows MIDI Services SDK dependency added.
- CI workflow updated to run on v4-experimental branch.

## v4.0.0 - Native UMP backend feasibility research
- **Documentation-only feasibility release.** (Não adicionou nem encerrou C++ novo, apenas relatórios prospectivos).
- Investigou massivamente o cenário futuro sobre `Windows MIDI Services`, `ALSA UMP`, `libremidi` e `JUCE` enquanto possíveis motores paralelos de escuta de porta.
- Estabeleceu e elegeu o **Windows MIDI Services** como o candidato primário (Target #1) das próximas inserções de hardware na planta do Windows.
- O software mantém-se cego a capturas de hardware real (Nenhum UMP real backend fora criado).
- Ratifica que o bloco da **v3.x** e seu `FakeUmpInputBackend` permanecem intocáveis, servindo como o pilar estático, sólido e incontestável para fins de demonstração (TCC-ready baseline).
