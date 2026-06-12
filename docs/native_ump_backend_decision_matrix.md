# Native UMP Backend Decision Matrix

Esta matriz delineia os parâmetros críticos investigados durante a fase exploratória (v4.0.0) para nortear o futuro acoplamento com hardware real.

| Backend / Recurso | Windows MIDI Services | ALSA UMP (Linux) | libremidi | JUCE | Manter FakeUmpInputBackend |
|---|---|---|---|---|---|
| **Suporte UMP Real?** | ✅ Sim | ✅ Sim | ⚠️ Parcial/Em maturação | ✅ Sim (Massivo) | ❌ Não (Sintético Mock) |
| **Integração c/ Windows?**| ✅ Nativa Core | ❌ N/A | ✅ Via Abstrações | ✅ Nativa | ✅ Plena C++ Puro |
| **Integração MSVC/CMake?**| ⚠️ Requer WinRT/SDK | ❌ Somente GCC/Linux | ⚠️ Médio Risco | ❌ Destrutiva ao Qt | ✅ Plug-and-Play |
| **Risco p/ o Projeto?** | Moderado (API Nova) | Baixo (Restrito) | Alto (Lib Externa) | Altíssimo (Troca UI) | Nenhum (Garante build) |
| **Dependências Externas?**| SDK/Tools Microsoft | ALSA Header Libs | libremidi submodules | JUCE Framework Core | Mero Qt6/C++ |
| **Valor p/ Pesquisa TCC?**| ⭐ Excelente (Novo) | ⭐ Ótimo (Expansão) | ⚠️ Terceiriza Lógica | ⚠️ Desvia o Foco | ⭐ Fundamental (Testes) |
| **Maturidade API** | Crescente Rápido | Sólida | Variável | Sólida (Mercado) | Cimentada Interna |
| **Complexidade Geral** | Média/Alta (C++ COM)| Média (C/Linux) | Alta | Extrema (Migração) | Zero |

### Conclusão Técnica
O motor nativo **Windows MIDI Services** foi eleito como candidato primário devido à base de pesquisa e plataforma prioritária (Qt6 MSVC Desktop) do projeto atual. A adoção direta da fonte sem um intermédio externo maciço coroa a finalidade de "Dissecador Forense".

O **FakeUmpInputBackend** será permanentemente fixado como camada primária deFallback (Rede de Segurança) viabilizando builds em laboratórios desprovidos da última atualização da Microsoft ou hardware físico caro.

Alternativas secundárias: `ALSA UMP` aguardará demanda futura baseada em SO. Ferramentas abstrativas pesadas como `JUCE` violariam os dogmas fundamentais de interface nativa estática e, como o `libremidi`, devem ser despriorizadas neste cenário do semestre acadêmico.
