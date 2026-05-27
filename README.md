# MIDI 2.0 Workbench Port (C++ / Qt6)

**Versão:** v0.4.0 - Partial Flex Data parsing

## Status da versão v0.4.0

Este projeto é uma versão independente e portável, desenhada puramente como um **analisador offline estático de pacotes UMP** (Universal MIDI Packet) em C++ e Qt6. O aplicativo aceita entrada manual hexadecimal ou importa arquivos `.txt` contendo pacotes e os processa isoladamente. A versão **v0.4.0** mantém todos os avanços da v0.3.0 e adiciona capacidades de *parsing* parcial para o formato Flex Data (MT 0xD).

### Funcionalidades Suportadas Parcialmente
Atualmente, o parser offline reconhece e descreve os seguintes dados:
- **MIDI 1.0 Channel Voice**
- **MIDI 2.0 Channel Voice básico**
- **SysEx7** como payload bruto
- **SysEx8/MDS** como payload bruto
- **Flex Data MT 0xD com parsing parcial de cabeçalho:** Group, Format, Address, Channel, Bank e Status; payload interno preservado como bruto.
- **UMP Stream / Endpoint** parcialmente

### Flex Data MT 0xD
O projeto agora possui uma mecânica dedicada ao Message Type 0xD (Flex Data), que opera sob as seguintes regras de extração diretas:
- O parser identifica as mensagens de Flex Data e valida seu formato restrito de **128 bits**.
- Extrai diretamente da primeira palavra: **Group**.
- Extrai **Format** (Complete, Start, Continue, End).
- Extrai **Address** (Channel, Group, Reserved).
- Extrai **Channel** apenas quando a variável Address indica o respectivo alvo (`Address = Channel`).
- Extrai **Bank** e **Status** em formato Hexadecimal visual.
- **Mantém estritamente as words 1, 2 e 3 como Payload bruto** no final da formatação.
- **Não interpreta** subdados internos dinâmicos como texto, metadados internos, tempo, acordes, letras ou qualquer payload específico embutido no banco de status.
- **Não usa ASCII filtrado** em Flex Data.
- Mantém rigorosamente a tag **parcial/não detalhado** em todas as interações da classe.

### Status Parcialmente Reconhecidos em UMP Stream (MT 0xF)
Como consolidado na v0.3.0, o formato 0xF mantém a leitura descritiva parcial para os seguintes Status de UMP Stream parcialmente reconhecidos:
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

É importante detalhar que campos textuais longos (como Endpoint Name, Product Instance ID e Function Block Name) usam exclusivamente uma extração de texto *ASCII filtrada*, varrendo apenas aquele pacote em si. A regra universal contínua diz que a **fragmentação das mensagens não é reconstruída**, não havendo portanto a criação de buffers de armazenamento ou estados persistentes.

## Limitações Estritas (Escopo MVP)
O projeto opera como leitura forense puramente passiva. Desse modo, reiteramos as restrições arquiteturais vigentes:
- Sem MIDI real (físico, interações com periféricos).
- Sem MIDI-CI real (negociações ou perfis ativos).
- Sem Property Exchange.
- Sem USB MIDI 2.0.
- Sem Windows MIDI Services integrados.
- Sem respostas automáticas programadas de loopback.
- Sem driver nativo anexado.
- Sem reconstrução de mensagens fragmentadas (Stateless Parsing).

## Testes Manuais
Toda a auditoria visual deste projeto para simular a chegada e formatação dos bits, sem a necessidade de hardware físico atrelado, está rigorosamente documentada como uma bateria de testes manual listada passo a passo em:
[TESTS.md](./TESTS.md)
*(Consulte especificamente o **Teste 22**, que cobre com detalhes visuais e práticos o parsing das tabelas e cabeçalhos em Flex Data).*

## Build do Projeto
Requisitos: CMake 3.16+, Qt 6.11.1, Visual Studio 2022.

```powershell
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```
