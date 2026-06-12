# Windows MIDI Services Feasibility Notes

## What it is
O Windows MIDI Services é o novo stack de áudio, *API* e *SDK* construído do zero pela Microsoft para suceder o legado Windows MMSystem. Ele viabiliza transporte em nanosegundos simultâneo para equipamentos MIDI 1.0 antigos e lida de forma nativa e ininterrupta com toda a especificação UMP (Universal MIDI Packet) 2.0.

## Why it matters
- Ele permite uma via real (kernel/USB/Bluetooth) para recepcionar pacotes brutos do MIDI 2.0.
- Servirá como o motor matriz que implementará as virtuais instâncias do nosso `IUmpInputBackend`, dividindo espaço dinamicamente com o nosso atual `FakeUmpInputBackend`.

## What must be investigated
Para que a orquestração do C++ abrace a API da Microsoft sem causar explosões no ambiente do desenvolvedor, algumas pesquisas urgentes englobam:
1. Qual a metodologia exata de linkagem (`find_package`, MSBuild) ou dependência de Runtime nativo (Windows 11)?
2. Como lidar com ponteiros COM e bibliotecas WinRT nativas cruzando a fronteira de ponteiros C++ estritos.
3. Métodos em chamadas seguras para listar Endpoints do sistema local.
4. Desvendar a interrupção que joga a palavra UMP na variável da *Session*.
5. Construir a "tradução", movendo um evento do driver Microsoft para a nossa `struct UmpRawEvent`.

## Proposed integration path
1. **[v4.1.0 - Feito]** Criar um esqueleto atômico chamado `WindowsMidiServicesBackend` preenchendo as promessas da *interface* `IUmpInputBackend`.
2. **[v4.1.0 - Feito]** Proteger as chamadas do CMakeLists.txt com `ENABLE_WINDOWS_MIDI_SERVICES=OFF`.
3. **[v4.1.0 - Feito]** A compilação é estéril. Não quebra o build legível do MSVC purista.
4. **Primeiro Marco Futuro (v4.2.0):** Listar o nome de um endpoint UMP conectado no dropdown (ComboBox).
5. **Segundo Marco Futuro:** Garantir acesso de hardware a porta (Open endpoint).
6. **Terceiro Marco Futuro:** Estourar um pacote MT 0x4 UMP advindo do Windows no console C++.
7. **Quarto Marco Futuro:** Enviar esse pacote para o canal da UI Experimental nativa do Workbench para validação visual pericial.

## State in v4.1.0
A versão v4.1.0 instanciou o **Skeleton**, consolidando arquiteturalmente a árvore. Não obstante, o projeto recusa a captura real até segunda ordem. Ela está bloqueada contra:
- Headers da Microsoft (`<winrt/...>`).
- Bibliotecas do Windows SDK embutidas no sistema.
- Listagem real e captura física de pacotes UMP interativos.
- Nenhum código fonte operando APIs Windows de última geração.
