# Test Plan

O projeto MIDI 2.0 Workbench Port mantém alta cobertura de código por meio do isolamento de lógicas principais em componentes unitários que podem ser invocados automaticamente através de automações PowerShell (via CTest / console).

## Testes Automatizados C++ (`run_tests.ps1`)
Todos os testes são compilados de forma independente da interface gráfica sob o target test runner `UmpParserTests`. A invocação é feita através da chamada `powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1`.

- **Testes do UmpParser**: Verificação estática e forense contra caracteres de quebra, bytes insuficientes, blocos quebrados e validação lógica dos message types vitais (`MT 0x2`, `MT 0x4`, `SysEx7`, `SysEx8`, e propriedades de descobrimento `UMP Stream`). *Nota técnica (Auditoria M2-104-UM): Pendente a inclusão do Message Type 0x1 (System Common/Real-Time).*
- **Testes do Midi1LiveDecoder**: Garantia de assertividade do string output. Avalia se bytes de Status limitados ou com limites dinâmicos de dados 1 (como Velocities em Note Off gerados por `90 3C 00` vel=0) são validados firmemente pelas lógicas modulares.
- **Testes do Midi1ToUmpPreviewConverter**: Cobertura profunda dos cálculos lógicos e bitwise. Comprova que inserções simuladas geram as Words finais exatas para leitura sem deslocamentos ilegais na memória (incluindo *padding* de zeros para Program Change e Pitch Bend, conforme M2-104-UM 4.1).
- **Testes do FakeUmpInputBackend**: Certifica que a simulação de pooling experimental provê pacotes estáveis (MT 0x2 artificiais) sem atrelar APIs irreais ou criar ilusão de MIDI 2.0 de hardware.

### Exemplos Esperados de UMP Preview em TDD:
- **Note On:** Byte original (`90 3C 7F`) → Resultado (`20903C7F`)
- **Note Off:** Byte original (`80 3C 40`) → Resultado (`20803C40`)
- **Control Change:** Byte original (`B0 07 64`) → Resultado (`20B00764`)
- **Program Change:** Byte original (`C0 05`) → Resultado (`20C00500`)
- **Pitch Bend:** Byte original (`E0 00 40`) → Resultado (`20E00040`)

## Testes Manuais de GUI (Qualidade Visual)
Checklist prático de validação a ser realizado por testador QA em instâncias compiladas Windows `vX.Y.Z-windows-x64`:

### Executável Normal
- [ ] O projeto abre instantaneamente sem travar (nenhuma necessidade do `rtmidi.dll`).
- [ ] O "Sample Loader" preenche a barra hexadecimal automaticamente.
- [ ] "Interpret" atualiza a matriz principal visivelmente e relata sucesso na contagem.
- [ ] "Export CSV/TXT" invocam o salvamento mantendo filtros WYSIWYG habilitados.

### Executável RtMidi (-EnableRtMidi)
- [ ] Conexões de dispositivos reais no botão `Atualizar portas` varrem efetivamente as capacidades MIDI do computador do usuário host.
- [ ] Teclas tocadas preenchem logs instantaneamente usando o limitador de travamento nativo.
- [ ] Os `Filtros` de GUI funcionam visualmente sem impactar buffers traseiros.
- [ ] A contagem das `Live MIDI Statistics` relata acréscimos numéricos sadios e confiáveis.
- [ ] A conversão paralela via grid do `Live UMP Preview Table` é exibida com consistência a cada *Voice Message*.
- [ ] A suíte de `Session Recording` salva o pacote longo independentemente do limite das 1000 linhas da GUI.
- [ ] O `Session Summary Report` compila um arquivo texto estruturado detalhado da métrica final de conversão da sessão testada.

## Scripts e CI de Pipeline Local
- A automação `package_release.ps1` engatilha todos os compiladores baseados no `-Version` selecionado.
- Os zipfiles gerados preservam limpezas corretas em pastas transitórias.
