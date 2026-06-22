# CHANGELOG

## v4.12.0 - Windows MIDI Services C++/WinRT generated projection strategy research
* Added C++/WinRT generated projections strategy research.
* Investigated the missing `impl/` headers and `.winmd` structural requirements.
* Documented `cppwinrt.exe` pipeline limits inside isolated build constraints.
* Safe mode kept intact. No endpoint listing or UI activation.

## v4.11.0 - Windows MIDI Services C++/WinRT projection alignment research
* Added C++/WinRT projection alignment diagnostics.
* Added safe diagnostic mode for type reference research.
* Added explicit real include attempt flag for local diagnostics only.
* Documented `CPPWINRT_VERSION` mismatch and missing generated impl header issue.
* Kept normal and RtMidi builds SDK-free.
* No runtime API calls, endpoint listing or UMP capture yet.

## v4.10.0 - Windows MIDI Services isolated API type reference compile experiment
- Added optional `ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT` flag.
- Added isolated `WindowsMidiServicesTypeReferenceProbe`.
- Updated SDK diagnostic report with type reference experiment status.
- Kept build normal and RtMidi SDK-free.
- No runtime API calls, endpoint listing or UMP capture yet.

## v4.9.0 - Windows MIDI Services SDK candidate discovery correction
- Corrected Windows MIDI Services SDK candidate discovery.
- Added detection for `Microsoft.Windows.Devices.Midi2` WinMD metadata candidates.
- Added detection for `Microsoft.Windows.Devices.Midi2` header candidates.
- Kept detection restricted to `WINDOWS_MIDI_SERVICES_SDK_ROOT` with `NO_DEFAULT_PATH`.
- Kept discovery non-fatal and research-only.
- No real API calls, endpoint listing or UMP capture yet.

## v4.8.0 - Windows MIDI Services optional header include compile experiment
- Introduced `ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT`.
- Created `WindowsMidiServicesHeaderIncludeProbe` to run `#include` checks completely isolated from core system.
- Kept UI untouched. Main functionality untouched.
- No real Native API calls are made.

## v4.7.0 - Windows MIDI Services optional header detection with SDK root
- Added optional `find_path`-based header detection using `WINDOWS_MIDI_SERVICES_SDK_ROOT`.
- Restricted header search to the user-provided SDK root.
- Added formatted header detection diagnostics.
- Kept detection non-fatal and SDK-free for normal builds.
- No real header consumption, endpoint listing or UMP capture yet.

## v4.6.0 - Windows MIDI Services user-provided SDK root research
- Added optional `WINDOWS_MIDI_SERVICES_SDK_ROOT` CMake variable.
- Added SDK root readiness fields to WindowsMidiServicesSdkProbe.
- Added formatted SDK root diagnostics.
- Documented manual SDK root research workflow.
- Kept SDK usage optional and non-fatal.
- No real SDK headers, endpoint listing or UMP capture yet.

## v4.5.0 - Windows MIDI Services optional package detection experiment
- Added optional package/header detection readiness fields to WindowsMidiServicesSdkProbe.
- Added documentation for future non-invasive SDK package/header detection.
- Created dedicated WindowsMidiServicesBackendTests test file.
- Kept Windows MIDI Services SDK dependency optional and non-required.
- No real endpoint listing or UMP capture yet.

## v4.4.0 - Windows MIDI Services SDK detection research
- Added Windows MIDI Services SDK detection report structure.
- Added formatted diagnostic output for SDK experiment status.
- Added documentation for SDK detection research.
- Preserved stable builds without SDK dependency.
- No real SDK header probing, endpoint listing or UMP capture yet.

## v4.3.0 - Windows MIDI Services SDK build experiment
- Added optional ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT build flag.
- Added documentation for future Windows MIDI Services SDK integration.
- Preserved stable builds without SDK dependency.
- No real Windows MIDI Services endpoint listing or UMP capture yet.
- No UI backend selector yet.

