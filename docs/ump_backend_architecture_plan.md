# UMP Backend Architecture Plan

Para garantir que a integração futura de suporte UMP nativo seja resiliente, projetamos uma arquitetura focada em injeção de dependência e polimorfismo, preservando a estabilidade e impedindo bloqueios visuais na GUI em Qt6.

## Proposta da Interface `IumpInputBackend`
Uma classe estritamente virtual pura que deve ser preenchida pelas futuras implementações reais (Windows, ALSA, etc).
```cpp
class IumpInputBackend {
public:
    virtual ~IumpInputBackend() = default;
    virtual QString backendName() const = 0;
    virtual QStringList listInputPorts() = 0;
    virtual bool openInputPort(int portIndex) = 0;
    virtual void closeInputPort() = 0;
    virtual bool isOpen() const = 0;
    virtual std::vector<UmpRawEvent> pollUmpEvents() = 0;
};
```

## Proposta da Estrutura `UmpRawEvent`
Contêiner que armazena temporalmente e empacota perfeitamente o buffer recebido da interrupção do Driver:
```cpp
struct UmpRawEvent {
    InputSourceType sourceType;
    double timestampMs = 0.0;
    std::vector<uint32_t> umpWords; // Words puras de 32 bits (1 para MT 0x2, 2 para MT 0x4, etc.)
    QString backendName;
    QString portName;
};
```

## Fluxo Futuro Projetado

A extração deve convergir para uma espinha dorsal única (pollUmpEvents), onde a fonte geradora das words (*Windows API*, *ALSA* ou *Software Fake*) não importa mais para a Interface Gráfica. O fluxo será o seguinte:

```text
[ WindowsMidiServicesBackend / AlsaUmpBackend / FakeUmpBackend ]
    | (Despejam words hexadecimais numa fila com std::mutex)
    v
[ IumpInputBackend ]
    |
    v
[ MainWindow::pollUmpEvents() ] (QTimer invocando de forma não blocante na UI)
    |
    v
[ UmpParser ] (Reaproveitado, validando a integridade do pacote live C++)
    |
    v
[ Live UMP Table & Statistics ] (Atualização gráfica da grid)
    |
    v
[ Session Recording / Export ] (Armazenagem vetorial perene na RAM)
```

## O Papel do `FakeUmpBackend` (Testabilidade)
Para desenvolvermos MIDI 2.0 sem a barreira financeira de possuir um teclado físico premium 2.0 (como o Roland A-88MKII ou Korg Keystage), criaremos um **FakeUmpBackend**.
Esta subclasse injetará arrays sintéticos pré-programados de MT 0x4 diretamente na Interface, provendo ao desenvolvedor e às suítes de testes unitários a falsa impressão de que um hardware validado está conectado.

## Isolamento do UmpParser e Callback Protegido (Thread Safety)
### Por que o UmpParser não deve depender da UI?
O motor estático deve continuar a receber exclusivamente a sequência numérica bruta (`std::vector<uint32_t>`). Se a validação não estiver amarrada ao framework `QWidget`, conseguiremos invocar o parser diretamente via CTest e console CI/CD.

### Bloqueio do Callback Subjacente
Quando a *Audio Thread* do SO (seja via WinRT no Windows ou interrupção ALSA no Linux) emitir UMP, não podemos de maneira alguma chamar código manipulador gráfico Qt (como atualizar texto de *QTableWidget*). O sistema travaria catastroficamente por violação de *Thread-Safety*. 
O *Backend* deve se limitar a copiar os pacotes em um cache isolado e silencioso. Em paralelo, a Main Thread do Qt continuará invocando um `QTimer` para verificar a fila de maneira rítmica (`pollUmpEvents()`), recuperando pacotes em segurança no exato momento que a interface estiver ociosa.

## Entregas da v3.1.0 e v3.2.0
A versão v3.1.0 consolidou materialmente as classes propostas: criação do `UmpRawEvent`, da interface `IUmpInputBackend` e do simulador `FakeUmpInputBackend` para CTest.
A versão v3.2.0 materializou o fluxo visual: a interface Qt agora instancia o `FakeUmpInputBackend`, executa polling passivo (500ms~1000ms via `QTimer`), decodifica o cabeçalho (*MT/Group/Size*) localmente e submete o *hexadecimal derivado* para o `UmpParser` resolver, preservando os motores legados intocados.

## Evoluções da v3.5.0 (Session Recording)
A `v3.5.0` materializou a etapa abstrata de `Session Recording / Export` desenhada na arquitetura acima. Agora, a aplicação suporta "Fake UMP Session Recording", implementando uma camada robusta de gravação em memória independente da tabela visual (`QTableWidget`). Eventos UMP recebidos continuam sendo guardados mesmo se a interface for obliterada, possuindo exportação CSV/TXT separada que imita uma transação densa perene. Importante: tudo atua sob a mecânica do *FakeUmpInputBackend* e ainda não processa hardware *MIDI 2.0* real.

## Evoluções da v3.6.0 (Session Summary Report)
A versão `v3.6.0` adiciona um relatório de resumo da sessão UMP fake. O resumo varre a memória transiente do gravador independente (`m_fakeUmpRecording`), agrupa matematicamente totais e eventos por tipo, canal, e formato, elaborando uma exportação TXT com amostras forenses (`Representative Examples`). Este sistema puramente lógico funciona à base do *Fake UMP Backend* sem intervir no pipeline legado ou alocar buffer em discos lentos.

## v3.7.0 Documentation and demo polish
A versão `v3.7.0` encerra as preparações de *software* ao estender guias didáticos para as bancas acadêmicas, deixando claro e explícito que a interface interage com um **software mock** (*FakeUmpInputBackend*). O próximo salto técnico real projetado para a linha `v4.x` seria a construção nativa de um backend conectado a *Windows MIDI Services* ou *ALSA UMP*.

## v4.x Native UMP backend direction
Esta espinha dorsal puramente arquitetural arrasta o projeto de encontro ao Hardware real.
A linha `v4.x` tem caráter **estritamente de pesquisa e viabilidade**. Nenhum acoplamento é implementado de surpresa, testando o peso e o impacto na compilação host.
A arquitetura Polimórfica C++ futura abraçará de maneira modular os desdobramentos:

```text
[ IUmpInputBackend ] (Interface Absoluta Mestra)
    |
    ├── [ FakeUmpInputBackend ] (O Fallback Eterno e Testador TDD, existente)
    |
    ├── [ WindowsMidiServicesBackend ] (Foco da Linha v4.x - Microsoft OS Kernel, futuro)
    |
    ├── [ AlsaUmpBackend ] (Alvo Linus secundário para portabilidade futura)
    |
    └── [ LibremidiBackend ] (Dependência externa híbrida futura, a avaliar se benéfica)
```
A fundação teórica desta migração se inicia pacificamente na Release **v4.0.0**.
