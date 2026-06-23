# Project Context & Architectural Memory

Este documento serve como memória externa fundamental para futuras sessões de desenvolvimento (especialmente orientadas a IA), preservando a história, os propósitos e os limites técnicos do **MIDI 2.0 Workbench Port**.

---

## 1. Análise geral do projeto

### Estrutura e Árvore de Diretórios
- **`src/`**: Contém o código-fonte principal da aplicação C++/Qt.
  - **`midi/`**: Concentra o núcleo da lógica MIDI, Backends de entrada, conversores e probes.
- **`tests/`**: Suite de testes unitários isolados (não dependentes da UI).
- **`docs/`**: Memória documental, artefatos de roadmap e guias técnicos de experimentação.
- **`dist/`**: Pasta de destino para empacotamentos gerados pelo script de release.
- **`scripts/`**: Scripts PowerShell (`package_release.ps1`, `run_tests.ps1`) que comandam o build e distribuição.

### Sistema de Build
- **CMake**: Orquestrador primário. Lida com múltiplas matrizes de build opcionais usando _flags_.
- **Visual Studio 2022 (MSVC)**: Compilador nativo Windows e provedor da Toolchain em arquitetura x64.
- **windeployqt**: Garante que o build em "Release" obtenha todas as DLLs dependentes da Qt6 de forma automática e standalone.

### Dependências
- **Qt6 (Widgets/Core/Gui)**: Base do projeto para a UI Gráfica.
- **RtMidi (Opcional - WinMM)**: Fetch nativo via CMake para integrar hardware legado MIDI 1.0.
- **Windows SDK / C++/WinRT (Opcional/Experimental)**: Uso focado exclusivamente nas pesquisas da linha `v4.x` (SDK Microsoft.Windows.Devices.Midi2).
- **Vcpkg**: Ferramenta de pacotes acoplada teoricamente à Toolchain de pesquisa para C++/WinRT.

### Módulos e Fluxo de Dados
A interface gráfica (`MainWindow`) atua primariamente de maneira reativa, escutando dados provenientes de Backends polares, transformando-os em representações visuais ou decodificando seus Hexadecimais utilizando o coração analítico central (`UmpParser` e `Midi1LiveDecoder`).

---

## 2. Identificação do propósito do projeto

* **Nome do Projeto**: MIDI 2.0 UMP Analyzer (MIDI 2.0 Workbench Port)
* **Objetivo Principal**: Ser um analisador estático forense e interpretador profundo de Universal MIDI Packets (UMP) com suporte progressivo para hardware real.
* **Problema que resolve**: A carência de ferramentas leves e diretas que permitam decifrar a semântica de pacotes de 32, 64, 96 ou 128 bits do novo protocolo UMP 2.0 nativamente em Windows.
* **Tecnologias Utilizadas**: C++ 17+, CMake, Qt6, PowerShell, MSVC.
* **Público-alvo**: Desenvolvedores de Hardware Musical, Pesquisadores de Áudio e Integradores de Software Musical que precisam debugar e auditar envios UMP byte a byte.
* **Estado Atual**: UI plenamente estável e em versão madura de features legadas, transitando investigativamente rumo ao motor oficial Microsoft MIDI Services através da arquitetura de pesquisa v4.x.

---

## 3. Arquitetura atual

### Núcleo MIDI
É a zona "sagrada" do projeto, operando com total independência de API de hardware:
- **`UmpParser`**: Decodifica bytes (Words) determinando Message Type (MT), Groups, Endpoints, SysEx 7/8, FlexData e Utility Messages.
- **Estruturas UMP**: Manipula os MTs formatando em strings de representação exata (MT 0x1, 0x2, 0x4).
- **`Midi1LiveDecoder`**: Converte status legados do hardware (Note On, Note Off, CC) para consumo na UI.
- **`Midi1ToUmpPreviewConverter`**: Converte live stream de 1.0 (via `RtMidi`) para pacotes UMP MT 0x2 estáticos didáticos de 32 bits.
- **Exportadores**: Registram relatórios gerados via "Sessões Falsas" e dumps em formato TXT/CSV.