## v4.2.0 - Windows MIDI Services endpoint listing research
- Updated README to reflect current v4.x project state.
- Added endpoint listing research notes for Windows MIDI Services.
- Prepared WindowsMidiServicesBackend skeleton for future endpoint enumeration.
- Preserved build stability without Windows MIDI Services SDK dependency.
- No real endpoint listing or UMP capture yet.

## v4.1.0 - Windows MIDI Services backend skeleton
- Added initial WindowsMidiServicesBackend skeleton.
- Added optional ENABLE_WINDOWS_MIDI_SERVICES flag, OFF by default.
- Preserved FakeUmpInputBackend as the working test backend.
- No real Windows MIDI Services SDK integration yet.
- No real UMP endpoint listing or capture yet.
- Updated CI workflow to run on v4-experimental branch.

## v4.0.0 - Native UMP backend feasibility research
- Started v4.x research line for native UMP backend feasibility.
- Added Windows MIDI Services feasibility notes.
- Added native UMP backend roadmap.
- Added backend decision matrix.
- Clarified that no real UMP capture is implemented yet.
- No parser, backend, converter, UI polling, export, recording or summary logic changes.

## v3.8.0 - TCC final demo readiness review
- Added final TCC demo script.
- Added final demo checklist.
- Added expected defense questions and answers.
- Added final release notes for v1/v2/v3 project evolution.
- Reviewed documentation to avoid claiming real UMP hardware capture.
- No parser, backend, converter, polling, export, recording or summary logic changes.

## v3.7.0 - Experimental UMP backend documentation and demo polish
- Added documentation for demonstrating the Experimental UMP Backend.
- Added TCC-oriented explanation of the fake UMP backend pipeline.
- Added limitations document for fake backend, UMP preview and real UMP capture.
- Added evidence checklist for academic presentation.
- Updated About/Help text to clarify the experimental/fake nature of the UMP backend.
- No parser, backend, converter, export or polling logic changes.

## v3.6.0 - Fake UMP session summary report
- Added TXT summary report for Fake UMP Session Recording.
- Added counts by Message Type, Group, Status and Channel.
- Added representative examples from recorded fake UMP events.
- Preserved separation from table export, session export, Offline UMP Analyzer and Live MIDI Monitor.
- No Windows MIDI Services, ALSA UMP or real MIDI 2.0 backend yet.

## v3.5.1 - Fix fake UMP session recording buttons
- Fixed scope omission for Qt connects, making session UI recording buttons fully operational.

## v3.5.0 - Fake UMP backend session recording
- Added session recording for Experimental UMP Backend.
- Added TXT/CSV export for fake UMP recorded sessions.
- Preserved separation from table export, Offline UMP Analyzer and Live MIDI Monitor.
- No Windows MIDI Services, ALSA UMP or real MIDI 2.0 backend yet.

## v3.4.1 - Fix experimental export and add MT 0x1 tests
- Fixed experimental backend CSV export to enforce `="val"` spreadsheet-safe strings, preventing hex values from being parsed as scientific notation.
- Injected default temporal filenames (`experimental_ump_export_YYYYMMDD_HHMMSS`) in export dialogues.
- Enforced standard BOM UTF-8 bytes to CSV generation.
- Added explicit test routines for MT 0x1 System Real-Time inside the offline parser (`10F80000`, `10FA0000`, `10FC0000`).
- Clarified that MIDI 2.0 MT 0x4 *Velocity 0* maps implicitly to Note On, unlike MT 0x2.

## v3.4.0 - Experimental UMP backend export
- Added TXT export for Experimental UMP Backend table.
- Added CSV export for Experimental UMP Backend table.
- Preserved separation from Offline UMP Analyzer, Live MIDI Monitor and Session Recording exports.
- No Windows MIDI Services, ALSA UMP or real MIDI 2.0 backend yet.

