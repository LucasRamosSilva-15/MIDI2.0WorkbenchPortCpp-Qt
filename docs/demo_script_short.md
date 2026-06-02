# Roteiro de Demonstração (Demo Script) - Duração aprox. 3~5 min

## 1. Introdução (30s)
*"Bom dia/boa tarde a todos os membros da banca. Meu nome é [Seu Nome] e irei demonstrar o software MIDI 2.0 Workbench Port, desenvolvido como parte essencial deste Trabalho de Conclusão de Curso."*

## 2. A Problemática (30s)
*"Com a chegada do padrão MIDI 2.0 (UMP), os pacotes deixaram de ser bytes soltos para assumirem 'Words' fixas de 32 a 128 bits. A transição técnica impõe que todo instrumento antigo precisará ser reinterpretado em um pacote moderno. Meu projeto isola esse motor de leitura e exibe empiricamente como isso funciona."*

## 3. Demonstração Offline Analyzer (1 min)
*"Primeiramente, na aba 'Offline Analyzer', temos o coração matemático isolado do projeto. Clicando em 'Load Examples', vou carregar o pacote do TCC.*
*(Mostra a string e clica em Interpret).*
*Vejam a tabela: As strings puras de 32-bits não são mais jargões matemáticos; o motor identifica com segurança que representam os dados limpos de pacotes 'Message Type 0x2', evidenciando o status e os dados transportados."*

## 4. Demonstração Live & Conversão MT 0x2 (1.5 min)
*"A teoria foi consolidada no passo anterior. Agora, na aba 'Live MIDI Monitor', eu ativo a interceptação física real via RtMidi.*
*(Seleciona a porta, abre e pressiona algumas teclas no teclado físico).*
*Conforme eu toco as notas físicas, à nossa esquerda o decoder apresenta o byte legado recebido (ex: `90 3C 7F`). Simultanemanete, à nossa direita, na 'Live UMP Preview Table', essa comunicação escassa ganha estofamento arquitetural por meio de bit-shifting e preenche exatamente 32 bits equivalentes ao UMP `Message Type 0x2`.*
*É importante destacar que **não** estou gerando dados 'MIDI 2.0' (alta resolução, MT 0x4), mas sim a prova-de-conceito de como um Host moderno encapsula perfeitamente equipamentos jurássicos na nova esteira do Universal MIDI Packet."*

## 5. Limitações e Session Recording (1 min)
*"Se eu tocar dezenas de teclas bruscamente, a placa gráfica do sistema congelaria. Para tanto, o limite na tela é cortado em 1000 linhas. Para não perder evidências laboratoriais reais, vou clicar em 'Iniciar Gravação' e tocar um acorde longo.*
*(Toca).*
*Esse trânsito invisível fluiu direto para a RAM isolada.*
*(Clica em Parar e Exportar).*
*Ao extrairmos este TXT final de Sumário de Sessão, as limitações empíricas ficam claras: Os relatórios detalham perfeitamente que System Exclusives ficaram de fora do UMP Preview com exatidão e evidenciam nossa métrica."*

## 6. Fechamento (30s)
*"A arquitetura construída não corrompeu os módulos C++ clássicos para encaixar threads dinâmicas em Qt6, atestando uma suíte robusta e versátil de tradução teórica para desenvolvedores. Muito obrigado, abro agora a palavra aos avaliadores."*
