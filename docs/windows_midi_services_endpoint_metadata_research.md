# Windows MIDI Services Endpoint Metadata Research (v4.16.0)

## Goal
Aprofundar a extração de propriedades da API da Microsoft. O ecossistema do Windows MIDI Services não expõe apenas "Nomes de Portas", mas sim um rico conjunto de metadados (`DeviceInformation` / PnP Properties) associados a cada `MidiEndpointConnection`. O objetivo desta pesquisa é coletar e estruturar esses dados vitais.

## Key Properties Investigated
Os seguintes metadados são críticos para o nosso analisador forense e estão sendo estudados nesta camada:
* **Transport:** Qual o barramento físico do dispositivo (USB, BLE, PCIe, Virtual, Network)?
* **Manufacturer:** A identidade da placa PnP nativa.
* **Capabilities (Native Format):** Se a porta suporta UMP nativo (MIDI 2.0) ou se trata-se de um dispositivo legado recebendo Byte Stream via Translator do próprio S.O.

## Safe Mode Isolation
Assim como na enumeração estrita, a extração de metadados ocorre isoladamente no `WindowsMidiServicesMetadataProbe`.
- Nenhum dado enriquece a árvore de UI da *MainWindow*.
- Todo o tráfego é logado puramente como *strings* no Console de Diagnóstico e no Probe Report.
- A flag do CMake `WINDOWS_MIDI_SERVICES_METADATA_RESEARCH_ENABLED` atua como barreira de contenção, impedindo a injeção do código nativo nas versões oficiais distribuídas.