## v3.3.0 - Experimental UMP backend parser integration
- Improved Experimental UMP Backend table with richer parsed fields.
- Added MT, Group, Status, Channel and Size columns.
- Improved UmpParser reuse for fake UMP events.
- Maintained separation from Offline UMP Analyzer and Live MIDI Monitor.
- No Windows MIDI Services, ALSA UMP or real MIDI 2.0 backend yet.

## v3.2.0 - Fake UMP backend integration prototype
- Added "Experimental UMP Backend" tab in UI.
- Integrated FakeUmpInputBackend to validate UMP UI polling pipeline.
- Visual display of extracted UmpRawEvent (MT, Group, Size).
- Re-used UmpParser silently for deep-packet description resolution.
- Kept Live MIDI 1.0 logic untouched and independent.

## v3.1.0 - Define UMP backend interfaces
- Added UmpRawEvent structure.
- Added IUmpInputBackend interface.
- Added FakeUmpInputBackend for hardware-free testing.
- Added tests for fake UMP backend behavior.
- No Windows MIDI Services, ALSA UMP or real MIDI 2.0 backend yet.

## v3.0.0 - Experimental MIDI 2.0/UMP backend research
- Added experimental UMP backend research documentation.
- Added future IumpInputBackend architecture plan.
- Added Windows MIDI Services notes.
- Initiated v3 release summary.
- No functional MIDI backend changes (RtMidi kept intact).

## v2.21.0 - Stability and final review
- Conducted final stability pass for TCC presentation.
- Added Final Review Checklist document.
- Verified all documentation, links, and versions.
- Preserved core architecture without adding new MIDI features.

## v2.20.0 - TCC demo polishing
- Improved About / Help text for presentation use.
- Added screenshots guide.
- Added short demo script.
- Added TCC demo sample (tcc_demo_midi1_to_ump.txt).
- Updated README demo links.
- Updated quick start instructions.

## v2.19.0 - TCC documentation package
- Added TCC-focused documentation package
- Added demo guide
- Added architecture overview
- Added Live MIDI to UMP explanation
- Added known limitations document
- Added test plan
- Added v2 release summary

## v2.18.0 - Live MIDI session report polishing
- Refinamento textual e visual do relatório de sessão Live MIDI, agregando formato acadêmico, propósito, metodologia, e melhorias de legibilidade.
- Exemplos representativos formatados estruturalmente com indentação, bytes e decodificação descritiva.
- Declarações explícitas sobre limitações de conversão (somente UMP MT 0x2, sem MT 0x4 ou mensagens de sistema).

## v2.17.0 - Live MIDI session summary report
- Adicionada funcionalidade de exportação de um relatório analítico resumido em formato TXT sobre a sessão Live MIDI gravada.
- O relatório agrega totais de eventos, contagens por tipo e canal, e captura dinamicamente o primeiro evento de cada categoria (Note On, Note Off, CC, etc.) para exemplificação.

## v2.16.0 - Live MIDI session recording
- Implementado sistema de gravação em memória paralela (`m_liveMidiRecording`), independente do buffer visual da interface.
- Adicionados controles dedicados (Iniciar Gravação, Parar Gravação, Limpar Sessão, Exportar TXT/CSV).
- A gravação preserva todos os eventos sem ser afetada pelo limite visual ou pela pausa da interface gráfica.

## v2.15.0 - Live Monitor layout refinement
- Refinamento do layout da aba Live MIDI Monitor, otimizando proporções do QSplitter e adicionando scroll areas para melhor suporte a monitores menores.
- Reorganização e espaçamento dos componentes de Live MIDI e UMP Preview.

## v2.14.0 - Live UMP preview table
- Adicionada tabela visual "Live UMP Preview Table" na aba Live MIDI Monitor para exibir as conversões em tempo real.
- Layout reestruturado utilizando um `QSplitter` vertical, permitindo redimensionar a nova tabela e o log original.
- A tabela separa estruturalmente dados do UMP: Timestamp, Bytes, UMP Word, MT, Group, Status e Channel.
- O botão "Limpar Live Log" agora também limpa o histórico da tabela de preview.