> **Importante:** Estas camadas são provadas por `tests/run_tests.ps1` e são consideradas estritamente ESTÁVEIS. Modificá-las é proibido sem autorização explícita em escopo.

### Backends de entrada
Responsáveis por alimentar a aplicação com dados. Todos implementam `IUmpInputBackend`.

#### `FakeUmpInputBackend`
* **Finalidade**: Teste e simulação didática de hardware.
* **Comportamento**: Dispara temporizadores assíncronos despejando pacotes UMP hardcoded de forma a alimentar as planilhas da GUI.

#### `RtMidiInputBackend`
* **Finalidade**: Interceptar hardware legado real conectado ao Windows.
* **Limitações**: Restrito a eventos MIDI 1.0 (limitação inerente da biblioteca / WinMM).
* **Dependência Opcional**: Só é habilitado via `ENABLE_RTMIDI=ON`.

#### `WindowsMidiServicesBackend`
* **Finalidade/Estado**: Criado inicialmente como skeleton (`v4.1.0`). Atualmente **não captura hardware**, **não lista endpoints** e **não possui integração Runtime**. Toda sua infraestrutura foi talhada para investigações futuras isoladas do SDK C++/WinRT da Microsoft.

---

## 4. Histórico da linha v4.x

Esta linha iniciou a busca pelo backend nativo UMP sem comprometer o repositório legado.

* **v4.1.0 - Windows MIDI Services backend skeleton**:
  Criação do backend vazio (`WindowsMidiServicesBackend.cpp/h`) e interface `IUmpInputBackend` para garantir total isolamento das investigações futuras.
* **v4.2.0 - Windows MIDI Services endpoint listing research**:
  Pesquisa estática e documentação de como endpoints operam na API. Ausência total de chamadas reais de hardware.
* **v4.3.0 - Windows MIDI Services SDK build experiment**:
  Criação de flag de compilação experimental (`ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT`), confirmando ausência do SDK como dependência obrigatória no CMake.
* **v4.4.0 - Windows MIDI Services SDK detection research**:
  Inclusão do C++ `WindowsMidiServicesSdkProbe` emitindo strings de status de detecção no relatório da aplicação em tempo real.
* **v4.5.0 - Optional package detection experiment**:
  Separação absoluta dos testes. O `WindowsMidiServicesBackendTests.cpp` foi criado garantindo blindagem contra testes puramente voltados a parsers.
* **v4.6.0 - User provided SDK root research**:
  Introdução da variável `WINDOWS_MIDI_SERVICES_SDK_ROOT` instruindo o CMake a caçar headers C++ locais para desvincular o projeto de diretórios do sistema global.
* **v4.7.0 - Optional header detection**:
  Pesquisa acurada através do CMake usando `find_path` contido com `NO_DEFAULT_PATH`.
* **v4.8.0 - Header include compile experiment**:
  Inicia o ensaio do C++ em referenciar um header real detectado, mantendo a falha (ou sucesso) totalmente isolada em pastas não rotineiras.
* **v4.9.0 - SDK candidate discovery correction**:
  Afinamento da detecção pelo vcpkg. Diferenciou a busca para incluir `Microsoft.Windows.Devices.Midi2.winmd` e `Microsoft.Windows.Devices.Midi2.h`.
* **v4.10.0 - Type reference compile experiment**:
  Tenta compilar a sintaxe. Descobre-se um erro letal do compilador MSVC: *CPPWINRT_VERSION mismatch* e ausência de headers *winrt/impl/...*.
* **v4.11.0 - C++/WinRT projection alignment research**:
  Converte o probe a um *Safe Mode* por padrão, reportando documentalmente que a dependência estrutural do C++/WinRT bloqueou os includes. Desacopla as tentativas de injetar hardware atrás da flag de perigo iminente.

---

## 5. Estado atual do Windows MIDI Services

