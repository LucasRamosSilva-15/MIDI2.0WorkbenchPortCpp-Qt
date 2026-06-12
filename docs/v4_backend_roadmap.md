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

## v4.3.0
Windows MIDI Services SDK build experiment.
- Avaliação da linkagem verdadeira no MSVC contra a suíte de cabeçalhos de Kernel (`<winrt/Windows...`) sem estourar dependências quebra-galho.

## v4.4.0
Endpoint listing prototype with SDK.
- Retorno dos verdadeiros hardwares pela malha C++. Funcionalidade opcional travada pelo CMake.

## v4.5.0
UI backend selector.
- Design gráfico aprimorado, permitindo o desenvolvedor comutar entre o "Teste Didático" (FakeUmpInputBackend) ou a varredura por hardware (WindowsMidiServicesBackend) pela interface QWidget, caso a API de leitura se mostre blindada.

## v4.6.0
Experimental UMP receive prototype.
- Receber pacotes reais injetando eventos físicos na *Thread* de callback. Avaliação final com periférico UMP validado.
