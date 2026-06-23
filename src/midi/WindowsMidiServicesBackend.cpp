#include "WindowsMidiServicesBackend.h"
#include <QDebug>
#include <QSettings>
#include <QEventLoop>

WmsWorker::WmsWorker(std::shared_ptr<SharedBuffer> sharedBuffer, QObject *parent)
    : QObject(parent), m_sharedBuffer(sharedBuffer) {}

WmsWorker::~WmsWorker() {
    closePort();
    if (m_mtaInitialized) {
        winrt::uninit_apartment();
    }
}

void WmsWorker::doInit() {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        m_mtaInitialized = true;
    } catch (...) {
        emit errorReported("Failed to initialize MTA COM apartment in Worker Thread.");
    }
}

void WmsWorker::queryAvailableEndpoints() {
    QStringList names;
    QMap<QString, QString> idMap;

    try {
        // GARANTIA DE INICIALIZAÇÃO NA THREAD DO WORKER
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
        } catch (...) { /* Ignora se já estiver inicializado nesta thread */ }

        auto endpoints = winrt::Microsoft::Windows::Devices::Midi2::MidiEndpointDeviceInformation::FindAll();
        for (uint32_t i = 0; i < endpoints.Size(); ++i) {
            auto ep = endpoints.GetAt(i);
            QString name = QString::fromStdWString(std::wstring(ep.Name()));
            QString id = QString::fromStdWString(std::wstring(ep.EndpointDeviceId()));
            
            names.append(name);
            idMap.insert(name, id);
        }

        if (names.isEmpty()) {
            names.append("Zero dispositivos retornados pelo FindAll nativo.");
        }
    } catch (const winrt::hresult_error& ex) {
        // EXTRAÇÃO DO HRESULT E MENSAGEM
        QString errorCode = QString::number(static_cast<uint32_t>(ex.code()), 16);
        QString errorMsg = QString::fromStdWString(std::wstring(ex.message()));
        names.append(QString("Erro WinRT (0x%1): %2").arg(errorCode, errorMsg));
    }

    // Emita o sinal com as listas de volta para a MainWindow/Fachada
    emit endpointsDiscovered(names, idMap);
}

void WmsWorker::openPort(QString deviceId, QString portName) {
    try {
        if (!m_mtaInitialized) {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            m_mtaInitialized = true;
        }
        
        m_openedPortName = portName;

        winrt::hstring hDeviceId(deviceId.toStdWString());
        m_session = winrt::Microsoft::Windows::Devices::Midi2::MidiSession::Create(L"MidiUmpAnalyzerSession");
        m_endpoint = m_session.CreateEndpointConnection(hDeviceId);
        
        // Proteção do Ciclo de Vida: Captura estritamente o SharedBuffer
        auto sharedBuf = m_sharedBuffer;
        QString capturedPortName = m_openedPortName;
        m_eventToken = m_endpoint.MessageReceived([sharedBuf, capturedPortName](auto const& /*sender*/, winrt::Microsoft::Windows::Devices::Midi2::MidiMessageReceivedEventArgs const& args) {
            std::lock_guard<std::mutex> bufferLock(sharedBuf->mutex);
            UmpRawEvent ev;
            ev.backendName = "Windows MIDI Services (Native)";
            ev.portName = capturedPortName;
            ev.umpWords.push_back(args.PeekFirstWord());
            
            uint8_t mt = (ev.umpWords[0] >> 28) & 0xF;
            int expectedWords = 1;
            if (mt == 0x2 || mt == 0x3) expectedWords = 2;
            else if (mt == 0x4) expectedWords = 3;
            else if (mt == 0xF || mt == 0x5) expectedWords = 4;
            
            sharedBuf->buffer.push_back(ev);
        });
        
        m_endpoint.Open();
        emit portOpened(true, "Porta WinRT UMP conectada com sucesso via MTA QThread!");
    } catch (const winrt::hresult_error& ex) {
        emit portOpened(false, QString("WinRT COM Exception: %1").arg(QString::fromStdWString(std::wstring(ex.message()))));
    } catch (...) {
        emit portOpened(false, "Unknown exception during openPort in WmsWorker.");
    }
}

void WmsWorker::closePort() {
    try {
        if (m_endpoint) m_endpoint.MessageReceived(m_eventToken);
        if (m_session) m_session.Close();
        m_endpoint = nullptr;
        m_session = nullptr;
    } catch (...) {
        emit errorReported("Exceção silenciosa capturada durante o encerramento do UMP Endpoint.");
    }
    emit portClosed();
}


