# Windows MIDI Services C++/WinRT Generated Projections Strategy (v4.12.0)

## The Core Problem
O desenvolvimento nativo em Windows C++ muitas vezes requer a travessia de APIs modernas escritas em Rust, C# ou expostas via COM (Component Object Model). Para o **Windows MIDI Services**, a Microsoft empacota essas APIs em arquivos de metadados binários (`.winmd`).

No entanto, o C++ puro não sabe ler ou instanciar classes a partir de um `.winmd`. Ele precisa de **Projeções** (Projections).

Uma Projeção é essencialmente uma biblioteca de Headers C++ puros gerados automaticamente, que encapsulam todo o ritual perigoso do COM `HRESULT` e o traduzem para instâncias familiares como `std::shared_ptr` e *exceptions* modernas. Essa geração é feita pela ferramenta `cppwinrt.exe`.

### O Conflito Observado na v4.10.0
Na versão v4.10.0, o compilador MSVC explodiu um erro letal:
`error C2338: static_assert failed: 'Mismatched C++/WinRT headers.'` e `winrt/impl/... não encontrado.`

Isso ocorreu porque:
1. O Visual Studio traz uma versão nativa do compilador C++/WinRT injetada na Windows SDK padrão (normalmente localizada em `C:\Program Files (x86)\Windows Kits\10\...`).
2. O pacote baixado via `vcpkg` (`Microsoft.Windows.Devices.Midi2`) foi pré-processado com uma versão diferente do `cppwinrt`.

Quando tentamos incluir os Headers do vcpkg, o compilador mesclou os defines nativos do MSVC com os includes locais do pacote, causando o choque de versões de macro `CPPWINRT_VERSION`. Além disso, a ferramenta `cppwinrt` segmenta suas projeções exigindo subpastas chamadas `impl/` que nem sempre vêm distribuídas nos binários estáticos padrão.

## A Estratégia Segura (Diagnostic Safe Mode)

Para mantermos o *MIDI 2.0 Workbench Port* à prova de falhas offline, determinamos que o projeto principal não deve gerar ou compilar suas próprias Projeções C++/WinRT ativamente, a menos que isso seja engatilhado de propósito no CMake.

**Diretriz Diagnóstica Adotada na v4.12.0:**
* O CMake agora procura atestar passivamente se as Projeções já estão alinhadas ou se o ambiente WinRT do SDK foi providenciado, reportando os achados na UI do Probe, sem executar compilação agressiva de `winrt/impl`.
* Mantemos o flag de perigo da v4.11.0 (`ENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT`), pois é ele quem "pula no abismo" tentando a compilação do header sujo.

## Próximos Passos (v4.13.0+)
A próxima etapa (`v4.13.0`) será testar uma inicialização mínima das fundações de runtime (`winrt::init_apartment`) em um Probe limpo. Somente se o ambiente conseguir orquestrar os apartamentos COM sem estilhaçar a thread do Qt6, é que avançaremos para tentar referenciar e instanciar os objetos do pacote MIDI real.
