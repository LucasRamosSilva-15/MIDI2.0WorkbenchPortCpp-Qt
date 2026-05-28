# Live MIDI Backend Architecture

## Objetivo
Preparar a fundação de código para receber mensagens MIDI ao vivo vindas de hardwares, sem acoplar a UI diretamente a nenhuma biblioteca específica de áudio/MIDI.

## Por que não ligar RtMidi diretamente na UI?
Se a interface gráfica ou o parser offline começarem a fazer instâncias diretas de objetos RtMidi, ficaremos trancados nela para sempre. A arquitetura de plug-ins (através de `IMidiInputBackend`) garante que a UI consuma apenas um `MidiInputController` agnóstico.

## O Papel do RtMidi (Status na v2.5.0)
RtMidi é fenomenal e multi-plataforma, sendo excelente para ler e escrever mensagens *MIDI 1.0 reais byte-a-byte*. No entanto, o RtMidi não tem suporte nativo sólido a Universal MIDI Packets (UMP de 32 bits). É por isso que ele será considerado como uma fonte estrita de *Bytes 1.0*. Eventualmente, construiremos um tradutor UMP para empacotar esses bytes em UMP MIDI 1.0 Channel Voice (MT=2).

Na compilação `v2.5.0`, nós finalmente habilitamos os comandos do Sistema Operacional para fixar ponteiros (*openPort*) em dispositivos físicos/virtuais MIDI a partir da GUI. Isso consolida a dependência arquitetônica do RtMidi como um gerenciador de tráfego seguro. A recepção via *callbacks* e a respectiva conversão de *bytes* para *UMP Words* estão sendo estrategicamente preparadas. O ambiente *offline* continua intacto e desvinculado.

## Limitações e Evolução
*   **Limitação do RtMidi**: Ele extrai bytes. O MIDI 2.0 exige transporte robusto UMP (que envolve endpoints, function blocks, formatação de 32 a 128 bits). 
*   **Windows MIDI Services**: No futuro, o app poderá instanciar um backend que conversa com as novas APIs de MIDI da Microsoft nativas no Windows 11 para receber UMP limpo direto do hardware via USB.
*   **Libremidi**: Uma biblioteca em franca ascensão. Como a arquitetura agora é baseada em interface (`IMidiInputBackend`), trocar para libremidi no futuro será apenas uma questão de registrar a classe no `MidiInputController`.

## Roadmap v2.x / v3.x
*   **v2.0.0**: Fundação das interfaces.
*   **v2.x.x**: Implementação efetiva do RtMidi com conversão de Bytes para UMP e integração inicial na UI.
*   **v3.x.x**: Suporte nativo e experimental ao Windows MIDI Services.