WindowsMidiServicesBackend::WindowsMidiServicesBackend() 
    : m_state(ConnectionState::Disconnected), m_isOpen(false) {
    
    m_sharedBuffer = std::make_shared<SharedBuffer>();
    m_worker = new WmsWorker(m_sharedBuffer);
    m_worker->moveToThread(&m_workerThread);

    // Mapeamento de Sinais Fachada (STA) -> Worker (MTA)
    connect(this, &WindowsMidiServicesBackend::requestInit, m_worker, &WmsWorker::doInit);
    connect(this, &WindowsMidiServicesBackend::requestEndpoints, m_worker, &WmsWorker::queryAvailableEndpoints);
    connect(this, &WindowsMidiServicesBackend::requestOpenPort, m_worker, &WmsWorker::openPort);
    connect(this, &WindowsMidiServicesBackend::requestClosePort, m_worker, &WmsWorker::closePort);

    // Mapeamento de Erros
    connect(m_worker, &WmsWorker::errorReported, this, [](QString err) {
        qWarning() << "[WMS Worker Error]" << err;
    });

    // Cleanup seguro da QThread
    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    m_workerThread.start();
    emit requestInit();
}

WindowsMidiServicesBackend::~WindowsMidiServicesBackend() {
    closeInputPort();
    m_workerThread.quit();
    m_workerThread.wait();
}

QString WindowsMidiServicesBackend::backendName() const {
#ifdef USE_WINDOWS_MIDI_SERVICES
    return "Windows MIDI Services Backend (v4.25 MTA Worker)";
#else
    return "Windows MIDI Services Backend (v4.25 skeleton)";
#endif
}

QStringList WindowsMidiServicesBackend::listInputPorts() {
    QEventLoop loop;
    auto conn = connect(m_worker, &WmsWorker::endpointsDiscovered, [&](QStringList names, QMap<QString, QString> idMap) {
        m_cachedEndpointNames = names;
        m_cachedEndpointMap = idMap;
        loop.quit();
    });
    
    emit requestEndpoints();
    loop.exec();
    disconnect(conn);
    
    return m_cachedEndpointNames;
}

bool WindowsMidiServicesBackend::openInputPort(int portIndex) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    
    if (portIndex < 0 || portIndex >= m_cachedEndpointNames.size()) {
        m_lastError = "Índice de porta inválido.";
        m_state = ConnectionState::Error;
        return false;
    }
    
    QString name = m_cachedEndpointNames.at(portIndex);
    QString deviceId = m_cachedEndpointMap.value(name);
    
    if (deviceId.isEmpty()) {
        m_lastError = "Nenhum ID físico encontrado. Falha no Sandbox Bypass.";
        m_state = ConnectionState::Error;
        return false;
    }

    bool success = false;
    QEventLoop loop;
    auto conn = connect(m_worker, &WmsWorker::portOpened, [&](bool s, QString msg) {
        success = s;
        m_lastError = msg;
        loop.quit();
    });
    
    emit requestOpenPort(deviceId, name);
    loop.exec();
    disconnect(conn);
    
    m_isOpen = success;
    m_state = success ? ConnectionState::Active : ConnectionState::Error;
    return success;
}

void WindowsMidiServicesBackend::closeInputPort() {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    if (!m_isOpen) return;

    QEventLoop loop;
    auto conn = connect(m_worker, &WmsWorker::portClosed, &loop, &QEventLoop::quit);
    
    emit requestClosePort();
    loop.exec();
    disconnect(conn);
    
    m_isOpen = false;
    m_state = ConnectionState::Disconnected;
}

bool WindowsMidiServicesBackend::isOpen() const {
    return m_isOpen;
}

WindowsMidiServicesBackend::ConnectionState WindowsMidiServicesBackend::getState() const {
    return m_state;
}

QString WindowsMidiServicesBackend::getLastError() const {
    return m_lastError;
}

std::vector<UmpRawEvent> WindowsMidiServicesBackend::pollUmpEvents() {
    std::vector<UmpRawEvent> eventsToReturn;
    if (m_sharedBuffer) {
        std::lock_guard<std::mutex> lock(m_sharedBuffer->mutex);
        eventsToReturn = std::move(m_sharedBuffer->buffer);
        m_sharedBuffer->buffer.clear();
    }
    return eventsToReturn;
}
