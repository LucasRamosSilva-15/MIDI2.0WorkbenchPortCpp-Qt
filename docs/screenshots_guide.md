# TCC Screenshots & Visual Demo Guide

Este checklist documenta as imagens e capturas essenciais requeridas para montagem do relatório de TCC e dos slides de apresentação.

### 1. Tela inicial / About Help
- **O que mostrar:** A tela inicial padrão focado na aba *About / Help*.
- **O que falar:** "O MIDI 2.0 Workbench Port tem o escopo primário de abstrair pacotes hexadecimais densos em relatórios humanos, separando de forma estrita o motor passivo do monitor de fluxo."
- **Detalhe a destacar:** Mostrar as explicações textuais listando explicitamente o que a aplicação suporta (MT 0x2) e o que se isenta (MT 0x4).

### 2. Offline UMP Analyzer com sample carregado
- **O que mostrar:** O botão `Load Examples` selecionando `tcc_demo_midi1_to_ump.txt`, preenchendo a caixa de texto bruta sem a tabela processada ainda.
- **O que falar:** "A entrada de dados exige apenas streams seriais puras. Letras, comentários e quebras de espaço são nativamente higienizadas pela aplicação."
- **Detalhe a destacar:** Os comentários textuais `#` mantendo-se coloridos e não bloqueando a matriz.

### 3. Tabela offline UMP preenchida
- **O que mostrar:** Após clicar em *Interpret*.
- **O que falar:** "A tradução paralela matemática e estática garante que os 32 bits de um UMP do tipo MIDI 1.0 (Message Type 0x2) foram perfeitamente decodificados listando Grupos e Status originais."
- **Detalhe a destacar:** Campo numérico indicando que 0 erros foram encontrados, com dados precisos listados na grid lateral.

### 4. Live MIDI Monitor com porta aberta
- **O que mostrar:** O *ComboBox* com uma porta hardware selecionada (ex: Pano MIDI, Interface USB), botão *Abrir Porta* verde.
- **O que falar:** "Aqui deixamos a análise teórica offline para monitorar ao vivo os eventos enviados pelo sistema operacional usando um buffer seguro."
- **Detalhe a destacar:** O rótulo central indicando que o estado da porta está `Aberta` e *Logging* ativo.

### 5. Live MIDI Log recebendo Note On/Note Off
- **O que mostrar:** O painel de texto esquerdo subindo dados como `Note On` e `Note Off`.
- **O que falar:** "O instrumento real legado aciona apenas arrays de 3 bytes (ex: `90 3C 7F`), o Midi1LiveDecoder transforma isso nesse layout claro da tela com estampas milissegundadas."
- **Detalhe a destacar:** As cores ou formatação do Velocity e Note name extraídos com clareza.

### 6. Live UMP Preview Table mostrando 20903C7F
- **O que mostrar:** A janela dividida (*Splitter*) focada na matriz da direita (Live UMP Preview Table).
- **O que falar:** "Neste exato milissegundo, a máquina pegou os 3 bytes nativos e compôs dinamicamente uma UMP Word 0x2 de 32 bits para ser enviada num transporte tipo MIDI 2.0/USB. Por exemplo, `20903C7F`."
- **Detalhe a destacar:** O isolamento e padding perfeito sem dados espúrios de alta resolução, agindo restritamente num frame 0x2.

### 7. Live MIDI Statistics preenchida
- **O que mostrar:** O contador estatístico exibindo algumas dezenas de Note On/Off recebidos e rastreados.
- **O que falar:** "O rastreamento acumula métricas atestando a performance técnica e identificando se há engasgos gráficos versus as mensagens despachadas da interface real."
- **Detalhe a destacar:** Os contadores de último valor e métricas segmentadas (*Recebidas* versus *Exibidas* com filtro ligado).

### 8. Session Recording ativa
- **O que mostrar:** O log acusando *Session Recording iniciada*.
- **O que falar:** "O log de 1000 linhas se perde caso você toque agressivamente, porém, ao ligarmos a Session Recording, a RAM engole toda e qualquer tecla silenciosamente numa lista autônoma e inviolável, independente de termos pausado o gráfico de tela."
- **Detalhe a destacar:** Botão de Parar Gravação piscando/habilitado.

### 9. Session Summary Report exportado em TXT
- **O que mostrar:** A janela nativa do Bloco de Notas contendo o relatório acadêmico de exportação estruturado (com linhas `---` e exemplos capturados dinamicamente).
- **O que falar:** "Ao final da execução, não precisamos salvar logs confusos. O exportador analítico sintetiza os grupos, canais, UMPs suportadas e exemplos concretos, pronto para a publicação em anexo ao TCC."
- **Detalhe a destacar:** Seção de *Representative Examples* exibindo exatamente a primeira nota tocada do teclado físico.

### 10. Pasta dist/ com os dois zips gerados
- **O que mostrar:** Diretório nativo do Windows Explorer mostrando o pacote puro e o pacote com sufixo `-rtmidi`.
- **O que falar:** "O Cloud Build separa perfeitamente quem deseja um pacote hermético e estável para UMP de quem vai brincar num host que puxa dinamicamente a `rtmidi.dll` na máquina."
- **Detalhe a destacar:** Semelhança visual e o selo de versionamento gerado pelos scripts automáticos PowerShell `v2.20.0`.
