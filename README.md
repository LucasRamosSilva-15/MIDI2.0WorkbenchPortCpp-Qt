# MIDI 2.0 Workbench Port (C++ / Qt6)

Este projeto é uma versão independente e portável do **MIDI 2.0 Workbench**.
Ele foi projetado como um **analisador offline estático** de pacotes UMP (Universal MIDI Packet) escrito inteiramente em C++ e Qt6.

## Limitações / Escopo do MVP Atual (v0.2.0)

Este projeto opera no mais estrito sentido de análise pericial de dados (Forensic Parsing). Portanto:

1. **Sem Comunicação MIDI Real**: Não há implementação de comunicação USB ativa, Bluetooth MIDI ou cabos DIN.
2. **Sem Transporte OS**: Não há interligação com Windows MIDI Services, CoreMIDI ou ALSA. O app não enxerga os hardwares da máquina.
3. **Sem MIDI-CI ou Property Exchange**: Nenhum *Handshake* ou transação em múltiplas vias (Discoveries) é estabelecida de fato. Nenhuma mensagem do tipo *Property Exchange* é transmitida ativamente. O programa possui comportamento puramente inerte.
4. **Respostas Automáticas Desativadas**: O aplicativo não responde à rede ao identificar eventos de Requisição (Requests). Ele simplesmente relata o que encontrou no pacote.

## Funcionalidades Atuais

O aplicativo aceita **entrada hexadecimal manual** (digitada) ou via importação de um **arquivo `.txt`** contendo pacotes e os processa, um a um.

Atualmente, o parser offline reconhece e descreve com precisão:
- **MIDI 1.0 Channel Voice**
- **MIDI 2.0 Channel Voice (básico)**
- **System Exclusive 7-Bit (SysEx7)** (Extração como Payload bruto e segurança contra dados malformados).
- **System Exclusive 8-Bit (SysEx8 / MDS)** (Extração e encapsulamento em Payload bruto).
- **Flex Data** (Extração como parcial / Payload bruto).

### Suporte a UMP Stream / Endpoint Messages (MT 0xF)
O formato 0xF possui parsing parcial robusto com foco em visibilidade estática:
- **Endpoint Discovery (0x000)** (Mapeado)
- **Endpoint Info Notification (0x001)** (Versão, Static FB, etc)
- **Device Identity Notification (0x002)** (Fabricante, Modelo e Revisão extraídos com máscaras de 7-bits)
- **Stream Configuration Request (0x005)** (Parsing do Protocolo exigido e Jitter Reduction)
- **Stream Configuration Notification (0x006)** (Parsing do Protocolo aceito)
- **Function Block Discovery (0x010)** (Alvos da descoberta mapeados na primeira palavra)
- **Function Block Info Notification (0x011)** (Mapeamento cruzado da Word0 e Word1 para extração de Status Ativo, Direção, Comprimento, etc)
- **Function Block Name Notification (0x012)** (O aplicativo exibe apenas uma visualização **ASCII filtrada** com os caracteres visíveis do pacote atual. Não utiliza buffer de fragmentação nem tenta reconstruir nomes longos divididos entre múltiplos pacotes. O Payload bruto é rigorosamente mantido na íntegra para auditoria visual e segurança).

> **Nota Técnica:** Campos textuais que excedam o tamanho de um único pacote (ex: `Endpoint Name`, `Product Instance ID` e `Function Block Name`) podem aparecer como "Fragmentados", estar incompletos ou serem exibidos parcialmente baseando-se no preenchimento do cabeçalho `Format`. Campos duvidosos continuarão preservados dentro da chave estática "Payload bruto".

## Como Testar
Você pode executar uma auditoria de comportamento completa acompanhando os testes manuais catalogados em:
[TESTS.md](./TESTS.md)

## Como Compilar

Este repositório requer o **CMake 3.16+**, **Qt 6.11.1** e o **Visual Studio 2022**.

1. Crie os arquivos de Build apontando para o seu diretório Qt e para o compilador do VS2022:
```powershell
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.11.1\msvc2022_64"
```

2. Compile e libere o executável final em Release:
```powershell
cmake --build build --config Release
```
O arquivo de execução (`MidiUmpAnalyzer.exe`) estará na pasta `build\Release\`.