## v2.13.0 - Live MIDI 1.0 to UMP preview
- Adicionada prévia experimental de conversão de mensagens Live MIDI 1.0 para UMP (MIDI 1.0 Channel Voice, MT 0x2).
- Checkbox na aba Live MIDI Monitor permite ligar/desligar a exibição da UMP word (32 bits) no log da interface.
- Nova coluna `ump_preview` incluída na exportação Live CSV.
- Arquivo Live TXT agora exporta o sufixo `| UMP: XXXXXXXX` junto às linhas, se habilitado.

## v2.12.0 - Live MIDI statistics
- Adicionada seção de estatísticas do monitor Live MIDI, agrupando metadados de pacotes processados.
- Implementado cálculo de rate (mensagens por segundo) com base no tempo de sessão da porta.
- Adicionada contagem de mensagens filtradas (`Recebidas - Exibidas`).
- Adicionadas contagens granulares de pacotes recebidos por tipo (Note On, CC, etc.) e por canal.
- Adicionados rastreadores de "Último valor recebido" detalhando Notas, Velocities, Control Changes, Pitch Bend e Channels.

## v2.11.0 - Live MIDI filters
- Adicionada barra de "Filtros Live MIDI" no monitor em tempo real.
- Permite filtrar visivelmente pacotes por "Tipo de Mensagem" e "Canal", inclusive suportando opções globais de hardware como System Real-Time.
- Inclui checkbox para tratar pacotes de Note On com Velocity 0 como Note Off nativamente.
- O filtro é aplicado na tela sem causar paradas/drops no hardware de origem (os pacotes continuam sendo esvaziados).

## v2.10.0 - Live MIDI export
- Adicionados botões dedicados de exportação no Live MIDI Monitor ("Exportar Live TXT" e "Exportar Live CSV").
- As exportações refletem a lista real e estruturada das mensagens visualizadas, contornando parsing ineficiente da GUI.
- Separação completa e estrita entre as lógicas de Exportação Offline (Tabela de UMP Hex) e de Captura Live.
- O formato CSV inclui suporte a UTF-8 com fallback RFC 4180 de aspas, para abrir confiavelmente em Excel/Numbers.

## v2.9.0 - Live MIDI monitor controls
- Adicionados controles avançados na aba do Live MIDI Monitor.
- Botão "Pausar/Retomar Monitor" adicionado, permitindo que a porta permaneça aberta e drenada sem sobrecarregar a renderização da GUI.
- Novo sistema de contagem que diferencia mensagens "Recebidas" na porta e mensagens "Exibidas" no log.
- Botão dedicado para "Limpar Live Log".
- Implementado limite de bloco na GUI do log live (1000 linhas) para evitar perdas de desempenho e congelamentos na thread principal.

## v2.8.0 - UI layout reorganization
- Reorganização completa da interface de usuário da `MainWindow` em abas (`QTabWidget`).
- Criadas abas dedicadas para: `Offline UMP Analyzer`, `Live MIDI Monitor`, `Logs / Diagnostics` e `About / Help`.
- Resolução do problema de espaço e poluição visual, mantendo as funcionalidades de backend e integração intocadas.
- Adicionada aba estática `About / Help` contendo instruções de uso curtas e destacando claramente as limitações do app.

## v2.7.0 - Decode live MIDI 1.0 messages
- Novo componente estático `Midi1LiveDecoder` no backend do sistema isolado.
- Adicionada capacidade de analisar os bytes hexadecimais brutos de dispositivos MIDI de hardware mapeando canais e o significado dos eventos MIDI 1.0 principais (Channel Voice).
- Eventos `Note On`, `Note Off`, `Control Change`, `Pitch Bend`, e os Aftertouches e Program Changes são decodificados nativamente no painel de log Live MIDI sem misturar com o fluxo UMP original.
- Pacotes de System/Common/Real-Time (ex: *Timing Clock* ou System Exclusive longos) são intencionalmente filtrados como "(parcial/bruto)" nesta versão temporária.

