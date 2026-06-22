# Windows MIDI Services Input Stream Prototype (v4.17.0)

## Goal
Modelar a arquitetura e o fluxo de dados para a recepção de Universal MIDI Packets nativos da API da Microsoft. O objetivo é estabelecer o protótipo lógico do "callback handler" (recebimento assíncrono de eventos) e demonstrar como os dados brutos WinRT serão convertidos em `std::vector<uint32_t>` compatíveis com nosso `UmpParser`.

## Architecture Prototype
O ciclo de vida projetado segue os seguintes passos lógicos (ainda simulados):
1. Cria-se o `MidiEndpointConnection`.
2. Um `Event Token` é gerado para assinar o evento `MessageReceived`.
3. Quando o evento dispara, extrai-se o pacote do `MidiMessageReceivedEventArgs`.
4. Os dados sofrem cast para as Words de 32-bits que a nossa UI exige.

## Isolation and Safety
O protótipo (`WindowsMidiServicesInputStreamProbe`) atua de forma estritamente contida:
- Não consome tráfego real.
- Não encaminha os "pacotes UMP falsos" para a interface gráfica.
- É dependente da flag `WINDOWS_MIDI_SERVICES_INPUT_STREAM_EXPERIMENT_ENABLED` na compilação.
Desta forma, garantimos que a lógica de eventos legada, o `RtMidi` e as exibições no hex viewer permaneçam puros para os usuários finais, protegendo a integridade da demonstração acadêmica.
