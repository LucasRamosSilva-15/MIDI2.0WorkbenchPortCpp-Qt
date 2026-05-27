# MIDI 2.0 Workbench Port (C++ / Qt6)

**Versão:** v0.7.0 - Robustness & Input Validation

## Status da versão v0.7.0

Este projeto é uma versão independente e portável, desenhada puramente como um **analisador offline estático de pacotes UMP** (Universal MIDI Packet) em C++ e Qt6. O aplicativo aceita entrada manual hexadecimal ou importa arquivos `.txt` contendo pacotes e os processa isoladamente. A versão **v0.7.0** mantém todos os avanços da v0.6.0 e solidifica a arquitetura defensiva do projeto focando em Robustez e Tolerância a Falhas:
- **Resiliência de Arquivos**: O projeto bloqueia arquivos maiores que 1 MB prevenindo travamentos ou estouro de memória no loop primário.
- **Auditoria Léxica Hexadecimal**: Agora a interface rastreia o caractere exato e a posição de invasores de texto escondidos dentro do *payload*, recusando processar chaves sujas.
- **Detecção de Truncamento**: Se os caracteres enviados não fecharem perfeitamente no limite matemático UMP (múltiplos de 8 caracteres/32 bits), o app alerta exatamente quantos caracteres restaram (sobras) como diagnóstico forense.
- **Pacotes Interrompidos**: O Parser agora lida agressivamente com *Message Types* que anunciam, digamos, "4 palavras" no cabeçalho, mas cujos dados físicos enviados são menores, impedindo *crashes* de memória e acusando o truncamento incompleto.
- **Tolerância Branca Segura**: Espaços, quebras de linha e tabs continuam sendo higienizados sem acionar os alarmes, viabilizando recortes brutos de tabelas externas sem atritos.

### Funcionalidades Suportadas Parcialmente
Atualmente, o parser offline reconhece e descreve os seguintes dados:
- **MIDI 1.0 Channel Voice**
- **MIDI 2.0 Channel Voice básico**
- **SysEx7 MT 0x3 com parsing parcial de cabeçalho:** Group, Form/Status, Byte Count e Payload bruto em bytes reais.
- **SysEx8/MDS MT 0x5 com parsing parcial de cabeçalho:** Group, Form/Status, Byte Count, Stream ID em SysEx8 e Payload bruto em bytes reais.
- **Flex Data MT 0xD com parsing parcial de cabeçalho:** Group, Format, Address, Channel, Bank e Status; payload interno preservado como bruto.
- **UMP Stream / Endpoint** parcialmente

### SysEx7 / SysEx8 / Mixed Data Set
A evolução traz o desmembramento técnico da palavra fundamental de cabeçalhos de sistema exclusivo:
- **SysEx7 MT 0x3** é tratado estritamente como mensagem UMP de 64 bits.
- **SysEx8/MDS MT 0x5** é tratado estritamente como mensagem UMP de 128 bits.
- O parser identifica e extrai **Group**, **Form/Status** (Start, Continue, End, Complete, MDS Header, MDS Payload) e **Byte Count** (Número de Bytes válidos).
- **SysEx7** possui no máximo 6 bytes de dados por pacote.
- **SysEx8** possui Stream ID separado e no máximo 13 bytes de dados por pacote.
- **MDS** não usa Stream ID nesse parser e possui até 14 bytes de payload por pacote.
- O Payload bruto é exibido rigorosamente como **bytes reais válidos**, respeitando o limite do Byte Count.
- O parser considera que parte dos bytes de dados pode vir da própria word0, não apenas das words seguintes.
- Não identifica o fabricante.
- Não decodifica o conteúdo interno SysEx.
- Não reconstrói fragmentos de múltiplos pacotes.
- Não usa buffer de estado.
- Mantém incondicionalmente a tag de segurança **parcial/não detalhado**.

### Flex Data MT 0xD
O projeto possui uma mecânica dedicada ao Message Type 0xD, extraindo o cabeçalho de roteamento:
- Extrai **Group**, **Format**, **Address**, **Channel** (quando Address = Channel), **Bank** e **Status**.
- O payload interno é preservado passivamente como bruto.
- Não interpreta subdados (como texto, letras, metadados internos ou tempo).

### Status Parcialmente Reconhecidos em UMP Stream (MT 0xF)
Como consolidado, o formato 0xF mantém a leitura descritiva parcial para os dez formatos fundamentais:
- Endpoint Discovery
- Endpoint Info Notification
- Device Identity Notification
- Endpoint Name Notification
- Product Instance ID Notification
- Stream Configuration Request
- Stream Configuration Notification
- Function Block Discovery
- Function Block Info Notification
- Function Block Name Notification

É importante detalhar que campos textuais longos usam exclusivamente extração de texto *ASCII filtrada*, varrendo apenas aquele pacote em si. A regra universal contínua diz que a fragmentação das mensagens não é reconstruída.

## Limitações Estritas (Escopo MVP)
O projeto opera como leitura forense puramente passiva. Desse modo, reiteramos as restrições:
- Sem MIDI real.
- Sem MIDI-CI real.
- Sem Property Exchange.
- Sem USB MIDI 2.0.
- Sem Windows MIDI Services.
- Sem respostas automáticas.
- Sem driver nativo.
- Sem reconstrução de mensagens fragmentadas.

## Testes Manuais
Toda a auditoria visual deste projeto para simular a chegada dos bits está rigorosamente documentada como uma bateria de testes manual listada passo a passo em:
[TESTS.md](./TESTS.md)
*O arquivo conta com validações recém implementadas:*
- **Teste 23** cobre visualmente o SysEx7.
- **Teste 24** cobre visualmente o SysEx8.
- **Teste 25** cobre visualmente o MDS Header.

## Build do Projeto
Requisitos: CMake 3.16+, Qt 6.11.1, Visual Studio 2022.

```powershell
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```
