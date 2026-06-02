# Live MIDI to UMP Explanation

Este documento fornece a explicação técnica da mecânica matemática de conversão aplicada na funcionalidade de "UMP Preview" do projeto.

## O que são Bytes MIDI 1.0 Crus?
Protocolos MIDI 1.0 tradicionais enviam fluxos seriais de bytes não alinhados (tipicamente de 1 a 3 bytes por mensagem).
- Exemplo prático de uma nota C4 pressionada no máximo: `90 3C 7F`.
  - `90`: Status byte (Note On, Canal 1)
  - `3C`: Data byte 1 (Nota 60)
  - `7F`: Data byte 2 (Velocidade 127)

## O que é UMP e o Message Type 0x2
UMP (Universal MIDI Packet) é a infraestrutura de pacote do MIDI 2.0. As mensagens são encapsuladas sempre em "words" de 32 bits, garantindo alinhamento na memória (crucial para USB/PCIe).

O Message Type 0x2 (MT 0x2) foi projetado retroativamente para conter uma mensagem MIDI 1.0 Channel Voice intacta dentro de uma word UMP, permitindo que hardware legado e novo convivam no novo protocolo sem perda de informação ou conversões agressivas para a especificação completa de MT 0x4.

## Diferenças Principais
- **MIDI 1.0 bytes**: Fluxo serial de 1 a 3 bytes transmitido convencionalmente por cabos DIN ou drivers USB legados.
- **UMP MIDI 1.0 Channel Voice MT 0x2**: Representação de 32 bits empacotando os mesmos limites técnicos do MIDI 1.0 (velocity de 7 bits) para transporte universal moderno em hardware e rede.
- **UMP MIDI 2.0 Channel Voice MT 0x4**: Representação massiva em pacotes de 64 bits utilizando recursos avançados e atributos de registro em altíssima resolução (velocity em 16 bits), inviáveis de ser inferidos confiavelmente em tempo real apenas ouvindo dados velhos.

## Fórmula Utilizada na Conversão (C++)
Na arquitetura do UMP MT 0x2, uma word de 32 bits segue o layout:
`[4 bits: Message Type] [4 bits: Group] [8 bits: Status] [8 bits: Data1] [8 bits: Data2]`

A conversão no projeto faz um deslocamento de bits (bit shifting) lógico simples para preencher cada byte original da mensagem MIDI 1.0 recebida na word final.
```cpp
word = 0x20000000 
     | (group << 24) 
     | (statusByte << 16) 
     | (data1 << 8) 
     | data2;
```

### Exemplos Representativos de Conversão
- **Note On** (`90 3C 7F`) → `20903C7F`
- **Note Off** (`80 3C 40`) → `20803C40`
- **Control Change** (`B0 07 64`) → `20B00764`
- **Program Change** (`C0 05`) → `20C00500`
  *Observação: Por ter apenas um byte de dados originais (data1), o campo `data2` é preenchido/padded obrigatoriamente com `00`.*
- **Pitch Bend** (`E0 00 40`) → `20E00040`

## O que NÃO é feito no projeto
Para manter a clareza didática perante a especificação da MIDI Association, não executamos as seguintes inferências obscuras:
- **Não há conversão para MIDI 2.0 MT 0x4**: Transmutar um valor 7-bits (0-127) para a riqueza descritiva e 16-bits do modelo MIDI 2.0 traria dados falsos preenchidos com zeros indesejados à simulação. 
- **Não há MIDI-CI nem Property Exchange**: O programa trabalha sem invocar conexões bi-direcionais no hardware.
- **Não há captura de UMP nativo pelo SO**: Nenhuma chamada ao "Windows MIDI Services" é executada para forçar transporte no Kernel USB nesta versão. O programa atua traduzindo bytes puramente na lógica da aplicação.
