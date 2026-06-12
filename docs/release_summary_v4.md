# Release Summary v4.x

A árvore de versões `v4.x` tem como manifesto principal a migração e exploração orgânica rumo à captação em altíssima resolução de protocolos reais UMP 2.0 pelo Sistema Operacional primário (Windows).

## v4.0.0 - Native UMP backend feasibility research
- **Documentation-only feasibility release.** (Não adicionou nem encerrou C++ novo, apenas relatórios prospectivos).
- Investigou massivamente o cenário futuro sobre `Windows MIDI Services`, `ALSA UMP`, `libremidi` e `JUCE` enquanto possíveis motores paralelos de escuta de porta.
- Estabeleceu e elegeu o **Windows MIDI Services** como o candidato primário (Target #1) das próximas inserções de hardware na planta do Windows.
- O software mantém-se cego a capturas de hardware real (Nenhum UMP real backend fora criado).
- Ratifica que o bloco da **v3.x** e seu `FakeUmpInputBackend` permanecem intocáveis, servindo como o pilar estático, sólido e incontestável para fins de demonstração (TCC-ready baseline).
