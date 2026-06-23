# Windows MIDI Services Experimental Capture Backend (v4.19.0)

## Goal
Apresentar a primeira implementação funcional de captura ativa via `WindowsMidiServicesBackend`. A arquitetura provada anteriormente na v4.18.0 ganha vida, orquestrando apartamentos COM (WinRT), estabelecendo *Sessions* e interceptando callbacks assíncronos que convertem `MidiMessageReceivedEventArgs` para a nossa fila FIFO baseada em blocos de 32-bits (UMP Words).

## Concurrency and Thread Safety
A API de Serviços MIDI do Windows dispara callbacks em threads nativas do pool do SO. Isso significa que as mensagens chegam fora do contexto da thread principal (GUI) do Qt. Para garantir segurança:
1. O backend possui um buffer isolado (`std::vector<UmpRawEvent> m_eventBuffer`).
2. Todas as leituras e escritas deste buffer são protegidas por um Mutex em `WindowsMidiServicesBackend` (`m_mutex`).
3. O frontend consome os dados apenas através da função contratuais síncrona `pollUmpEvents()`, que drena o FIFO de forma segura.

## Headless Execution
O motor opera isolado do ecossistema legível pelo usuário, provando a captura ativa somente via diagnósticos. Nenhuma dependência cruzada corrompe a estabilidade da `MainWindow`.
