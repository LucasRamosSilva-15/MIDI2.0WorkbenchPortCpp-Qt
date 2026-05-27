# MIDI 2.0 UMP Analyzer (Offline MVP 0.1)

Um aplicativo de interface gráfica desenvolvido em C++ e Qt6 para atuar como um analisador básico e offline de pacotes **UMP (Universal MIDI Packet)** da especificação MIDI 2.0.

Este projeto visa facilitar o desenvolvimento e os testes visuais rápidos de lógicas de *parsing* UMP antes da integração com drivers complexos de transporte do sistema operacional ou hardwares.

## O que o app faz
Ele recebe pacotes UMP em formato hexadecimal puro (representando blocos de 32 bits da especificação) e decodifica as estruturas da mensagem, organizando-as em uma tabela amigável contendo:
- Tamanho real (32, 64, 96 ou 128 bits) deduzido pelo *Message Type*.
- Message Type (MIDI 1.0, MIDI 2.0, SysEx, Utility, Flex Data, Stream, etc.)
- Campos aplicáveis (Group, Status e Channel)
- Descrição da performance com suporte dinâmico a *payloads* extraídos.

## Como Usar
Ao abrir a aplicação, a interface oferece as seguintes ferramentas:
- **Campo "Hex UMP"**: Área multi-linha onde você pode colar *dumps* hexadecimais (com espaços e quebras de linha). O parser ignora formatação e trata o bloco como uma via contínua.
- **Interpretar**: Analisa o buffer fornecido e preenche a tabela visual e o painel de *logs* de status.
- **Carregar exemplos**: Preenche rapidamente o campo de entrada com mensagens estruturais (Note On, CC, Program Change) para agilizar os testes.
- **Abrir arquivo**: Carrega todo o texto de um `.txt` contendo *dumps* para dentro do analisador, o que é ideal após capturar pacotes de ferramentas de rede ou *debuggers*.
- **Salvar log**: Exporta os resultados da tabela UMP dissecada junto com os relatórios de integridade do painel de *log* para um arquivo de texto de fácil compartilhamento.
- **Limpar**: Zera todo o estado (entrada, tabela e log) preparando o app para uma nova rotina.

## Limitações do MVP 0.1
Visando estrita segurança e correção, o escopo do MVP está focado apenas na robustez e não em amplitude de recursos:
- **Sem conexão MIDI Real**: O programa atua 100% offline. Não estabelece nenhum *socket* ou gancho em APIs como *Windows MIDI Services* ou drivers USB.
- **Sem MIDI-CI ou Property Exchange**: Não realiza auto-descoberta, *Profile Configuration* nem *Protocol Negotiation*.
- **Parsing Intencionalmente Limitado**: A dissecação foca primordialmente em estabilidade para *Voice Messages* (MT 0x2 e MT 0x4). As cargas complexas (Payloads) de *SysEx*, *Flex Data*, e *Endpoint/Stream Messages* ainda não têm seus interiores completamente detalhados na UI para evitar falsos positivos na ausência de validação de *flags* avançadas de *Options* ou de Atributos estendidos.

## Validação e Testes
Sempre verifique o documento de bateria de testes incluso: [TESTS.md](TESTS.md). Ele contém uma gama de amostras cruas (divididas por taxa de confiabilidade) com as quais o comportamento do parser e dos manipuladores de falhas e corrupção de memória (buffer incompleto) podem ser testados de imediato.

## Como Compilar (Windows)
**Requisitos mínimos:**
- CMake 3.16+
- Visual Studio 2022 (Compilador MSVC)
- Instalação do Qt 6.11.1 (especialmente com dependências do componente *Widgets*)

Via terminal (*Developer PowerShell* recomendado), rode na raiz deste diretório:

```powershell
# 1. Configurar indicando onde o módulo Qt 6 está contido
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"

# 2. Iniciar a compilação de fato
cmake --build build --config Release
```

O aplicativo `MidiUmpAnalyzer.exe` com todos os links MOC e dlls dinâmicas necessárias (via `windeployqt`) será alocado prontamente no diretório `build/Release/`.
