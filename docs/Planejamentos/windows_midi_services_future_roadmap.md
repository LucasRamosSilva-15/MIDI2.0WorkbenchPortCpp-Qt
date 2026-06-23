# Windows MIDI Services Future Roadmap - v4.12.0+

## Objetivo

Este documento define o planejamento estrutural das próximas versões experimentais de integração com o motor nativo Windows MIDI Services.

A estratégia segue uma evolução controlada e metódica:

`Pesquisa` → `Detecção` → `Compilação isolada` → `Projeção C++/WinRT` → `API Runtime` → `Backend real`

> **Regra de Ouro:** Nenhuma versão deve pular etapas.

---

## v4.12.0 - Windows MIDI Services C++/WinRT generated projection strategy research
**Objetivo:** Investigar como o SDK Windows MIDI Services gera e expõe suas projeções C++/WinRT.
* **Será feito:**
  * Análise dos headers gerados e arquivos `.winmd`.
  * Documentação da relação entre C++/WinRT, `cppwinrt`, projeções geradas e headers `winrt/*`.
  * Criação de Probe de diagnóstico.
* **Não será feito:**
  * Criação de objetos WinRT, abertura de dispositivos, captura UMP ou endpoint listing.
* **Resultado esperado:** Ter conhecimento estrutural de como uma futura integração real deve ser compilada de forma coesa.

## v4.13.0 - Windows MIDI Services isolated WinRT activation experiment
**Objetivo:** Testar uma ativação mínima de tipos WinRT sem integrar ao backend.
* **Será feito:** Criar uma aplicação/probe pequeno e isolado, validar carregamento de tipos MIDI 2.0 e verificar inicialização do ambiente WinRT.
* **Não será feito:** Comunicação com dispositivos, callbacks, captura de eventos.
* **Resultado esperado:** Confirmar se o ecossistema C++/WinRT está vivo e funcional.

## v4.14.0 - Windows MIDI Services API surface mapping
**Objetivo:** Mapear arquiteturalmente as APIs disponíveis do Windows MIDI Services.
* **Será feito:** Documentar classes principais, interfaces, objetos vitais e ciclo de vida de conexões (ex: devices, endpoints, sessions, UMP messages).
* **Não será feito:** Backend definitivo.
* **Resultado esperado:** Criar um mapa de engenharia para implementação futura.

## v4.15.0 - Windows MIDI Services endpoint enumeration prototype
**Objetivo:** Criar o primeiro protótipo controlado de listagem real de endpoints.
* **Será feito:** Enumerar endpoints MIDI, retornar metadados básicos e manter estrito isolamento em testes.
* **Não será feito:** Integração com UI, seleção de portas pelo usuário ou captura contínua.
* **Resultado esperado:** Primeiro contato físico real com dispositivos MIDI através do motor Microsoft.

## v4.16.0 - Windows MIDI Services endpoint metadata research
**Objetivo:** Aprofundar os estudos das informações retornadas pelos endpoints da v4.15.0.
* **Será feito:** Coletar Nome, Fabricante, Transporte, Capacidades MIDI 2.0 e Propriedades nativas disponíveis.
* **Resultado esperado:** Definir e mapear o modelo de dados interno do nosso Analisador para refletir a topologia real dos Dispositivos.

## v4.17.0 - Windows MIDI Services input stream prototype
**Objetivo:** Criar um protótipo de recebimento de mensagens UMP.
* **Será feito:** Abrir um endpoint experimental, injetar *callbacks* e receber mensagens, convertendo-as para as estruturas nativas do nosso Parser.
* **Restrições:** Não substituir o `FakeBackend` ou `RtMidi` de forma alguma.
* **Resultado esperado:** Nascimento do primeiro fluxo contínuo real: `WMS → UMP → Parser`.

## v4.18.0 - Windows MIDI Services backend integration preparation
**Objetivo:** Preparar integração base para satisfazer a interface `IUmpInputBackend`.
* **Será feito:** Gerenciar estados de conexão, orquestrar `sessions` e encapsular as adaptações de mensagens. Sem ativação por padrão na UI.
* **Resultado esperado:** A classe `WindowsMidiServicesBackend` evolui do seu atual estado de "Skeleton" vazio para um orquestrador dormente.

## v4.19.0 - Windows MIDI Services backend experimental
**Objetivo:** Criar o primeiro backend funcional atrelado ao ecossistema do App.
* **Fluxo:** `Hardware MIDI` ↓ `WMS` ↓ `WindowsMidiServicesBackend` ↓ `IUmpInputBackend` ↓ `UmpParser`
* **Será feito:** Captura real contínua, rotinas de polling/callbacks e tratamento profundo de erros de leitura.
* **Não será feito:** Substituir ou matar backends existentes.

## v4.20.0 - Windows MIDI Services UI integration research
**Objetivo:** Planejar como a arquitetura reagirá à troca a quente (*hot-swap*) de backends.
* **Será feito:** Pesquisar seletores de dispositivos, troca segura de backends, mutexes e displays de status na GUI.
* **Não será feito:** Implementar a UI definitiva gráfica de imediato.

## v4.21.0 - Windows MIDI Services backend selector
**Objetivo:** Adicionar o seletor visual experimental de backends à `MainWindow`.
* **Possíveis opções interativas:**
  * Fake UMP
  * RtMidi (API Tradicional)
  * Windows MIDI Services (UMP Nativo)
* **Resultado esperado:** Poder total ao usuário de comutar as fontes de entrada em tempo real.

## v4.22.0 - Windows MIDI Services production hardening
**Objetivo:** Lapidar e endurecer a integração rumo ao estado de Produção.
* **Será feito:** Tratamento extensivo de crashs, ampliação dos testes locais, polimento de documentação e revisão da resiliência inter-processos.

---

# Regras Permanentes (Imutáveis)

**Todas as versões devem OBRIGATORIAMENTE:**
* Manter *Builds Offline* operacionais. Nenhuma dependência não instalada deve quebrar o CMake.
* Preservar o `FakeUmpInputBackend`.
* Manter o `RtMidi` totalmente independente.
* Atualizar documentação e Checklists.
* Isolar riscos em testes separados.

**NUNCA:**
* Remover os backends legados.
* Obrigar usuários normais a instalarem o WMS SDK Runtime.
* Quebrar builds de ecossistemas paralelos (Mac/Linux cross-compiles).
* Misturar protótipos experimentais com rotinas consolidadas de UI.

---

# Estado Final Esperado (Triunvirato)

Ao final da jornada investigativa, o projeto desfrutará de três pilares de leitura robustos e independentes, todos centralizados na interface `IUmpInputBackend`:

1. **Offline:** (Fake UMP) - Garantindo estudos, testes CI/CD contínuos e demonstrações acadêmicas sem hardware.
2. **Cross-platform:** (RtMidi) - Mantendo sobrevida imortal para os clássicos hardwares MIDI 1.0 através de conversores lógicos MT 0x2.
3. **Windows Native:** (Windows MIDI Services) - Extirpando limitações de bytes passadas, abrindo as comportas de Hardware UMP de 128 Bits sob altíssima resolução.
