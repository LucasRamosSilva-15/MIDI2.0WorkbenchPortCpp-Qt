# Windows MIDI Services API Surface Mapping (v4.14.0)

## Goal
Mapear arquiteturalmente as APIs da Microsoft para a futura implementação de um enumerador de Hardware e Backends no *MIDI 2.0 Workbench Port*. Este documento explana a teoria de uso das classes essenciais sem atrelar a aplicação principal ao ecossistema real (Runtime).

## Key Classes Mapped
O pacote `Microsoft.Windows.Devices.Midi2` expõe um conjunto central de objetos essenciais para se estabelecer o fluxo de dados. Nossa arquitetura rastreará as seguintes referências:

* **`MidiSession`**: A sessão representa o ciclo de vida do cliente (aplicação) perante o sistema operativo de áudio do Windows. Toda conexão precisa de uma sessão viva.
* **`MidiEndpointConnection`**: O canal lógico para onde mensagens UMP são despachadas ou recebidas de um dispositivo físico/virtual específico (Endpoint).
* **`MidiMessageReceivedEventArgs`**: O "pacote de correio" que as callbacks nativas entregarão para nossa aplicação contendo os buffers UMP de 32, 64, 96 ou 128 bits.

## Lifecycle (A Trilha Oficial MS)
De acordo com a engenharia da Microsoft, para obtermos UMP válido através das *Projections* C++/WinRT, o fluxo arquitetural que construiremos nas próximas versões seguirá este roteiro estrito:

1. **Init Apartment**: Inicialização da thread COM/MTA (`winrt::init_apartment`).
2. **Create Session**: `MidiSession::CreateSession(L"Nome do App")`.
3. **Enum Endpoints**: Coletar os Ids das Portas usando a API de enumeração de periféricos do Windows.
4. **Connect**: Estabelecer a `MidiEndpointConnection` anexando a ela o manipulador de *MessageReceived*.
5. **Receive UMP**: A thread de áudio despeja blocos binários; nós despacharemos para o `UmpParser`.
6. **Close/Dispose**: Limpeza rigorosa do COM ao fechar a janela.

### Disclaimer da v4.14.0
**A versão v4.14.0 não executa esse ciclo.** Ela serve unicamente para declarar que o compilador C++ e a arquitetura CMake foram parametrizados para *enxergar* os tipos e validar o mapeamento teórico. Nenhum Enumerador físico ou captura real foi codificada até o momento.