## v2.6.0 - Receive raw MIDI 1.0 bytes
- Implementada rotina thread-safe (lock/queue + QTimer) no pacote experimental para processamento contínuo do RtMidi sem travar ou dessincronizar a interface gráfica.
- Mensagens MIDI 1.0 recebidas do *hardware* de áudio são agora renderizadas (estampa temporal e *bytes hexadecimais*) em tempo real em uma nova mini-janela dedicada (modo Lab).
- A tabela principal e o motor UMP (`UmpParser`) permanecem protegidos, sem receber interferência das mensagens 1.0 enquanto a tradução nativa para *UMP 32-bit Words* não está finalizada.
- A compilação *offline* padrão (sem a flag *RtMidi*) segue em isolamento máximo.

## v2.5.0 - Open and close MIDI input port
- Introduzido suporte real de abertura e liberação de portas de hardware MIDI nativas no sistema via RtMidi (`openPort()` e `closePort()`).
- Adicionados controles visuais dinâmicos ("Abrir porta", "Fechar porta") na seção experimental, bloqueando edição durante conexão.
- O painel de Diagnóstico/Status agora monitora e reporta explicitamente o status da conexão (`Desativado`, `Pronto`, ou `Aberta`).
- Apenas afeta a `MainWindow` sob a compilação com tag RtMidi; compilação padrão (Offline) segue isolada.
- Recepção de dados em tempo real ainda não ativada (fase futura).

## v2.4.0 - RtMidi-enabled release build option
- O script de empacotamento (`package_release.ps1`) agora suporta a flag `-EnableRtMidi` para forçar o build em pasta paralela e envelopar as bibliotecas geradas no FetchContent.
- A pipeline de Integração Contínua (CI) via GitHub Actions (`release.yml`) foi atualizada para expor duplamente os artefatos de compilação a cada tag `vX.Y.Z` gerada.
- Garante separação rigorosa de modo que os usuários do pacote padrão offline continuem 100% isolados de instâncias ou bibliotecas do RtMidi.

## v2.3.0 - Experimental MIDI input port UI
- Introduzida uma área visual de laboratório ("Live MIDI Experimental") diretamente na interface principal.
- Agora é possível pressionar "Atualizar portas MIDI" para povoar um `QComboBox` com as portas reais detectadas no S.O.
- Tratamento explícito quando compilado com `ENABLE_RTMIDI=OFF` informando o estado da build desativada.
- Sem aberturas de porta ou envio de pacotes implementados; puro mapeamento seguro.

## v2.2.0 - List MIDI input ports with RtMidi
- Implementada com sucesso a varredura e listagem de portas ativas de hardware MIDI via `RtMidiIn::getPortCount()` e `getPortName()`.
- A integração foi confirmada em ambos os perfis CMake: retornando portas reais do SO quando `ENABLE_RTMIDI=ON` e silenciando com segurança (`QStringList` vazio) quando `OFF`.
- Mantida rigorosamente a promessa de não abrir a porta (não alocar buffer de recebimento) nem injetar callbacks.
- A UI permanece puramente inalterada, mantendo o `UmpParser` intacto.

## v2.1.0 - RtMidi Dependency Integration
- Integrada a estrutura inicial para gerenciar a dependência do `RtMidi` via `FetchContent` do CMake de maneira 100% opcional.
- A flag `ENABLE_RTMIDI=ON/OFF` agora controla de forma fluida se a aplicação compila como um sistema estrito sem hardware (`OFF`) ou traz a robustez de portas em tempo real (`ON`).
- Adicionado o backend `RtMidiInputBackend` implementando stubs perfeitamente seguros (sem vazamentos de memória ou concorrência exposta). A UI segue livre de amarras.

