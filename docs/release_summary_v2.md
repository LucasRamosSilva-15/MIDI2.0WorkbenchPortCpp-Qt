# Release Summary v2

O projeto sofreu amadurecimento integral a partir das decisões estruturais e desdobramentos de refatoração para integrar dados vivos (hardware streaming em Live Monitor) aos módulos estáticos isolados legados da arquitetura.

O roadmap do `v2` seguiu o fluxo ordenado abaixo:

- **v2.0.0**: Estabelecida base e camada abstrata de arquitetura Mock para MIDI-IN real.
- **v2.1.0**: Integração e gestão da dependência FetchContent opcional (`RtMidi`).
- **v2.2.0**: Liberação de mapeamento dinâmico visual validando varreduras estritas no sistema operacional sem bloqueio (blocking reads) invasivos na thread visual principal.
- **v2.3.0**: Incorporação visual da tabela experimental base do Live Monitor nos pacotes.
- **v2.4.0**: Lançado framework do Cloud CI para distribuir nativamente a flag `-EnableRtMidi` aos *release zipfiles* em duplicata aos modelos passivos.
- **v2.5.0**: Refino lógico ativando as portas. Mecanismos `openPort`/`closePort` habilitados e travamentos visuais estabelecidos garantindo segurança de interface de hardware.
- **v2.6.0**: Integração final do buffer nativo. Componente seguro thread-safe coleta array dinâmico `uchar` vindos do som e aciona locks para despache rápido de *MidiRawEvent*.
- **v2.6.1**: Correções pontuais no CMakeLinker sob isolamentos offline (desacoplamento limpo do linker dinâmico).
- **v2.7.0**: Adição robusta do `Midi1LiveDecoder`. Textos brutos ganharam inteligência extraída das classificações dos bytes e suas fatias para *Logs Visuais* ricos e explicativos em língua inglesa (*Note On*, *CC*).
- **v2.8.0**: Resolução drástica de colapso de poluição visual na GUI refatorando *UmpParser*, *Live Monitor*, *Logs* e *About* para navegações modulares limpas por abas (`QTabWidget`).
- **v2.8.1**: Reparo pontual dos limites e scrolls nativos do controle log.
- **v2.9.0**: Inserção imperativa de controles fundamentais: "Limpeza de Logs" rápida (Flush) e "Pausa Visual", permitindo que o processamento interno corra invisível a falhas renderizadas em caso de afunilamento de buffer do SO.
- **v2.10.0**: Separação rigorosa para gerar `Live CSV` e `Live TXT` (Reports do tempo real) dissociando e evitando concorrências ilógicas de exportadores legados da Aba Offline original do software.
- **v2.11.0**: Barras de Filtros em Tempo Real permitindo limpezas de tipo ou canal. Incluída lógica estrita tratando "Velocity 0" implicitamente como *Note Off* nativo de forma passiva.
- **v2.12.0**: Lançamento das `Statistics` visuais agregadas atestando taxa amostral (Rate) e agrupando deltas (Recebidas x Exibidas).
- **v2.13.0**: Prototipagem da lógica central do UMP Preview. Bit-shifts ativando representação de pacotes limpos `Message Type 0x2`.
- **v2.14.0**: O UMP Preview se manifesta fisicamente numa tabela grid separada em tempo-real (Live UMP Preview Table) preenchendo as Words em canais alinhados de forma gráfica instantânea no UI.
- **v2.15.0**: Tratamentos de redimensionamento dos Splitters e alturas padrões para suporte a telas modestas garantindo proporções seguras nos painéis múltiplos.
- **v2.16.0**: Criação paralela de cofre autônomo e invulnerável à tela (RAM buffer) chamado de "Session Recording", com comandos independentes `Start` e `Stop` de amostragem perene isolada do tráfego.
- **v2.17.0**: Transformação dos dados gravados em resumos acadêmicos estruturados de métricas em bloco, culminando no *Live MIDI Session Summary Report*.
- **v2.18.0**: Refino e embelezamento textual acadêmico explícito implementando divisórias e detalhamento estrutural dos componentes da gravação extraída (`.txt`).
- **v2.19.0**: Versão documental TCC Package. Criação exclusiva de roteiros de demonstração técnica, explanação de lógica paralela matemática MT 0x2 e documentação técnica oficial expondo limites.
