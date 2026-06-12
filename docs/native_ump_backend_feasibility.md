# Native UMP Backend Feasibility Research

## Goal
O objetivo desta pesquisa é estudar a transição suave do protótipo simulado (`FakeUmpInputBackend`) para um backend UMP nativo e funcional com hardwares conectados à máquina.

## Current State (v4.1.0)
Neste momento (linha evolutiva v4.1.0):
- Foi forjado o **Esqueleto Arquitetural (Skeleton)** do motor WindowsMidiServicesBackend C++, provando a estabilidade da interface IUmpInputBackend. 
- A compilação é estéril; retorna falsos/vazios e o hardware não é acessado. Flag isolada `ENABLE_WINDOWS_MIDI_SERVICES=OFF`.
- O componente de áudio real do projeto baseia-se unicamente no `RtMidiBackend`, que por sua natureza restrita capta apenas MIDI 1.0 (bytes crus de 8-bits).
- A interface exibe pacotes UMP em demonstrações live via `Midi1ToUmpPreviewConverter` (convertendo o fluxo real 1.0 para o contêiner UMP MT 0x2).
- Os fluxos 100% UMP (ex: MT 0x4 High-Resolution) só operam hoje pelo gerador em C++ na memória: `FakeUmpInputBackend`.

## Candidate Backends

### 1. Windows MIDI Services
- É o candidato **principal** da nossa matriz Windows Qt6/MSVC.
- Oferece suporte completo e nativo a arquitetura de pilhas MIDI 1.0, MIDI 2.0 e Universal MIDI Packet.
- *Limitação temporária:* Exige possivelmente um SDK Runtime ou Tools instalado externamente à biblioteca core do Windows. Pode requerer conhecimento em WinRT/COM C++.

### 2. ALSA UMP
- Candidato direto para distribuições Linux de baixa latência.
- Trata-se de uma via essencial para que o porto Qt cumpra sua promessa multiplataforma em *Raspberry Pi* e correlatos no futuro.
- *Decisão:* Não deve ser prioridade enquanto os mantenedores estiverem engessados na matriz de builds Windows/MSVC.

### 3. libremidi
- Trata-se de uma biblioteca paralela (semelhante a nossa herança do RtMidi) que atua como abstração multiplataforma.
- Em constante desenvolvimento, clama possuir headers de UMP.
- *Decisão:* A sua injeção pesada no CMake prejudica a didática da arquitetura atômica do nosso projeto. Será avaliada num ponto futuro no longo prazo de *refactoring*.

### 4. JUCE
- O motor formidável da indústria de plugins e áudio nativo. Possui MIDI 2.0 nativo embrionário e massivo.
- *Decisão:* Considerar apenas como referência didática. O escopo do `MIDI 2.0 Workbench Port` é alicerçado sob o *framework* visual modular do Qt6. Arrancar o projeto para um pipeline pesado da JUCE destruiria anos de UI baseada em `QWidgets` e violaria o peso/performance proposta.

## Risks
- O SDK da Microsoft ainda está em constante mutação nos Windows Insiders.
- A retrocompatibilidade do Windows Desktop do usuário médio pode apresentar ruído.
- Dispositivos MIDI 2.0 reais continuam escarssos no varejo nacional.
- O Build local (MSVC/CMake) não pode quebrar caso a máquina matriz careça do SDK Microsoft.

## Recommendation
- O `FakeUmpInputBackend` será **mantido ad aeternum** para cobrir desenvolvedores ausentes do hardware ou SDK.
- Um backend nativo real será criado como **opcional protegido por flag** no *CMake*.
- O primeiro degrau prático para desenvolvimento futuro é a simples e inofensiva **listagem de endpoints (portas)** do Windows MIDI Services, antes de sequer construir funções de escuta/captura total.
- Flag a ser reservada: `ENABLE_WINDOWS_MIDI_SERVICES=OFF` (padrão desligado).
