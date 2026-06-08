# Experimental UMP Backend Limitations

Para fins de avaliação de auditoria técnica no TCC, as seguintes limitações do módulo *Experimental UMP Backend* da versão `v3.x` estão assumidas e publicamente documentadas:

1. **Hardware Real Não Injetado:** O `FakeUmpInputBackend` não conecta com portas lógicas de Sistema Operacional, USB ou Bluetooth. Apenas fornece hexadecimais controlados sintéticos.
2. **Limites do RtMidi (MIDI 1.0 Legado):** A biblioteca `RtMidi` suporta apenas dados crus de 8-bits, incapaz de empacotar nativamente o escopo `UMP` 32-bits de MT 0x4.
3. **Conversão Unidirecional (Preview UMP):** O modo "UMP Preview" inserido no *Live MIDI Monitor* mapeia *bytes legacy* para UMP de MT 0x2 (MIDI 1.0 em envelope 32-bit), e nunca como MT 0x4 (High-Resolution MIDI 2.0). 
4. **Drivers UMP Não Acoplados:** Toda a via C++ polimórfica (`IUmpInputBackend`) está pronta para acoplar `Windows MIDI Services` (WinRT) ou `ALSA UMP` (Linux), mas ambos continuam sendo alvos de pesquisa futura.
5. **Automação Dispositivos (MIDI-CI):** Negociação de perfis (*Protocol Negotiation*, *Profile Configuration*, *Property Exchange*) não está programada no motor atual.
6. **Estados Massivos (Buffers):** SysEx de 8-bits, pacotes UMP Stream nativos ou requisições Flex Data fragmentadas em dezenas de `Continue/End` pacotes operam de forma isolada, não mantendo sessão cumulativa para reconstrução de strings pesadas inter-pacote.
7. **Finalidade Primária Fake:** A aba experimental, tabela, e *Fake Session* existem precipuamente para testar o **estresse da UI**, garantir *Thread Safety* no **pipeline de polling C++**, validar **algoritmos matemáticos de Parsing** e blindar as arquiteturas de **exportação IO/File**.
