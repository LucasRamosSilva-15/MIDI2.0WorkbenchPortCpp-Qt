# Windows MIDI Services UI Integration Research (v4.20.0)

## Goal
Planejar arquiteturalmente como a camada de visualização (`MainWindow`) e o controlador lógico (`MidiInputController`) transitarão ativamente de um backend simulado (`FakeUmpInputBackend`) ou estrito legado (`RtMidi`) para a malha nativa (`WindowsMidiServicesBackend`).

## Hot-Swap Architecture
A mecânica de troca "a quente" (hot-swap) entre backends exigirá um manuseio cuidadoso de ponteiros inteligentes (`std::unique_ptr<IUmpInputBackend>`). 

O ciclo seguro planejado é:
1. **Pausa Visual:** A thread do QTimer (que chama o `pollEvents`) na `MainWindow` deve ser pausada temporariamente para evitar falhas de concorrência.
2. **Destruição Controlada:** O backend atual será fechado (chamando `closeInputPort()`) antes de ter seu ponteiro reiniciado.
3. **Instanciação (Orquestração):** O `WindowsMidiServicesBackend` é alocado no heap local e atribuído ao ponteiro unificado de *Input*.
4. **Acoplamento Frio:** Tenta-se abrir a porta no WinRT (`openInputPort()`).
5. **Retomada:** Caso a porta seja aberta com sucesso, a leitura UMP da UI (`QTimer`) volta a rodar e consome passivamente a fila FIFO do backend nativo através do contrato `pollUmpEvents()`. Em caso de erro, a UI reage, exibe a string de `getLastError()` e retorna (fallback) para o modo *Fake UMP* de segurança.

## UI Implications
Futuramente, a ativação visual exigirá:
- Um seletor visual na interface (QComboBox) listando "Windows MIDI Services (Native)".
- Um bloco de lock (`std::lock_guard`) na área do *Controller* que detém o ponteiro da Interface.
- O modo "Headless" atual da v4.x continua intocado, aguardando essa fusão na v5.0 ou estágios finais desta branch.
