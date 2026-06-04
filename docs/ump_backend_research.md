# Experimental MIDI 2.0 / UMP Backend Research

## Goal
Este documento serve como fundação teórica para investigar, avaliar e catalogar as opções disponíveis para implementar uma captura real de pacotes UMP (Universal MIDI Packet) e MIDI 2.0 no futuro da aplicação.

## Current Project State
Atualmente, o projeto utiliza o backend **RtMidi**, que lida estritamente com **MIDI 1.0 em bytes crus**. 
O "UMP Preview" operante na aplicação converte (via bit-shifting) essas mensagens MIDI 1.0 Channel Voice para uma representação UMP de 32-bits em memória (**Message Type 0x2**).
- Isso **não** equivale a uma captura nativa de pacotes UMP transportados por um driver USB no S.O.
- Isso **não** gera dados MIDI 2.0 Channel Voice reais de alta resolução (**Message Type 0x4**).

## What "Real UMP Backend" Means
Construir um backend de "UMP Real" implica em:
- Conectar-se a uma API de baixo nível do Sistema Operacional que trafegue fluxos de pacotes UMP nativos.
- Extrair words íntegras de 32, 64, 96 ou 128 bits geradas por hardware legítimo.
- Preservar o *Message Type*, *Group* e payload recebido.
- Diferenciar adequadamente um hardware legado que o SO encapsulou em MIDI 1.0 *in* UMP (MT 0x2) de um teclado MIDI 2.0 nativo que dispara Voice MT 0x4 de 64-bits.

## Candidate Backend: Windows MIDI Services
A nova stack oficial da Microsoft para o ecossistema musical no Windows.
- **O que é:** API unificada para MIDI 1.0 e MIDI 2.0 que fornece suporte oficial ao formato UMP e à negociação bidirecional de MIDI-CI.
- **Integração:** Exige uso de bibliotecas de Windows SDK (WinRT / C++). 
- **É o candidato primário** para a próxima grande iteração no ambiente Windows.
- **Riscos e Dificuldades:**
  - Forte dependência de builds recentes do Windows 11.
  - Curva de aprendizado numa API ainda em evolução pela Microsoft.
  - A integração e injeção do SDK nos ambientes CMake via MSVC pode encarecer e complexificar o build.
  - Escassez de hardware físico no mercado (para testar *MT 0x4* real).

## Candidate Backend: Linux ALSA UMP
Subsistema de som oficial do Kernel Linux que recebeu patches robustos para lidar nativamente com UMP.
- **O que é:** O *ALSA sequencer API* atualizado suporta o tráfego rawmidi para MIDI 2.0.
- **Relevância:** Caso a arquitetura deste projeto se direcione para dispositivos embarcados (como Raspberry Pi) ou hosts Linux, o ALSA UMP é obrigatório.
- **Riscos e Dificuldades:**
  - Instabilidade em versões de *Kernel* fragmentadas no ecossistema de distribuições.
  - Abstrações POSIX e callbacks do ALSA diferem muito estruturalmente da API Windows.

## Candidate Backend: libremidi
Biblioteca open-source mantida ativamente com o objetivo de suceder as funções C++ portáteis da era do RtMidi.
- **O que é:** Um wrapper moderno que projeta ser cross-platform e integrar MIDI 2.0.
- **Relevância:** Se provar maturação, pode poupar o desenvolvimento solitário contra o *ALSA* e *Windows MIDI Services*. **Deve ser tratada como candidato a avaliar, não como decisão cravada.**
- **Perguntas em Aberto:**
  - Ela já expõe a "Word UMP" nativa para o desenvolvedor de forma transparente?
  - Ela lida bem com os stubs do Windows MIDI Services localmente?
  - A inclusão no CMake (FetchContent) é estável frente ao compilador MSVC?

## Candidate Backend: JUCE MIDI 2.0 preview
O poderoso e onipresente framework focado em VSTs e Áudio.
- **O que é:** Possui um preview dedicado ao MIDI 2.0.
- **Veredito:** Embora seja uma excelente referência arquitetural de como os engenheiros da JUCE lidam com UMP, arrastar a colossal dependência do JUCE para dento de um projeto que já conta com Qt6 nativo causaria inchaço inaceitável.

## Proposed Architecture
Para proteger a interface, será preciso projetar uma abstração futura C++ no projeto, paralela à atual de MIDI 1.0:

```cpp
struct UmpRawEvent {
    InputSourceType sourceType;
    double timestampMs = 0.0;
    std::vector<uint32_t> umpWords;
    QString backendName;
    QString portName;
};

class IumpInputBackend {
public:
    virtual ~IumpInputBackend() = default;
    virtual QString backendName() const = 0;
    virtual QStringList listInputPorts() = 0;
    virtual bool openInputPort(int portIndex) = 0;
    virtual void closeInputPort() = 0;
    virtual bool isOpen() const = 0;
    virtual std::vector<UmpRawEvent> pollUmpEvents() = 0;
};
```
- **IMidiInputBackend** continuará responsável pelos legados via RtMidi.
- **IumpInputBackend** centralizará os injetores de UMP nativo e enviará aos motores isolados (reaproveitando o poderoso `UmpParser` offline para classificar as strings e UI logic em live-mode).

## Integration Plan (Fases Futuras)
- `v3.1.0` - Define UMP backend interfaces (Stubs).
- `v3.2.0` - Windows MIDI Services prototype documentation/stub.
- `v3.3.0` - Experimental Windows UMP input backend (POC).
- `v3.4.0` - Live UMP packet table integration.
- `v3.5.0` - UMP backend packaging research (DLLs/SDK bundles).
- `v3.6.0` - Linux ALSA UMP research (Opcional).

## Open Questions
- Qual versão mínima *exata* (build number) do Windows 11 será imposta ao usuário?
- O SDK Runtime do Windows MIDI Services pode ser injetado nos `.zip` da release, ou precisaremos obrigar a instalação prévia via Microsoft Store?
- Como listar/enumerar *endpoints* UMP e diferenciá-los dos endpoints MIDI 1.0 (que o próprio SO traduz localmente)?
- Como desenvolver e testar sem um instrumento físico habilitado a MT 0x4 (Uso de *FakeUmpBackend*)?
- Como planejar a arquitetura C++ futuramente para chamadas requisição/resposta de *MIDI-CI*?

## Decision for v3.0.0
Foi estipulado formalmente que **não implementaremos código de backend físico nesta versão.**
A `v3.0.0` delimita as bases, a interface, os planos arquiteturais e o desbravamento bibliográfico. O RtMidi continua intacto como backend Live, e o MT 0x2 Preview como demonstração de estabilidade teórica.
