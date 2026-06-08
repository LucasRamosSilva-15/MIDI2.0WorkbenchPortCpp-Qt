# TCC Final Demo Checklist

Use este checklist nos instantes finais antes da sua apresentação para garantir que nada técnico frustre a operação da ferramenta.

## Validações de Ambiente
- [ ] O binário de versão padrão (*Offline puro*) abre perfeitamente.
- [ ] O binário hibridizado `-rtmidi` abre perfeitamente.
- [ ] O arquivo README e o CHANGELOG.md estão abertos numa aba pronta para mostrar ao orientador as atualizações da versão `v3.8.0`.

## Checagem Funcional da GUI
- [ ] O módulo **Offline UMP Analyzer** consegue carregar um arquivo didático em `.txt` e preencher as colunas quando se clica em 'Interpret'.
- [ ] O módulo **Live MIDI Monitor** varre corretamente os periféricos Windows (somente no ZIP RtMidi).
- [ ] O módulo **Experimental UMP Backend** mostra a porta falsa simuladora `Fake UMP Port 1`.
- [ ] Quando o *Polling* é pressionado, pacotes gerados pipocam saudavelmente na tela sem corromper ou travar a UI.

## Sistema de Relatórios IO
- [ ] O exportador *TXT* da tabela experimental não apresenta falha gráfica/arquivo vazio.
- [ ] O exportador *CSV* da tabela formata pacotes numéricos com string cravada (`="VAL"`), preservando a legibilidade limpa nas planilhas.
- [ ] Clicar em *Gravar Sessão* incendeia o Status para vermelho, e os eventos da interface se acumulam no vetor oculto.
- [ ] A exportação TXT do **Fake UMP Session Summary Report** extrai com glória acadêmica o arquivo final sem dar "*Aviso de sessão vazia*" (tendo dados populados).

## Engenharia e Backups
- [ ] Executar o `tests\run_tests.ps1` uma última vez pelo terminal comprovando `54 / 54 PASS`.
- [ ] Deixar a pasta com capturas de tela minimizada.
- [ ] Deixar os pacotes oficiais (`dist\*.zip`) amostrais provando a Integração Contínua (CI).
