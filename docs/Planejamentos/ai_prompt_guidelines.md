# AI Prompt & Delivery Guidelines

## Objetivo
Este documento padroniza a forma como Agentes de Inteligência Artificial (LLMs) devem se comunicar ao final de cada iteração de desenvolvimento (*sprint* ou task) dentro do projeto **MIDI2.0WorkbenchPortCpp-Qt**.

A padronização garante que a IA respeite estritamente as regras arquiteturais do `AGENTS.md`, comprovando honestidade técnica, execução real de testes e isolamento de features experimentais (ex: Windows MIDI Services).

---

## A Anatomia do "Prompt de Entrega"

A resposta final da IA, após concluir a codificação de uma versão, **deve** obrigatoriamente seguir os 5 pilares abaixo. É proibido omitir qualquer uma dessas seções.

### 1. A Narrativa de Validação (Warm-up Técnico)
A resposta deve iniciar narrando o comportamento prático do código escrito frente aos compiladores (MSVC/CMake). A IA não deve apenas dizer "o código foi feito", mas sim "o código foi testado no `build-wms-sdk-root` e o pre-processador comportou-se da seguinte maneira...".
* **Motivo:** Traz transparência de que a IA validou os cenários localmente através de comandos isolados.

### 2. Resumo de Arquivos (O "Diff" Humano)
Lista exata de quais arquivos foram tocados. Nenhuma alteração pode ser feita sorrateiramente.
* **Criados:** Documentações (`docs/`), Headers ou Sources.
* **Modificados:** Quebras de refatorações base (como `CMakeLists.txt`), justificando brevemente a flag usada.
* **Intocados:** Confirmação explícita de que os parsers base (`UmpParser.cpp`, `FakeUmpInputBackend`, etc.) continuam intactos.

### 3. Checklist de Versionamento
A IA deve confirmar a alteração em massa das strings que exibem a versão atual para o usuário.
* Deve listar: `MainWindow.cpp` (títulos, *About*, Exportadores `.txt`/`.csv`), `README.md` e `CHANGELOG.md`.

### 4. Checklist de Execução & Comandos (A Prova Real)
Conforme ditado pelo `AGENTS.md`, a IA precisa documentar a saída dos scripts obrigatórios.
* **Testes Base:** Resultado do `tests\run_tests.ps1` (ex: *112 / 112 passed*).
* **Pacotes:** Confirmação de geração de `dist\MidiUmpAnalyzer-vX.Y.Z-windows-x64.zip` e sua variante `rtmidi.zip`.
* **Experimental CMake:** Se a task tocar na linha v4.x, a IA deve exibir o output do teste local seguro utilizando o `build-wms-sdk-root`.

### 5. Respeito às Diretivas Absolutas ("O que NÃO foi feito")
Este pilar garante aderência imediata à política restritiva de pesquisas (evita que a IA seja proativa de forma destrutiva). A IA deve terminar seu prompt jurando e atestando as seguintes restrições:
* "Nenhum código experimental invadiu a branch oficial (UI legada)."
* "Nenhum endpoint real foi listado."
* "Nenhuma captura UMP real foi ativada."
* "Nenhum seletor de backend foi adicionado à interface (se não estiver no escopo)."
* "Nenhum `git commit`, `git tag` ou `git push` foi executado em background."

---

## Exemplo de Prompt Homologado (Template)

Ao finalizar a *Task v4.12.0*, por exemplo, a IA deve entregar exatamente este formato:

> **Vitória arquitetural!** A pesquisa de projeções geradas do C++/WinRT foi codificada e devidamente isolada. Executei a malha de testes e garantimos que os `includes` experimentais não quebraram as builds normais do MSVC.
> 
> Aqui está o sumário técnico formal da **v4.12.0**:
> 
> **1. Arquivos Criados/Modificados:**
> * `docs/wms_cppwinrt_generated_projections_strategy.md`: Criado. Disseca os arquivos `winmd` vs `impl/`.
> * `CMakeLists.txt`: Modificado. Adicionada diretiva diagnóstica para WinRT.
> * `src/midi/WindowsMidiServicesSdkProbe.cpp`: Modificado.
> 
> **2. Versionamento:**
> * `src/MainWindow.cpp`, `CHANGELOG.md` e exportadores base foram evoluídos estritamente para **v4.12.0**.
> 
> **3. Execução & Validação:**
> * `tests/run_tests.ps1`: Passou brilhantemente (**115 / 115 OK**).
> * `package_release.ps1` (Base e RtMidi): Selados no `dist/`.
> * **Simulação Experimental**: Teste CMake (`build-wms-sdk-root`) via vcpkg rodou com sucesso no modo seguro restrito.
> 
> **4. Respeito às Diretivas (AGENTS.md):**
> * O `UmpParser` e os módulos `RtMidi` seguem estritamente intactos.
> * **Nenhuma API runtime WinRT foi ativada** e **Nenhum endpoint real** foi enumerado.
> * A ferramenta continua Offline/Standalone, sem obrigar instalação do SDK.
> * Nenhuma ação no Git (`commit`/`tag`/`push`) foi executada.
>
> Aguardando orientações para o próximo passo metodológico!
