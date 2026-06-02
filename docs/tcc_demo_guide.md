# TCC Demo Guide

Este guia apresenta um roteiro de demonstração prática para a apresentação de Trabalho de Conclusão de Curso (TCC) sobre o projeto MIDI 2.0 Workbench Port.

## Objetivo da Demonstração
Mostrar na prática o funcionamento do motor de análise offline (Offline UMP Analyzer) e a captura experimental em tempo real de mensagens MIDI 1.0 com prévia em formato UMP.

## Preparação do Ambiente e Diferença de Builds
O projeto disponibiliza dois executáveis:
- **Build Normal (`MidiUmpAnalyzer-vX.Y.Z-windows-x64.zip`)**: Versão focada exclusivamente na análise offline de registros hexadecimais de pacotes UMP (Universal MIDI Packet).
- **Build Experimental RtMidi (`MidiUmpAnalyzer-vX.Y.Z-windows-x64-rtmidi.zip`)**: Além da análise offline, contém o backend RtMidi ativado para abrir portas locais e interceptar hardware MIDI 1.0 real via SO. 

*Recomendação para a banca*: Inicie demonstrando o build normal para provar a isolação e capacidade forense offline, e em seguida mostre a build `-rtmidi` interagindo com um teclado físico.

## Roteiro de Demonstração

### 1. Offline UMP Analyzer
- Abra a aplicação na primeira aba: **Offline UMP Analyzer**.
- Use o botão **Load Examples** para carregar pacotes pré-cadastrados ou cole blocos hexadecimais de especificações UMP (como Message Type 0x2, 0x4, SysEx, etc.).
- Clique em **Interpret** e mostre a decodificação matemática realizada no grid.

### 2. Live MIDI Monitor
- Mude para a aba **Live MIDI Monitor**.

#### Configuração de Porta
- Clique em **Atualizar portas MIDI** para povoar o combobox com hardwares conectados no sistema.
- Selecione uma porta e clique em **Abrir porta**. O log de diagnósticos abaixo mostrará a conexão.
- *Nota técnica para a banca*: O driver RtMidi intercepta do sistema mensagens MIDI 1.0 (série de bytes crus), *não* pacotes UMP nativos.

#### Tocando Notas
- Pressione teclas no hardware MIDI.
- Observe o **Live MIDI Log**: Mensagens recebidas são decodificadas nativamente (ex: `Note On`, `Control Change`) mostrando o timestamp relativo, os bytes e o descritivo.
- Observe a **Live UMP Preview Table**: Mensagens do tipo Channel Voice são convertidas experimentalmente para o equivalente Universal MIDI Packet (UMP).
- Explique para a banca: O preview constrói uma "word" UMP (Message Type 0x2), preservando a semântica MIDI 1.0 dentro de um pacote de 32 bits de acordo com a especificação do USB MIDI 2.0, e **não** MIDI 2.0 Channel Voice (MT 0x4).

#### Live MIDI Statistics
- Demonstre a contabilidade na aba inferior, mostrando o total de mensagens, divisões por tipo, e filtros visíveis aplicados no momento.

### 3. Session Recording
- Para demonstrar gravação contínua:
  1. Clique em **Iniciar Gravação**.
  2. Toque acordes, modulações (Pitch Bend) e solte (Note Off).
  3. Pause a visualização (botão **Pausar Monitor**). Continue tocando e mostre que a sessão gravada não depende da renderização visual.
  4. Clique em **Parar Gravação**.
  5. Clique em **Exportar Resumo da Sessão**. Abra o arquivo de log exportado em um editor de texto (TXT) e apresente para a banca o sumário matemático da sessão (distribuição por canal, tipos de eventos e UMP preview gerado).

## Sequência Recomendada
1. Contextualização: Mostrar o que é UMP usando o *Offline Analyzer*.
2. Captura Física: Abrir a porta via RtMidi.
3. Demonstração Tátil: Tocar o teclado observando o log subir e as tabelas atualizarem.
4. Explicação Arquitetural: Frizar a limitação imposta de MT 0x2 para *Live MIDI*.
5. Gravação de Dados: Gravar uma sessão e expor os relatórios gerados.
