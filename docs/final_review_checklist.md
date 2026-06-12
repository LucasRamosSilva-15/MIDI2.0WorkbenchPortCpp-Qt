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

## Release
- [ ] dist/MidiUmpAnalyzer-v2.21.0-windows-x64.zip generated
- [ ] dist/MidiUmpAnalyzer-v2.21.0-windows-x64-rtmidi.zip generated
