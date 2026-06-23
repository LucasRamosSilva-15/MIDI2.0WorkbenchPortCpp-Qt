# Final Review Checklist

## Build
- [ ] Standard build compiles
- [ ] RtMidi build compiles
- [ ] Release package generated
- [ ] RtMidi release package generated

## Automated tests
- [ ] tests/run_tests.ps1 passes
- [ ] UmpParser tests pass
- [ ] Midi1LiveDecoder tests pass
- [ ] Midi1ToUmpPreviewConverter tests pass
- [ ] FakeUmpInputBackend tests pass

## Offline UMP Analyzer
- [ ] Opens sample files
- [ ] Parses UMP samples
- [ ] Handles comments
- [ ] Handles invalid input
- [ ] Filter works
- [ ] Export CSV works
- [ ] Export TXT works

## Live MIDI Monitor
- [ ] RtMidi build lists ports
- [ ] Can open MIDI input port
- [ ] Can close MIDI input port
- [ ] Receives MIDI 1.0 bytes
- [ ] Decodes Note On/Off
- [ ] Decodes Control Change
- [ ] Filters work
- [ ] Pause/Resume works
- [ ] Live statistics update
- [ ] UMP Preview generates MT 0x2 words
- [ ] Live UMP Preview Table updates
- [ ] Live TXT/CSV export works

## Session Recording
- [ ] Start recording works
- [ ] Stop recording works
- [ ] Clear session works
- [ ] Session records even when visual monitor is paused
- [ ] Session records independent from visual filters
- [ ] Session TXT export works
- [ ] Session CSV export works
- [ ] Session Summary Report works

## TCC Documentation
- [ ] TCC demo guide exists
- [ ] Screenshots guide exists
- [ ] Demo script exists
- [ ] Live MIDI to UMP explanation exists
- [ ] Known limitations exists
- [ ] Test plan exists
- [ ] Release summary exists

## Limitations clearly documented
- [ ] RtMidi receives MIDI 1.0 bytes, not native UMP
- [ ] UMP Preview uses MT 0x2
- [ ] MIDI 2.0 Channel Voice MT 0x4 is not implemented
- [ ] MIDI-CI is not implemented
- [ ] Property Exchange is not implemented
- [ ] Profiles are not implemented
- [ ] Offline UmpParser lacks unit test for MT 0x1 System Real-Time messages

## Experimental UMP Backend Review
- [ ] Verificar se a documentação deixa claro que Fake UMP não é captura real.
- [ ] Verificar se prints/evidências foram gerados.
- [ ] Verificar se testes passaram.
- [ ] Verificar se exports funcionam.
- [ ] Verificar se About/Help não promete UMP real.

## TCC final demo readiness
- [ ] Documentação pronta e revisada
- [ ] Demo guide / script prático redigidos
- [ ] Perguntas de arguição da banca testadas
- [ ] Screenshots de TDD definidos
- [ ] Exports WYSIWYG testados limpos em planilhas
- [ ] Limitações e mock da API explícitos no About
- [ ] Bateria de testes automatizados passou 100%
- [ ] Pacotes distributivos zips gerados e indexados

## v4.0.0 feasibility review
- [ ] Confirmar que nenhum código de terceiros C++/dependência SDK cruzou os portões do Main.
- [ ] Assegurar a compilação fluida sem pacotes bizarros na aba normal e RtMidi.
- [ ] Confirmar que a matriz de fallback v3.x Fake Backend continua preservada e testada.

## v4.2.0 feasibility review
- [ ] README atualizado e coerente com v4.2.0.
- [ ] WindowsMidiServicesBackend ainda não promete captura real.
- [ ] ENABLE_WINDOWS_MIDI_SERVICES=ON compila skeleton sem SDK.
- [ ] CI roda em main e v4-experimental.
- [ ] Release por tag v* preservado.

## v4.3.0 SDK experiment review
- [ ] ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT is OFF by default.
- [ ] Normal build passes without SDK.
- [ ] RtMidi build passes without SDK.
- [ ] WMS skeleton build passes without SDK.
- [ ] SDK experiment does not claim endpoint listing.
- [ ] README does not claim real UMP capture.

## v4.4.0 Detection research review
- [ ] Detection report confirms whether SDK experiment compile flag is enabled.
- [ ] Detection report must not claim real endpoint listing.
- [ ] Detection report must not claim real UMP capture.
- [ ] Normal build remains SDK-free.
- [ ] SDK experiment remains optional.

## v4.5.0 Optional package detection review
- [ ] Windows MIDI Services tests are separated from parser tests.
- [ ] Optional package/header detection fields do not claim real detection.
- [ ] Normal build remains SDK-free.
- [ ] SDK experiment remains optional.
- [ ] README accurately states no real endpoint listing or UMP capture yet.

## v4.6.0 User-provided SDK root research review
- [ ] `WINDOWS_MIDI_SERVICES_SDK_ROOT` is optional.
- [ ] Missing SDK root is non-fatal.
- [ ] SDK root does not imply real header detection.
- [ ] SDK root does not imply endpoint listing.
- [ ] SDK root does not imply UMP capture.
- [ ] Normal build remains SDK-free.

## v4.7.0 Optional header detection review
- [ ] Header detection runs only when `WINDOWS_MIDI_SERVICES_SDK_ROOT` is provided.
- [ ] Header detection uses `NO_DEFAULT_PATH`.
- [ ] Missing headers are non-fatal.
- [ ] Detected headers are not consumed by C++ yet.
- [ ] `realSdkHeadersUsed` remains false.
- [ ] No endpoint listing or UMP capture is claimed.
- [ ] Normal build remains SDK-free.

## v4.8.0 Header include experiment review
- [ ] `ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT` is OFF by default.
- [ ] The isolated probe only attempts `#include` if detected.
- [ ] No real Windows MIDI APIs are invoked.
- [ ] Endpoints are not listed. UMP capture is not attempted.
- [ ] Non-experimental compilation tracks are unaffected.

## v4.9.0 SDK candidate discovery review
- [ ] `Microsoft.Windows.Devices.Midi2` header names are prioritized.
- [ ] `Microsoft.Windows.Devices.Midi2` WinMD names are prioritized.
- [ ] Header detection does not imply Endpoint Listing.
- [ ] WinMD detection does not imply Endpoint Listing.
- [ ] Real API calls are still strictly forbidden.

## v4.10.0 API type reference compile experiment review
- [ ] `ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT` is OFF by default.
- [ ] Compiles `WindowsMidiServicesTypeReferenceProbe` only if requested and Headers exist.
- [ ] Does not enumerate Windows SDK Endpoints.
- [ ] Does not instantiate COM Apartments.
- [ ] Normal builds continue completely detached from Microsoft APIs.

## v4.11.0 C++/WinRT projection alignment review
- [ ] Safe mode enabled. `TypeReferenceProbe` refrains from `#include` by default.
- [ ] Explicit flag (`ENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT`) needed to attempt real headers.
- [ ] Endpoint Listing strictly disabled.
- [ ] MSVC / Qt / RtMidi builds continue unaltered and free of MS SDK.

## Release
- [ ] dist/MidiUmpAnalyzer-v2.21.0-windows-x64.zip generated
- [ ] dist/MidiUmpAnalyzer-v2.21.0-windows-x64-rtmidi.zip generated
