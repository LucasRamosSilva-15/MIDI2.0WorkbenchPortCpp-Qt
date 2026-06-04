# Windows MIDI Services Notes

O Windows MIDI Services é a iniciativa agressiva da Microsoft para substituir integralmente o arcaico subsistema WinMM de 1991, dotando o Windows 10/11 de capacidades diretas em transporte *Universal MIDI Packet* (UMP) e protocolos de negociação de propriedade (MIDI-CI).

## Dependências Prováveis
Para compilação nativa no *Visual Studio / MSVC / CMake*:
- Instalação oficial do Windows SDK mais recente (versão suportada pelo Windows 11 23H2/24H2).
- Conjunto de pacotes `Microsoft.Windows.Devices.Midi2` (frequentemente distribuídos em NuGet ou releases open-source no repositório oficial da Microsoft).
- Abstrações WinRT e/ou C++/WinRT projections para acessar as chamadas COM modernas.

## Perguntas Críticas de Integração (C++ / Qt / MSVC)
- **Bloqueio Assíncrono:** As chamadas de requisição MIDI-CI ou mapeamento UMP no WinRT são fortemente enraizadas em chamadas "IAsyncOperation". Como sincronizar elegantemente funções `.get()` bloqueantes na *worker thread* do backend sem impactar o Qt Event Loop?
- **Despacho Qt/CMake:** Como automatizar a instalação das *.dll* Runtime dependentes do MIDI Services junto do `windeployqt` sem falhas em distribuições isoladas *Zip* portáteis?
- **Namespace Collision:** Os headers do *Windows.h* aliados com os do Qt costumam sofrer vazamentos bizarros de MACROS. Um *PIMPL* (Pointer to implementation) pesado será exigido na classe C++ isolando completamente o backend das bibliotecas Qt base.

## Riscos Principais
- **A API é uma "Target Fixa em Movimento":** Ainda sofre quebras estruturais (Breaking Changes) ou updates massivos entre as versões Dev/Preview da Microsoft.
- **Portabilidade:** Usuários presos em edições mais antigas do Windows 10 podem receber falha total na inicialização (*DLL Not Found* ou *Class Not Registered*).

## Referências Úteis (Leitura do Desenvolvedor)
- O desenvolvedor deverá buscar pela *Windows MIDI Services API Reference* na documentação técnica da Microsoft (*learn.microsoft.com*).
- Repositório-chave open source: `microsoft/MIDI` no GitHub (para analisar os wrappers, os endpoints e o comportamento da runtime Console fornecida pela equipe do Windows).

**Aviso Formal v3.0.0:** 
Esta documentação é puramente investigativa. O código fonte atual NÃO requer e **NÃO** baixe SDKs do Windows MIDI Services, nem altera o `CMakeLists.txt` injetando dependências, preservando a portabilidade absoluta estabelecida na base do framework.
