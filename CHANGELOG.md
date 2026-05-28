# CHANGELOG

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
