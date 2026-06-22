# Windows MIDI Services Backend Integration Preparation (v4.18.0)

## Goal
Preparar o `WindowsMidiServicesBackend` para assumir o protagonismo do fluxo de dados nas futuras versões, introduzindo uma Máquina de Estados interna robusta. Ele deixa de ser um "Skeleton" vazio e passa a ser um "Orquestrador Dormente", ciente dos ciclos de vida assíncronos do WinRT.

## State Machine Architecture
O backend agora gerencia os seguintes estados operacionais (`ConnectionState`):
- `Disconnected`: Estado inicial. O backend não detém recursos e o WinRT pode estar não-inicializado.
- `Ready`: O ambiente WinRT (Apartments) foi inicializado com sucesso e as dependências estruturais estão carregadas, aguardando um alvo.
- `Active`: Uma Sessão (`MidiSession`) foi estabelecida e uma Porta (`MidiEndpointConnection`) está efetivamente conectada recebendo pacotes via *callbacks*.
- `Error`: Falha severa (ex: falha de alocação COM, hardware desconectado abruptamente).

## Safe Mode Constraints
Para respeitar o *Safe Mode* da v4.x, as conexões reais são curtas-circuitadas intencionalmente se a flag de segurança (`WINDOWS_MIDI_SERVICES_BACKEND_INTEGRATION_PREP_ENABLED`) não for ativada pelo CMake, ou se a compilação for feita sem headers WinRT. As tentativas de `openPort()` no Safe Mode engatilham erros graciosos que preservam o uptime da *MainWindow* e evitam crashes de Runtime.
