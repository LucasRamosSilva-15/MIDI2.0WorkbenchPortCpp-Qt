# Windows MIDI Services Endpoint Enumeration Prototype (v4.15.0)

## Goal
Relatar o sucesso ou falhas arquiteturais do primeiro contato de listagem com a API oficial da Microsoft para o Windows MIDI Services. O objetivo central é extrair metadados básicos (Nome e ID) das portas lógicas do sistema.

## Isolation Strategy
Para garantir a sanidade da ferramenta *MIDI 2.0 Workbench Port* nas suas demonstrações acadêmicas offline, todos os dispositivos listados foram intencionalmente enclausurados dentro do `WindowsMidiServicesEndpointEnumeratorProbe`.

Isso significa que:
1. **Nenhum dispositivo físico listado é injetado na MainWindow.** O usuário não os verá no QComboBox da interface gráfica principal.
2. O parser base legado continua soberano; a Enumeração acontece nos bastidores (em um *sandbox* de memória) e as strings geradas são enviadas unicamente para o log de Diagnóstico (Probe Report).
3. A macro `WINDOWS_MIDI_SERVICES_ENDPOINT_ENUMERATION_ENABLED` (via CMake) é obrigatória para que a listagem ocorra.

## Next Steps
A base da listagem foi selada. O próximo estágio lógico será avançar para a **v4.16.0** (Windows MIDI Services endpoint metadata research), onde estudaremos os metadados internos de cada porta descoberta, extraindo não apenas Nomes, mas propriedades como UMP Formats, Fabricante e suporte nativo ao MIDI 2.0.
