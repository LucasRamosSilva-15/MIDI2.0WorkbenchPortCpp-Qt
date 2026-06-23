# Release Summary v3

A linha `v3.x` surge com o propósito de avançar a infraestrutura construída no ciclo v2 e iniciar a inserção definitiva do projeto na era do UMP real e do MIDI 2.0 (MT 0x4 / MIDI-CI), integrando abstrações voltadas aos drivers de classe de kernel.

- **v3.0.0 - Experimental MIDI 2.0/UMP backend research**: 
  Neste primeiro marco, focamos inteiramente em arquitetura e planejamento. A linha "experimental UMP backend" estuda as abordagens via *Windows MIDI Services*, define as futuras interfaces rigorosas C++ `IumpInputBackend` e levanta notas sobre dependências *WinRT* sem quebrar a consagrada estabilidade do backend RtMidi original consolidado na v2.x.
  O software continua idêntico funcionalmente, mas o terreno da `v3` já traça a via livre para a futura chegada dos bytes de alta-resolução reais.

- **v3.1.0 - Define UMP backend interfaces**:
  - Added UmpRawEvent structure.
- Added IUmpInputBackend interface.
- Added FakeUmpInputBackend for testing without hardware.
- No real UMP backend or parsing changes yet.

### v3.2.0 - Fake UMP backend integration prototype
- Added "Experimental UMP Backend" tab in UI.
- Integrated FakeUmpInputBackend to validate polling pipeline.
- Rendered raw UmpRawEvent visually.

### v3.3.0 - Experimental UMP backend parser integration
- Improved experimental fake UMP table with Bits, Status, and Channel.
- Added richer parsed fields using UmpParser intelligently.
- Implemented fast bit-shifting heuristics in the UI layer.
- No real UMP backend implementation yet.

### v3.4.0 / v3.4.1 - Experimental UMP backend export & Auditing
- Added TXT/CSV export for Experimental UMP Backend table.
- Fixed experimental backend CSV export spreadsheet notation logic.
- Conducted exhaustive test audits validating MT 0x1, 0x2, 0x4 limits.
- No real UMP backend implementation yet.

### v3.5.0 / v3.5.1 - Fake UMP backend session recording
- Added fake UMP session recording.
- Added TXT/CSV export for recorded fake UMP sessions.
- Recording is independent from the experimental table view.
- No real UMP backend implementation yet.

### v3.6.0 - Fake UMP session summary report
- Added summary report export for Fake UMP Session Recording.
- Summary includes event totals, Message Type counts, Group counts, Status counts, Channel counts and representative examples.
- Report is generated from recorded fake UMP session data.
- No real UMP backend implementation yet.

### v3.7.0 - Experimental UMP backend documentation and demo polish
- Documentation polish release.
- Added demo guide, limitations, TCC explanation and evidence checklist.
- Clarifies that FakeUmpInputBackend is a prototype/simulation.
- No real UMP backend implementation yet.

### v3.8.0 - TCC final demo readiness review
- Final documentation review for academic demonstration.
- Added demo script, checklist, defense Q&A and final release notes.
- No real UMP backend implementation yet.
- No C++ logic changes.

> **Nota Final:** A versão v3.x é considerada a série madura, congelada e *TCC-ready* (Pronta para a banca acadêmica) estabelecendo a fundação indestrutível de Mock (Fake UMP Backend). Desenvolvimentos orgânicos foram transferidos para a linha v4.x.
