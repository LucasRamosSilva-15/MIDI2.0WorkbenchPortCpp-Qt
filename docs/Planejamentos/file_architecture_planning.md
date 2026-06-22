# File Architecture & Module Planning

Este documento cataloga a topologia técnica do projeto **MIDI2.0WorkbenchPortCpp-Qt**. Ele serve como um mapa de responsabilidades para desenvolvedores e IAs, definindo o escopo de cada arquivo C++ (`.cpp`/`.h`) e dos scripts de automação.

---

## 1. Raiz e Interface de Usuário (Core UI)

A raiz do diretório `src/` coordena a inicialização da aplicação e o front-end principal.

* **`src/main.cpp`**
  * **Responsabilidade:** Entry-point da aplicação (função `main`).
  * **O que faz:** Inicializa a *event loop* do Qt (`QApplication`), injeta os estilos visuais se necessário e lança a instância da `MainWindow`.

* **`src/MainWindow.cpp` & `MainWindow.h`**
  * **Responsabilidade:** Única classe gerenciadora de toda a Interface Gráfica (UI).
  * **O que faz:** Desenha as tabelas de pacotes, manipula botões de play/stop/export, exibe relatórios estáticos, invoca janelas de *About*, e reage (via arquitetura *Signals/Slots* do Qt) aos eventos cuspidos pelos *Backends* de MIDI e UMP para desenhá-los na tela. Também é a responsável por gerar os arquivos de texto `Export` e `Session Summary`.

---

## 2. O Coração Analítico (Parsers e Converters)

Esta seção lida com decodificação e processamento brutal de pacotes. Nenhuma classe aqui deve depender de bibliotecas gráficas ou de hardwares diretamente.

* **`src/UmpParser.cpp` & `UmpParser.h`**
  * **Responsabilidade:** Decodificação forense de Universal MIDI Packets (UMP).
  * **O que faz:** A peça mais importante do projeto. Recebe inteiros de 32 bits (*Words*), desmembra através de bitmasks, e categoriza tudo em Message Types (Utility, System Real Time, MIDI 1.0/2.0 Channel Voice, Data, SysEx 7/8, Endpoint Messages). Retorna strings altamente formatadas para a interface.

* **`src/midi/Midi1LiveDecoder.cpp` & `Midi1LiveDecoder.h`**
  * **Responsabilidade:** Interpretar mensagens puras de MIDI 1.0 (legadas).
  * **O que faz:** Recebe arrays de bytes capturados pelos backends antigos e traduz o *Status Byte* (Note On, CC, Pitch Bend), decifrando os valores em canais 1 a 16 para exibição nas tabelas da GUI.

* **`src/midi/Midi1ToUmpPreviewConverter.cpp` & `Midi1ToUmpPreviewConverter.h`**
  * **Responsabilidade:** Demonstrar uma conversão didática MT 0x2.
  * **O que faz:** Embala mensagens legacy (ex: `0x90 0x3C 0x64`) dentro do formato Universal de 32 bits (`Message Type 0x2`). Cria a representação visual de "Como isso ficaria num pacote UMP moderno".

---

## 3. Backends de Entrada (Acesso a Dados/Hardware)

Responsáveis por fornecer o fluxo constante (stream) de mensagens para a aplicação.

* **`src/midi/IUmpInputBackend.h`**
  * **Responsabilidade:** Contrato de Interface abstrata para provedores de UMP.
  * **O que faz:** Exige que classes implementem os métodos `openPort()`, `closePort()` e emita *callbacks* contendo vetores de `uint32_t` (pacotes UMP puros).

* **`src/midi/IMidiInputBackend.h`**
  * **Responsabilidade:** Contrato de Interface para provedores legados de bytes.
  * **O que faz:** Semelhante ao anterior, mas voltado para *arrays* de *unsigned char* (`std::vector<unsigned char>`).

* **`src/midi/RtMidiInputBackend.cpp` & `RtMidiInputBackend.h`**
  * **Responsabilidade:** Provedor oficial da ponte MIDI 1.0 usando API do Windows WinMM.
  * **O que faz:** Lista as portas USB antigas ligadas ao PC (Teclados, Controladoras), abre threads usando a biblioteca `RtMidi`, intercepta o tráfego em tempo real e joga na aplicação. Fica inativo se a flag `ENABLE_RTMIDI` não for ativada no CMake.

* **`src/midi/FakeUmpInputBackend.cpp` & `FakeUmpInputBackend.h`**
  * **Responsabilidade:** Gerador didático e de testes de UMP.
  * **O que faz:** Como ainda não capturamos UMP no sistema, ele finge ser uma placa real. Dispara *timers* jogando dados hardcoded de UMP MT 0x4 e SysEx diretamente nas planilhas, essencial para validar se o app não engasga em sessões longas ou *exports*.

