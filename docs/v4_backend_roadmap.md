# v4 Native UMP Backend Roadmap

## v4.0.0
Native UMP backend feasibility research.
(Release atual estritamente bibliográfica. Apenas arquiteturas documentais e notas de transição, sem nenhuma alteração funcional nas matrizes *Fake* e *Live* do TCC.)

## v4.1.0
Windows MIDI Services backend skeleton.
- Estruturação estéril dos arquivos `.h` e `.cpp` com injeção segura por flag de compilação CMake.
- Nenhum link efetivo com SDKs Microsoft ou execução obrigatória nas dependências diárias (mantido OFF por padrão).

## v4.2.0
Windows MIDI Services endpoint listing prototype.
- Prova de conceito funcional (se a máquina host permitir SDK e ferramentas). A janela interativa do C++ deverá rastrear os dispositivos de hardware UMP no computador. O Fallback (`FakeUmpInputBackend`) continuará garantindo suporte para desenvolvedores órfãos do SDK.

## v4.3.0
Experimental UMP receive prototype.
- O coração do projeto: Uma *Callback/Thread* conectando as interrupções Microsoft à nossa ponte atômica, mastigando arrays brutos e encapsulando todos de forma segura contra falhas no motor `UmpRawEvent`.

## v4.4.0
UI backend selector.
- Design gráfico aprimorado, permitindo o desenvolvedor comutar entre o "Teste Didático" (FakeUmpInputBackend) ou a varredura por hardware do mundo real (WindowsMidiServicesBackend) pela interface do QWidget.

## v4.5.0
Documentation and validation against real device.
- Avaliação com periférico validado de altíssima fidelidade, varrendo bugs marginais, documentando reações e confirmando o suporte ao TCC como analisador universal prático e pericial.
