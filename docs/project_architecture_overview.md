# Project Architecture Overview

Este documento expõe a separação de escopos e os núcleos do sistema, desenhados para segregar análise estática C++ pura de rotinas arriscadas da tela e threads do RtMidi em Qt.

## Visão Geral dos Módulos Principais

- **Offline UMP Analyzer**: Abstrai as lógicas centrais para validar blocos de hexadecimais puros ou de arquivos texto.
- **UmpParser**: O motor isolado matemático da estrutura anterior. Faz as dissecações rigorosas sem dependências de frameworks gráficos dinâmicos pesados.
- **Live MIDI Monitor**: UI da aplicação e conjunto de widgets que abriga botões, abas e tabelas dedicados a renderizar na tela tudo aquilo gerado pelo trânsito paralelo do instrumento em tempo real.
- **RtMidiInputBackend**: Stub ou Instância geradora que cria filas mutex de abstração conectando à biblioteca C++ de interações puras de sistema (Windows).
- **Midi1LiveDecoder**: Sub-rotina tática C++ orientada a puxar o Status Byte de uma comunicação e classificar o "tipo nativo" (ex: Note On/Off).
- **Midi1ToUmpPreviewConverter**: Processador bitwise projetado a criar Words limpas (MT 0x2) de 32-bits em UMP de cada status Channel Voice encontrado.
- **Session Recording**: Coletor autônomo que acumula de maneira veloz cada mensagem processada internamente (independente de estar bloqueada na UI).
- **Session Summary Report**: Exportador encarregado de processar os Arrays do sistema para gerar um arquivo acadêmico detalhado e numérico da avaliação realizada.

## Fluxo Recomendado de Operação

O diagrama textual a seguir representa o descarregamento das informações do controlador até a visualização sem comprometer a integridade (Segmentation Faults):

```text
[RtMidiInputBackend]
         | (Envia callback em Thread de Áudio de SO)
         v
[MidiRawEvent]
         | (Salvo na QQueue com Lock)
         v
[MainWindow::pollLiveMidi()]
         | (Processamento QTimer seguro puxando da fila no tempo ócio da GUI)
         v
[Midi1LiveDecoder]
         | (Interpreta "Control Change", Canal, Valores brutos)
         v
[Midi1ToUmpPreviewConverter]
         | (Compõe a word 0x20XXXXXX)
         v
[Live Log / UMP Table / Statistics / Recording]
         (Distribui para todos os listeners da interface baseados nos status atuais e filtros de tela)
```

## Por Que o Callback RtMidi Não Atualiza a UI Diretamente?
A manipulação dos widgets de tabelas (`QTableWidget`) deve obrigatoriamente residir na thread inicial (*Main/GUI Thread*) em que a janela do Qt6 foi gerada. Caso o callback do RtMidi, que frequentemente acorda centenas de vezes via *driver timer OS*, tentasse pintar a janela de modo nativo direto, um erro catastrófico por concorrência de acesso estouraria. Foi necessário o acoplamento do *mutex locker* enfileirador de objetos `MidiRawEvent`.

## Uso do QTimer
O componente QTimer fica rodando no *Loop* gráfico. Sua cadência em milissegundos dita quantas vezes o sistema limpa a fila do *mutex* de recebimento e entrega para as tabelas. Esse modelo é conhecido como *polling batch safe* para interfaces em sistemas embarcados e aplicações críticas.

## Separação Estrita (UmpParser.cpp)
O `UmpParser.cpp` não é invocado em nenhum estágio dos fluxos Live MIDI demonstrados no diagrama anterior. Isso garante que a função de validação teórica principal da aplicação permaneça incontaminada contra os gargalos, quebras de bits e latências vindas de testes em laboratório real. A isolação permite total testabilidade.