## v2.0.0 - Live MIDI Backend Architecture
- **Ponto de Inflexão Arquitetural**: Criada a fundação esquelética para receber eventos MIDI reais (live hardware input).
- Implementada a interface `IMidiInputBackend` para isolar a UI de bibliotecas de transporte reais (RtMidi, libremidi ou Windows MIDI Services).
- Introduzido `MidiInputController` como orquestrador, mantendo o `UmpParser` e as funcionalidades offline legadas intocáveis.
- Nenhum comportamento visual/funcional modificado nesta versão; serve estritamente como ponte para a futura adoção de MIDI-IN real.

## v1.9.0 - UI Status and Diagnostics Panel
- Adicionado um painel central de diagnóstico imediatamente acima da tabela de visualização.
- O aplicativo agora fornece feedback em tempo real sobre o Estado do Ambiente ("Arquivo Atual", "Status do Filtro", "Visibilidade da Tabela" e a "Última Operação realizada").
- Melhoria contínua da usabilidade (UX) mantendo o motor UMP puro intacto.

## v1.8.0 - Parser Result Export Improvements
- Novo recurso "Exportar CSV" adicionado para salvar os resultados da tabela interpretada com colunas em formato separado por vírgulas (ponto-e-vírgula e aspas, padrão seguro do Excel/Sheets).
- O botão "Salvar log" foi recriado como "Exportar TXT", gerando um relatório massivo que agora integra Data/Hora, Estatísticas de leitura, a Tabela de Resultados (formatada) e os erros em Log.
- Todas as exportações (CSV, TXT e Copiar Tabela) agora respeitam de maneira estrita o filtro ativo da UI. Linhas ocultas na tela ficam ocultas no arquivo, garantindo o princípio WYSIWYG ("What You See Is What You Get").

## v1.7.0 - Structured Validation Errors
- O motor de validação (`UmpParser`) agora devolve classificações fortes de erro (`UmpValidationError` Enum), substituindo arquiteturas legadas frágeis baseadas puramente na busca de strings na mensagem de erro.
- Maior robustez em toda a interface Qt e módulo de testes automatizados devido a verificações determinísticas como `EmptyAfterFormatting` e `IncompletePacket`.
- Preservação intacta das mensagens didáticas em português e do detalhamento exibido na tela para o usuário.

