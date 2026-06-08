# Experimental UMP Backend Demo Guide

Este guia é voltado para professores e avaliadores acompanhando a demonstração do analisador. Ele instrui passo a passo como operar e comprovar o funcionamento da linha de pesquisa `v3.x`.

## 1. Objetivo da Demonstração
Mostrar a capacidade da arquitetura do aplicativo em aceitar, tabular, extrair resumos matemáticos e exportar dados padronizados no formato UMP (Universal MIDI Packet) em "tempo real", validando o *pipeline* da interface gráfica antes mesmo de existirem teclados MIDI 2.0 acessíveis nativamente no laboratório.

## 2. Como Operar a Ferramenta

1. **Abrir a Interface:** Clique na aba superior chamada **"Live MIDI Monitor"** (e localize a seção inferior esquerda) ou observe a integração global.
2. **Atualizar Portas:** Clique em **"Atualizar portas fake"**. O aplicativo populará o *combobox* simulando a varredura de S.O.
3. **Abrir a Porta:** Selecione a *Fake UMP Port 1* e pressione **"Abrir Fake Port"**.
4. **Iniciar o Polling (Leitura):** Pressione **"Iniciar Fake Polling"**.
   - *O que acontece:* A interface acorda e um *Timer* C++ começará a consultar o *Backend* falso a cada segundo, trazendo rajadas injetadas de matrizes hexadecimais brutas (MT 0x2, MT 0x4 e *Timing Clocks*).
5. **Observar Eventos na Tabela:** As palavras UMP brotarão estruturadas visualmente na tabela *Experimental UMP Backend*, provando que o parser matemático desmembrou `Message Type`, `Group`, `Status` e `Channel` na hora.
6. **Gravação Pericial de Sessão:** Pressione **"Iniciar Gravação UMP"**. O status visual indicará "Gravando (Vermelho)".
   - *Explicação:* O painel passará a registrar todo pacote interceptado em uma memória vetorial invisível (RAM profunda), provando que o aplicativo resiste a perda de dados da tabela UI limitante.
7. **Pausa e Exportações (WYSIWYG):** 
   - Pressione **"Exportar Tabela TXT"** ou **CSV**. Isso extrairá apenas os pacotes que os seus olhos alcançam na grade *QTableWidget*.
8. **Exportação da Sessão Bruta:** No painel lateral, aperte **"Exportar Sessão CSV"** para despejar fielmente 100% dos eventos isolados desde o botão iniciar.
9. **Exportar Resumo Acadêmico:** Clique em **"Exportar Resumo da Sessão UMP"**. Um relatório legível com a soma de contagens e os bytes representativos de cada evento (*Representative Examples*) será gerado de forma atômica.

## 3. O Que Mostrar para a Banca Acadêmica
- A imunidade do projeto frente a caracteres estranhos ou conexões nulas.
- A riqueza das colunas sendo decompostas sem travamento em thread.
- O formato do arquivo de relatório `TXT`, ressaltando o detalhismo dos bytes (`UMP Words`) convertidos nos textos compreensíveis no *Representative Example*.

## 4. O Que NÃO Prometer
- **Não prometa** que este software conecta-se via USB e recebe pacotes reais UMP de hardwares do mercado nesta exata compilação.
- **Não garanta** auto-descoberta total (*Property Exchange / MIDI-CI*) pois dependem de canais de comunicação bidirecional ainda não homologados no motor `Fake`.
