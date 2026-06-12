# v4 Native UMP Backend Roadmap

## v4.0.0
Native UMP backend feasibility research.
(Release atual estritamente bibliográfica. Apenas arquiteturas documentais e notas de transição, sem nenhuma alteração funcional nas matrizes *Fake* e *Live* do TCC.)

## v4.1.0
Windows MIDI Services backend skeleton.
- Cimento arquitetural estruturado em `.h` e `.cpp` na malha de compilação protegido pela CMake flag `ENABLE_WINDOWS_MIDI_SERVICES`.
- Nenhum link efetivo com SDK Microsoft executado nesta etapa; retornando coleções de endpoints vazias (mock de segurança).

## v4.2.0 - Windows MIDI Services endpoint listing research
- Endpoint listing researched and documented na base textual `windows_midi_services_feasibility.md`.
- Esqueleto preparado com um *endpoint query stub* privado puramente virtual/isolado.
- Listagem nativa e real no kernel postergada por segurança.
- UI backend selector postergado.

## v4.3.0 - Windows MIDI Services SDK build experiment
- Added optional SDK experiment flag (`ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT`).
- Added documentation for SDK build path.
- No SDK dependency enabled by default.
- No endpoint listing or capture yet.

## v4.4.0
SDK detection/package integration research.
- Avaliação da linkagem verdadeira no MSVC contra a suíte de cabeçalhos de Kernel (`<winrt/Windows...`) através do SDK oficial.

## v4.5.0
Endpoint listing prototype.
- Retorno dos verdadeiros hardwares pela malha C++. Funcionalidade opcional travada pelo CMake.

## v4.6.0
Backend selector UI.
- Design gráfico aprimorado, permitindo o desenvolvedor comutar entre o "Teste Didático" (FakeUmpInputBackend) ou a varredura por hardware (WindowsMidiServicesBackend) pela interface QWidget, only if endpoint listing is stable.

## v4.7.0
Experimental UMP receive prototype.
- Receber pacotes reais injetando eventos físicos na *Thread* de callback. Avaliação final com periférico UMP validado.