* **`src/midi/WindowsMidiServicesBackend.cpp` & `WindowsMidiServicesBackend.h`**
  * **Responsabilidade:** O futuro motor de alta definição do projeto.
  * **O que faz:** Atualmente é apenas um esqueleto. Abrigará o motor C++/WinRT do `Microsoft.Windows.Devices.Midi2` para caçar verdadeiros Endpoints de 128 Bits pelo Windows 11. Está "dormente" aguardando o fim da saga de pesquisas de Projeção WinRT.

* **`src/midi/MidiInputController.cpp` & `MidiInputController.h`**
  * **Responsabilidade:** Orquestração de Backends.
  * **O que faz:** A `MainWindow` não fala com os Backends diretamente. Este *Controller* atua como uma fachada (*Facade*), gerenciando quem está aberto, fechado e fazendo a ponte de *callbacks* limpos para a aba certa da UI.

---

## 4. Ecossistema Experimental / Probes (Série v4.x)

Classes transitórias usadas única e exclusivamente para testar o terreno sem destruir a Build oficial do App.

* **`src/midi/WindowsMidiServicesSdkProbe.cpp` & `.h`**
  * **Responsabilidade:** O grande "Repórter" do diagnóstico de SDK.
  * **O que faz:** Coleta variáveis pre-processadas inseridas pelo CMake e aglutina num objeto unificado, formatando um *Report* de diagnóstico rico. É ele que diz à UI (e a nós) se o "Include Attempt" ou "Type Reference" está ativo.

* **`src/midi/WindowsMidiServicesHeaderIncludeProbe.cpp` & `.h`**
  * **Responsabilidade:** Forçar o Lexer do compilador a incluir arquivos em branco.
  * **O que faz:** Apenas atesta se a estrutura `#include <...h>` funciona no diretório `vcpkg` alvo.

* **`src/midi/WindowsMidiServicesTypeReferenceProbe.cpp` & `.h`**
  * **Responsabilidade:** Referenciar tipos de fato (Compile Only).
  * **O que faz:** Tenta acessar objetos ou ponteiros teóricos do WinRT. É o componente mais instável da fase experimental (falhou na `v4.10.0` devido a `Mismatched C++/WinRT`). Agora roda em "Safe Mode" por padrão na `v4.11.0`.

---

## 5. Estruturas Básicas (Core Structs)

Tipos básicos e Enums da aplicação.

* **`src/core/MidiRawEvent.h` & `UmpRawEvent.h`**
  * Estruturas base (`struct`) carregando timestamps e pacotes cruzeiros. Facilitam o tráfego dos callbacks entre as threads de hardware e a UI.

* **`src/core/InputSourceType.h`**
  * Enumerações que definem quem é o backend atual (ex: `Offline`, `RtMidi`, `FakeUmp`).

---

## 6. Automações e Build Pipeline (Scripts & Tests)

Os cães de guarda que atestam a qualidade da entrega a cada *commit*.

* **`tests/run_tests.ps1`**
  * **Responsabilidade:** Runner principal do pipeline de testes locais.
  * **O que faz:** Vai até a pasta `build/Release`, caça todos os binários de teste gerados, executa um por um coletando os Return Codes (0 para OK), listando no console o *stdout* e validando se a aplicação tem direito a ser empacotada.

* **`scripts/package_release.ps1`**
  * **Responsabilidade:** Montador final de binários (*Distributor*).
  * **O que faz:** Aceita o parâmetro `-Version vX.Y.Z` e `-EnableRtMidi`. Copia o executável principal (`.exe`), roda a ferramenta `windeployqt` para coletar dinamicamente DLLs nativas do Qt6 do seu Windows, varre a pasta `docs/` empacotando manuais, e condensa tudo num ZIP na pasta `dist/` pronto para subir no GitHub Releases.

* **`tests/UmpParserTests.cpp`**
  * **Responsabilidade:** Validar o cerne forense.
  * **O que faz:** Testes unitários puros testando cada *Bitmask* do parser. Injeta Words falhas, strings longas e pacotes corrompidos para garantir que o UmpParser não dê erro de segmentação.

* **`tests/WindowsMidiServicesBackendTests.cpp` & `.h`**
  * **Responsabilidade:** Validar as proibições do `AGENTS.md`.
  * **O que faz:** Avalia a classe Probe. Verifica asserções como: *A captura está ativada? (Espera: Falso)*; *Endpoint Listing Real? (Espera: Falso)*. Impede que lixo experimental caia na `MainWindow`.

* **`tests/TestUtils.h`**
  * **Responsabilidade:** Mini-framework interno.
  * **O que faz:** Cria a macro `assertTest("Nome", condicao)` que padroniza os retornos coloridos `[PASS]` e `[FAIL]` no console do PowerShell.
