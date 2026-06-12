# MidiUmpAnalyzer / MIDI2.0WorkbenchPortCpp-Qt

![CI](https://github.com/LucasRamosSilva-15/MIDI2.0WorkbenchPortCpp-Qt/actions/workflows/ci.yml/badge.svg)
![Release](https://github.com/LucasRamosSilva-15/MIDI2.0WorkbenchPortCpp-Qt/actions/workflows/release.yml/badge.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-blue)
![Qt](https://img.shields.io/badge/Framework-Qt6-green)
**Current version:** v4.2.0

## Descrição curta
Ferramenta em C++/Qt para análise de Universal MIDI Packet, monitoramento MIDI 1.0, UMP Preview MT 0x2 e pesquisa experimental de backend UMP nativo.
Foi criado para validar pacotes MIDI 2.0 (como SysEx8 e Flex Data) gerados por MCUs embarcados (como Raspberry Pi Pico) antes do transporte USB final.

## Estado atual
- **Offline UMP Analyzer**: funcional.
- **Live MIDI Monitor via RtMidi**: opcional, MIDI 1.0 bytes crus.
- **MIDI 1.0 to UMP Preview**: funcional como MT 0x2.
- **Experimental UMP Backend**: funcional com FakeUmpInputBackend.
- **Fake UMP Session Recording/Summary**: funcional.
- **WindowsMidiServicesBackend**: skeleton/research only.
- **Real Windows MIDI Services endpoint listing**: ainda não implementado.
- **Real UMP capture**: ainda não implementado.

## Important limitations
- O componente RtMidi **não é** um backend UMP real.
- O UMP Preview do Live MIDI usa MT 0x2, não realiza conversão completa para propriedades MT 0x4 de alta resolução.
- O `FakeUmpInputBackend` não captura hardware real. Ele injeta arrays hexadecimais in-memory para validação da Interface Gráfica.
- O `WindowsMidiServicesBackend` ainda é um *skeleton* puramente acadêmico sem comunicação com kernel.
- Protocolos MIDI-CI (Property Exchange, Profile Configuration, Protocol Negotiation) não estão implementados ativamente.
- O Windows MIDI Services SDK **não é** uma dependência obrigatória nesta versão, o *build* permanece independente.

## Version lines
- **v1.x:** Offline UMP Analyzer.
- **v2.x:** Live MIDI + UMP Preview.
- **v3.x:** Experimental Fake UMP Backend, recording, exports, summary, TCC-ready.
- **v4.x:** Native UMP backend research, Windows MIDI Services skeleton/endpoint listing research.

## Screenshots
- ![Interface Principal](docs/screenshots/Screenshot3.png)
- ![Filtragem SysEx](docs/screenshots/Screenshot4.png)

## Build
Para compilar localmente na sua máquina Windows utilizando MSVC 2022.

**Build Padrão (Offline / Fake Backend):**
```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```

**Build RtMidi (Live MIDI 1.0):**
```powershell
cmake -B build-rtmidi -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_RTMIDI=ON
cmake --build build-rtmidi --config Release
```

**Build Skeleton Windows MIDI Services:**
*(Nota: a flag ENABLE_WINDOWS_MIDI_SERVICES=ON ainda não ativa SDK real, apenas compila a casca skeleton protegida).*
```powershell
cmake -B build-wms -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64" -DENABLE_WINDOWS_MIDI_SERVICES=ON
cmake --build build-wms --config Release
```

## Tests
Automação unitária nativa em terminal (não requer interface):
```powershell
powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1
```

## Packaging
Zips de lançamento podem ser empacotados com scripts locais acoplados nativamente:
```powershell
powershell -ExecutionPolicy Bypass -File scripts\package_release.ps1 -Version v4.2.0
powershell -ExecutionPolicy Bypass -File scripts\package_release.ps1 -Version v4.2.0 -EnableRtMidi
```

## Docs
Manuais oficiais e cadernos de auditoria dispostos no repositório:
- [docs/tcc_final_demo_script.md](docs/tcc_final_demo_script.md)
- [docs/experimental_ump_backend_demo_guide.md](docs/experimental_ump_backend_demo_guide.md)
- [docs/native_ump_backend_feasibility.md](docs/native_ump_backend_feasibility.md)
- [docs/windows_midi_services_feasibility.md](docs/windows_midi_services_feasibility.md)
- [docs/v4_backend_roadmap.md](docs/v4_backend_roadmap.md)
- [docs/release_summary_v4.md](docs/release_summary_v4.md)
