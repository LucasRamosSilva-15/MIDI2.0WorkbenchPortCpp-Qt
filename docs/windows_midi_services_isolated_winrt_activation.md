# Windows MIDI Services Isolated WinRT Activation Experiment (v4.13.0)

## Goal
Validar a possibilidade de inicializar os apartamentos COM do C++/WinRT (`winrt::init_apartment()`) de forma isolada, provando que o ambiente subjacente do Windows suporta as chamadas essenciais para o uso da API MIDI 2.0.

## Background
Para instanciar qualquer objeto exposto pelas projeções `.winmd` do Windows MIDI Services, a thread em execução deve ter sido previamente inicializada em um apartamento COM apropriado (geralmente Multithreaded Apartment - MTA, o padrão do C++/WinRT). Se essa etapa falhar ou entrar em conflito com o Qt6, o aplicativo sofrerá um *crash* ou exceção irrecuperável antes mesmo de se comunicar com dispositivos MIDI.

## What changed in v4.13.0
- Criação de um `WindowsMidiServicesWinRtActivationProbe` para isolar estritamente o código perigoso de `winrt::init_apartment()`.
- O modo padrão (**Safe Mode**) está ativado, o que significa que nenhuma ativação real ocorre sem autorização prévia por macros (flags do CMake).
- Adicionada a flag `ENABLE_WINDOWS_MIDI_SERVICES_REAL_WINRT_ACTIVATION_ATTEMPT` para uso apenas de desenvolvedores que desejem correr o risco localmente de ativar os apartamentos.

## Integration Constraints
**Atenção:** Esta é uma pesquisa puramente teórica e estática:
1. Nenhuma ativação COM ocorre nas compilações oficiais (Normal/RtMidi).
2. Nenhuma API de endpoints, captura UMP ou callback foi habilitada.
3. Não conectamos o Probe à inicialização da *MainWindow*.

## Future work
* **v4.14.0**: Windows MIDI Services API surface mapping (documentar as interfaces de fato, sem inicialização agressiva).
* **v4.15.0**: Windows MIDI Services endpoint enumeration prototype (só ocorrerá após toda a fase estática estar saneada).
