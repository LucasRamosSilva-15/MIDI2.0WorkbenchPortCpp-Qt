# Known Limitations

O MIDI 2.0 Workbench Port assume responsabilidades bem delineadas de validação estática. Com o acréscimo das simulações "Live MIDI", é importante registrar técnica e profissionalmente as limitações arquiteturais conhecidas para orientação acadêmica e profissional.

- **RtMidi fornece bytes crus:** O sistema RtMidi trabalha extraindo as mensagens do subsistema de som legadas puras (streams de bytes MIDI 1.0).
- **O projeto não atua como Host MIDI completo:** O objetivo não é rotear sinais ou comportar-se como uma DAW multipista (Digital Audio Workstation) de latência nula.
- **Live UMP Preview experimental:** Trata-se de uma visualização didática forense em grid, não um tradutor transparente embutido para controle de equipamentos físicos em tempo real (pass-through converter).
- **Conversão focada em Channel Voice:** Somente dados Channel Voice MIDI 1.0 nativos (Note, CC, Bend) são transformados e convertidos ativamente para o pacote UMP MT 0x2.
- **Sinais críticos não geram UMP:** Mensagens longas e orientadas ao hardware nativo como SysEx (System Exclusive), System Common e mensagens de Real-Time (Active Sensing, Clocking) não são convertidas no fluxo Preview atualmente.
- **MIDI-CI não implementado:** O ecossistema de requisição *MIDI Capability Inquiry* atuando em dois-lados para descobrimento universal das capacidades não é ativado na aplicação.
- **Property Exchange e Profiles:** Recursos de metadados robustos e esquemas comportamentais (como teclados mudando modos organ/synth) por via da spec MIDI 2.0 não foram implementados.
- **Conversão para MIDI 2.0 MT 0x4 não implementada:** Não convertemos falsamente um teclado MIDI 1.0 clássico em dados "MIDI 2.0 High-Resolution" de 64 bits para evitar data-padding injustificado ou inferências equivocadas da dinâmica do instrumento.
- **A ausência do Windows MIDI Services:** A captura não tira proveito do transporte UMP USB real e de alta-prioridade por via de drivers diretos de classe USB nativa ao SO na atual etapa.
- **Sessão em Memória Volátil:** O módulo de "Session Recording" grava centenas de chamadas em estado puramente volátil (QVector RAM). Isso impõe perda fatal do registro se a gravação não for interrompida e exportada adequadamente num arquivo de sumário.
- **Restrição de Carga de Interface:** Adota-se um limite máximo explícito de cerca de 1000 linhas na renderização para as janelas e logs dinâmicos. A gravação em background de uma sessão não respeita tal limite, o que induz claras diferenças entre dados exibidos visualmente ao vivo (filtrados/suprimidos) perante os totais salvos no log e no Session Recording analítico final.
