# MIDI 2.0 Workbench Port (C++ / Qt6)

**Versão:** v0.3.0 - Partial UMP Stream parsing

## Status da versão v0.3.0

Este projeto é uma versão independente e portável, desenhada puramente como um **analisador offline estático de pacotes UMP** (Universal MIDI Packet) em C++ e Qt6. O aplicativo aceita entrada manual hexadecimal ou importa arquivos `.txt` contendo pacotes e os processa isoladamente.

### Funcionalidades Suportadas Parcialmente
Atualmente, o parser offline reconhece e descreve os seguintes dados:
- **MIDI 1.0 Channel Voice**
- **MIDI 2.0 Channel Voice básico**
- **SysEx7** como payload bruto
- **SysEx8/MDS** como payload bruto
- **Flex Data** como parcial/payload bruto
- **UMP Stream / Endpoint** parcialmente

### Status Parcialmente Reconhecidos em UMP Stream (MT 0xF)
O formato 0xF possui leitura descritiva parcial para os seguintes Status de UMP Stream parcialmente reconhecidos:
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

### Regras de Extração de Texto e Fragmentação
É importante detalhar que campos textuais longos (como **Endpoint Name**, **Product Instance ID** e **Function Block Name**) operam sob as seguintes limitações estritas:
- Usam exclusivamente uma extração de texto **ASCII filtrada**, varrendo apenas aquele pacote em si.
- **Mensagens fragmentadas não são reconstruídas**.
- **Não existe buffer de remontagem** para ligar nomes divididos entre múltiplos pacotes.
- O **Payload bruto é sempre preservado** na string final sem corrupções para auditoria paralela.
- O parser opera com design estrito **stateless/offline**.

## Limitações (Escopo MVP)
O projeto opera como leitura isolada passiva. Desse modo, reiteramos as seguintes restrições atuais:
- Sem entrada/saída MIDI real por hardware, driver ou cabo virtual.
- Sem MIDI-CI real (negociações ou interrogações dinâmicas).
- Sem Property Exchange.
- Sem USB MIDI 2.0 (classes de protocolo).
- Sem Windows MIDI Services associados.
- Sem respostas automáticas ou reações algorítmicas de host.
- Sem driver nativo embutido.

## Testes Manuais
Toda a auditoria visual deste projeto para simular a chegada dos bits, sem a necessidade de um hardware, está documentada passo a passo usando entradas hexadecimais explícitas em:
[TESTS.md](./TESTS.md)

## Build do Projeto
Requisitos: CMake 3.16+, Qt 6.11.1, Visual Studio 2022.

```powershell
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
cmake --build build --config Release
```
