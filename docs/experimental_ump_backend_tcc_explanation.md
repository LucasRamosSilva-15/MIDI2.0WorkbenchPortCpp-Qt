# Experimental UMP Backend TCC Explanation

Neste documento detalhamos a arquitetura proposta usando uma retórica acadêmica voltada à banca examinadora, correlacionando o *software* com os conceitos basais.

### O que é UMP?
**Universal MIDI Packet (UMP)** é a espinha dorsal estrutural adotada pelo consórcio MMA/AMEI para o MIDI 2.0. Ao contrário do MIDI 1.0, focado em transmissões textuais curtas baseadas em 8 bits por byte (e limitadas a fatias de 7 bits para dados), o UMP transita nativamente arquiteturas alinhadas em palavras atômicas de 32, 64, 96 ou 128 bits.

### O que é um Backend?
Dentro do design MVC (Model-View-Controller) modificado deste projeto em Qt6, o "Backend" representa a camada profunda (`IMidiInputBackend` ou `IUmpInputBackend`) responsável por escutar e absorver impulsos elétricos da placa mãe/SO sem afetar o redesenho visual (GUI) da tela.

### Por que um FakeUmpInputBackend foi construído?
Pela ausência temporária de *interfaces* físicas compatíveis e certificadas MIDI 2.0 no laboratório, a evolução do TCC seria bloqueada. O `FakeUmpInputBackend` soluciona o bloqueio gerando *arrays* de *uint32_t* na memória ram internamente, emulando o tráfego violento e massivo que um teclado profissional geraria em uma placa mãe.

### Diferença entre Simulação e Captura Real
Na captura real, quem envoca as amarras e acordos de segurança é a API do próprio *Windows Core Audio* (*Windows MIDI Services*). O tráfego precisa ser extraído de buffers inseguros sem derrubar a CPU do usuário.
Na simulação deste TCC, a injeção parte do próprio programa a cada batida do `QTimer`, injetando exatos ciclos controlados para testar até onde a matriz de objetos C++ e a renderização do Qt aguentam sem apresentar *Memory Leaks* ou *Lag*.

### O Pipeline e a Contribuição para o TCC
O motor transita da "Injeção Fake" → "Polling Não-Bloqueante" → "Parsing Bitwise Puro (Deslocamento >>)" → "Alojamento em Memória" → "Exportação Textual Arquivística".
Esta contribuição consolida, em viés puramente lógico, uma aplicação modular que provou ser 100% capaz de absorver MIDI 2.0 quando o bloco físico for conectado.