| Item                             | Estado   |
| -------------------------------- | -------- |
| Windows MIDI Services instalado  | Sim      |
| midi.exe disponível              | Sim      |
| Endpoint enumeration via console | Funciona |
| SDK vcpkg instalado              | Sim      |
| Captura UMP no projeto           | Não      |
| Backend real                     | Não      |
| UI integrada                     | Não      |

---

## 6. Architecture Rules (Restrições Permanentes)

### Nunca modificar sem motivo ou ordem estrita:
```text
UmpParser.cpp
UmpParser.h
FakeUmpInputBackend
RtMidiInputBackend
Midi1LiveDecoder
Midi1ToUmpPreviewConverter
```

### Não implementar prematuramente:
* captura UMP real;
* endpoint selector (UI Graphica para Windows MIDI Services);
* callbacks WinRT ou chamadas COM/Apartments (`init_apartment`);
* dependência obrigatória do Windows MIDI Services no pacote final;
* alteração destrutiva da UI Principal.

---

## 7. Estratégia de futuras versões

O avanço na integração oficial ocorrerá pela seguinte rota metodológica:
`Pesquisa` → `Probe isolado` → `Teste isolado` → `Integração controlada`

NUNCA execute saltos para `Pesquisa` → `Código Runtime Direto` com hardwares não consolidados.

### v4.12.0 - Windows MIDI Services C++/WinRT generated projection strategy research
* **Objetivo:** Entender e resolver as projeções C++/WinRT (arquivos `impl/`) reportadas na *v4.11.0*.
* Documentar a injeção correta de Toolchains via vcpkg sem ativar Hardwares Reais.

### Roadmap Adicional:
* **v4.13.0:** Isolated runtime initialization research (Apenas após sucesso na projeção).
* **v4.14.0:** Endpoint listing prototype (Apenas após sucesso de runtime).
* **v4.15.0:** Backend Selector UI integration.

---

## 8. Build e testes

**Sistemas de Base:**
* Orquestração: CMake
* IDE/Compilador: Visual Studio 2022 (MSVC 17+)
* Frontend Visual: Qt6

**Matrizes de Builds Existentes:**
1. **Normal (Obrigatória):** CMake default que invoca MSVC + Qt. Deve rodar `run_tests.ps1` passando livremente. Resulta na Release Base.
2. **RtMidi (Obrigatória):** Mesma rotina anterior acompanhada da flag `ENABLE_RTMIDI=ON`. Resolve dependências nativas (WinMM).
3. **Windows MIDI Services Experimental (Apenas Pesquisa):** Executado em diretório isolado (`build-wms-sdk-root`). Aceita Toolchains do vcpkg e valida os "Probes" sem comprometer a esteira de CI ou as Releases limpas dos desenvolvedores.

---

## 9. AI Development Guidelines (Regras para Agentes e LLMs)

Para garantir que IAs operem perfeitamente dentro deste repositório sem destruir ou "alucinar" abstrações legadas, as seguintes regras são imutáveis:

**Antes de modificar qualquer código:**
1. Leia a arquitetura contida neste documento.
2. Verifique estritamente se a mudança pretendida pertence ao ESCOPO da versão requisitada pelo humano.
3. Evite e NEGUE refatorações ou limpezas automáticas não solicitadas (não aplique "clean code" não ordenado nos C++ legados).

**Sempre (Obrigações):**
* Criar e isolar testes separados. Testes nativos MS não devem poluir testes analíticos padrão.
* Atualizar fielmente a documentação (Checklists, Roadmaps e README).
* Preservar a compilação offline. Nenhum pacote pode cair se o computador estiver sem vcpkg, SDK e internet.

**Nunca (Interdições Totais):**
* Executar `git commit`.
* Executar `git push`.
* Criar `tags`.
* Adicionar bibliotecas pesadas (NUGET, JUCE, ALSA, libremidi) ou dependências genéricas sem aprovação estrita de diretrizes do usuário.