## v1.6.0 - Better Table Readability
- Aprimoramento visual e de usabilidade (UI) na tabela de resultados principal.
- As larguras padrão das colunas foram recalibradas, diminuindo colunas de valores fixos (como #, Group, Channel) e ampliando substancialmente as de dados (Words e Type).
- Novo botão "Ajustar Colunas" incluído no painel de ações para redimensionamento instantâneo do Grid de acordo com os dados apresentados.
- Confirmação do uso pleno das APIs nativas de `tooltip` via QTableWidgetItem garantindo que nada fique escondido.

## v1.5.3 - Silent Comment Blocks
- O analisador de interface agora ignora silenciosamente blocos que resultem em conteúdo vazio (como blocos puramente didáticos compostos apenas por comentários e espaços). Isso evita a poluição do log com mensagens de aviso irrelevantes sem alterar o rigor da validação da entrada.

## v1.5.2 - Block-by-block parsing and Error Handling
- Adicionado suporte a análise de múltiplos blocos independentes. O aplicativo agora divide a entrada por linhas em branco duplas (`\n\n`), permitindo processar múltiplos casos e apresentar múltiplos erros sem abortar no primeiro erro encontrado.
- Limpeza automática do Log de interface para impedir o acúmulo infinito de mensagens de erro duplicadas.
- Limpeza de dados inválidos em `all_examples.txt` e melhoria do layout didático do `invalid_inputs.txt`.

## v1.5.1 - Comments Parsing Support
- O motor `UmpParser::validateAndExtractWords()` foi aprimorado para suportar ignorar seções de texto marcadas com `#` em entradas manuais e via arquivos `.txt`. Comentários inteiros e anotações *inline* agora são descartados pacificamente antes da sanitização hexadecimal, evitando travamentos e falsos positivos de caracteres inválidos.

## v1.5.0 - UI Sample Loader
- A interface de usuário (UI) recebeu um `QComboBox` dinâmico na barra de ações. O aplicativo agora varre o disco buscando a pasta `samples/` nativamente e permite que o usuário carregue os fragmentos didáticos com um clique dentro do próprio programa, sem precisar colar manualmente.

## v1.4.0 - Sample library and example files
- Inclusão da pasta `samples/` oficial no pacote da release contendo registros didáticos fragmentados em `.txt` para MT 0x2, 0x4, SysEx, Flex e Stream. Excelente para end-users que querem ver o programa funcionar imediatamente.

## v1.3.0 - Portable Windows launcher polish
- Adição de script de lançamento inteligente (`Start-MidiUmpAnalyzer.bat`) e guia de primeiros passos em texto puro (`QUICK_START.txt`) inclusos nativamente dentro do ZIP oficial da release.

## v1.2.0 - Presentation & Polish
- Atualização visual e comercial do GitHub: adição de *badges* de status, criação da pasta remota de *screenshots* e inclusão de minitutoriais para usuários finais na página inicial (Download e Uso).

## v1.1.0 - Automatic Release Packaging
- Workflow de Lançamento (CD) introduzido. Pacotes `.zip` empacotando o executável e as bibliotecas Qt compiladas são gerados instantaneamente e indexados ao GitHub Releases sempre que uma nova tag `v*` for empurrada no repositório.

## v1.0.0 - Offline UMP Analyzer MVP
- **Release principal**: Aplicação consolidada como Analisador Offline MVP.
- Documentação refinada, limitação estrita declarada (não atua como MIDI Host em tempo real).
- Build local e automação orientada por testes consolidados.

## v0.9.0 - GitHub Actions CI
- Workflow de compilação contínua e asserção unitária isolada implementados via nuvem (Windows 2022 + Qt6).

## v0.8.0 - Automated Parser Test Harness
- Script PowerShell `run_tests.ps1` e binário `UmpParserTests` adicionados ao CMake, isolando a análise hex fora da UI gráfica para validação profunda.

## v0.7.0 - Robustness & Input Validation
- Mecanismos rigorosos implementados para bloquear *crashes* de memória: rejeição limpa de limites ímpares hexadecimais, caracteres invisíveis falhos e arquivos de log massivos (> 1MB).

## v0.6.0 - Enhanced Usability UI
- Adição da Barra de Pesquisa, botão global Copy to Clipboard (tabulado) e rótulos de contabilidade (Pacotes Lidos vs Erros). 

## v0.5.0 - Partial SysEx7/SysEx8/MDS parsing
- Descoberta e tradução de cabeçalhos estáticos (Group, Status, Form, Byte Count) de mensagens nativas MT 0x3 e MT 0x5.

## v0.4.0 - Partial Flex Data parsing
- Tradução de cabeçalhos de propriedades do Message Type 0xD, extraindo estaticamente as formatações sem dependência do MIDI-CI.

## v0.3.0 - Partial UMP Stream parsing
- Extração lógica das declarações exclusivas do MT 0xF (Endpoints, Product Instance, Protocol/Blocks) sem usar reconstrução real ou interagir como Host.

## v0.2.0 - Parsing básico de Voice Messages
- Conversão binária limpa das fatias hexadecimais pertinentes aos Message Types 0x2 (MIDI 1.0) e 0x4 (MIDI 2.0).

## v0.1.0 - Port Inicial C++ / Qt6
- Configuração básica CMake e interface *boilerplate* importando as telas do protótipo em C#.
