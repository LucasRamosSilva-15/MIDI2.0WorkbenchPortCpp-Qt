# TCC Defense Questions (Q&A)

Este arquivo abriga uma pré-arguição de possíveis investidas técnicas por parte da banca avaliadora durante a defesa acadêmica.

---

**P: “Isso já captura pacotes via Hardware de MIDI 2.0 real?”**
R: Não. A demonstração de recepção utiliza o `FakeUmpInputBackend`, um simulador que provê o *stress* logístico para estourar o limite das rotinas de C++ da ferramenta, já que hardwares MIDI 2.0 acessíveis no Brasil ainda são insustentavelmente raros/caros no presente momento da pesquisa.

**P: “O que o FakeUmpInputBackend de fato prova na sua pesquisa?”**
R: Ele prova a solidez polimórfica e algorítmica da Interface C++ (`IUmpInputBackend`). Ele garante que, independentemente da injeção ser real (via S.O) ou emulada, a separação de *Thread*, o parser que decodifica campos do array binário e as exportações atuam de maneira 100% segura contra corrupção (Memory Leaks).

**P: “Qual a diferença entre o Message Type 0x2 e o 0x4 abordados na ferramenta?”**
R: O MT 0x2 aloja as clássicas *Channel Voice Messages* baseadas no MIDI 1.0 legado dentro de um pacote genérico restrito a 32-bits. O MT 0x4 representa o ápice expansivo (MIDI 2.0 *High-Resolution*), injetando 64-bits com novos atritubos como Per-Note Pitch Bend contínuo e Velocity decimal que destroem os velhos limitadores da década de 80.

**P: “Por que utilizar a biblioteca RtMidi não captura o fluxo UMP real no Live Monitor?”**
R: A biblioteca `RtMidi` é fantástica em abstrair conexões CoreMIDI, ALSA e MMSystem, porém seu arcabouço *legacy* repassa unicamente bytes segmentados (8-bits textuais de dados crus MIDI 1.0), tornando impossível extrair as rotinas estruturais completas de uma palavra atômica UMP sem amputação de sinal.

**P: “Por que você não implementou o descobrimento de perfis (MIDI-CI)?”**
R: A premissa do escopo MVP offline repousa na dissecção forense. O MIDI-CI (*Property Exchange*) requer uma comunicação contínua Host <-> Device (SysEx bi-direcionais). Sendo um trabalho de análise passiva temporal sem a conectividade de *driver de kernel* finalizada, emular negociações bi-laterais fugiria ao propósito estático isolado.

**P: “Qual é, no seu entender, o próximo salto técnico real?”**
R: Refatorar o braço de captura implementando APIs experimentais como o recém nascido `Windows MIDI Services` no Windows, construindo os gatilhos virtuais (`IMidiInputBackend`) que a versão `v3.8.0` graciosamente cimentou e isolou para o sucesso da ponte.

**P: “Como essa ferramenta construída contribui em geral para o TCC?”**
R: Ela desmistifica as barreiras lógicas do Universal MIDI Packet perante microcontroladores. Servirá para alunos e hobbistas avaliarem passivamente e estaticamente o tráfego das suas implementações de firmware.

**P: “O seu código nativo C++ consome dependências pesadas de IA na retaguarda?”**
R: O analisador foi desenvolvido de forma autossuficiente e atômica. Nenhuma *library* de decodificação ou validação repousa em nuvens externas ou IAs; toda bitwise-logic e decodificações são C++ isolado, podendo ser rodado no deserto.

**P: “O que afinal foi massivamente testado pela esteira contínua (CTest)?”**
R: Os motores lógicos (parsers e desmembradores numéricos) e conversores. Cargas excessivas com bits de sujeira (*garbage collection*), palavras fracionadas mal formatadas, blocos com falta de bytes nativos (MT 0x2 sendo inserido como MT 0x4) e injeções sintéticas controladas (*FakeUmpInputBackend*).

**P: “O que não foi coberto por testes unitários nativos?”**
R: Exportações físicas do IO local em File Systems dependentes da UI, renderização visual do *Qt6* (Grid de tabelas), e MT 0x1 (*System Real-Time*) na extração RtMidi 1.0 direta. Estas são homologações estritamente manuais.
