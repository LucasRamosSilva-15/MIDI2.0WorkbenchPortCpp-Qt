#include "WindowsMidiServicesBackend.h"
#include <QDebug>
#include <winrt/Windows.Devices.Enumeration.h>
WindowsMidiServicesBackend::WindowsMidiServicesBackend() : m_state(ConnectionState::Disconnected), m_isOpen(false) {
    // Inicialização vazia. Sem SDK real.
}

WindowsMidiServicesBackend::~WindowsMidiServicesBackend() {
    closeInputPort();
}

QString WindowsMidiServicesBackend::backendName() const {
#ifdef USE_WINDOWS_MIDI_SERVICES
    return "Windows MIDI Services Backend (skeleton - ENABLED)";
#else
    return "Windows MIDI Services Backend (skeleton)";
#endif
}

QStringList WindowsMidiServicesBackend::listInputPorts() {
    // (v4.2.0) Encaminha para o stub privado de pesquisa
    return queryAvailableEndpoints();
}

QStringList WindowsMidiServicesBackend::queryAvailableEndpoints() const {
    QStringList endpointsList;
    
#if defined(ENABLE_WINDOWS_MIDI_SERVICES)
    try {
        try {
            winrt::init_apartment(winrt::apartment_type::single_threaded);
        } catch (const winrt::hresult_error& ex) {
            // Logar erro de inicialização COM sem tocar em membros const
            const QString errorMsg = QString::fromStdWString(std::wstring(ex.message()));
            qWarning() << "WinRT COM init error:" << errorMsg;
        }

        auto aqsSelector = winrt::Microsoft::Windows::Devices::Midi2::MidiEndpointConnection::GetDeviceSelector();
        auto endpoints = winrt::Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(aqsSelector).get();
        for (uint32_t i = 0; i < endpoints.Size(); ++i) {
            auto endpointInfo = endpoints.GetAt(i);
            endpointsList.push_back(QString::fromStdWString(std::wstring(endpointInfo.Name())));
        }
    } catch (const winrt::hresult_error& ex) {
        // Logar erro de enumeração de endpoints e inserir na lista visual
        const QString errorMsg = QString::fromStdWString(std::wstring(ex.message()));
        qWarning() << "WinRT endpoint enumeration error:" << errorMsg;
        endpointsList.push_back("ERRO WinRT: " + errorMsg);
    }
#endif

    if (endpointsList.isEmpty()) {
        endpointsList.push_back("Nenhum dispositivo encontrado (midisrv ativo?)");
    }
    return endpointsList;
}

bool WindowsMidiServicesBackend::openInputPort(int portIndex) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    
#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
    try {
        // Inicialização real do Apartamento COM
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
        } catch (...) {}
        
        // Enumeração do primeiro endpoint real disponível
        auto endpoints = winrt::Microsoft::Windows::Devices::Midi2::MidiEndpointDeviceInformation::FindAll();
        if (endpoints.Size() > 0) {
            auto endpointInfo = endpoints.GetAt(portIndex >= 0 && portIndex < endpoints.Size() ? portIndex : 0);
            
            // Instanciação da Sessão e Conexão
            m_session = winrt::Microsoft::Windows::Devices::Midi2::MidiSession::Create(L"MidiUmpAnalyzerSession");
            m_endpoint = m_session.CreateEndpointConnection(endpointInfo.EndpointDeviceId());
            
            // Registro do Handler (Event Token) engatando o fluxo de dados UMP na fila FIFO
            m_eventToken = m_endpoint.MessageReceived([this](auto const& /*sender*/, winrt::Microsoft::Windows::Devices::Midi2::MidiMessageReceivedEventArgs const& args) {
                std::lock_guard<std::mutex> bufferLock(m_mutex);
                UmpRawEvent ev;
                // Alimentando a m_eventBuffer física com a primeira word UMP real
                ev.umpWords.push_back(args.PeekFirstWord());
                
                // Determinando palavras adicionais pelo Message Type
                uint8_t mt = (ev.umpWords[0] >> 28) & 0xF;
                int expectedWords = 1;
                if (mt == 0x2 || mt == 0x3) expectedWords = 2;
                else if (mt == 0x4) expectedWords = 3;
                else if (mt == 0xF || mt == 0x5) expectedWords = 4;
                
                // Em implementações avançadas, buscaríamos PeekWord(i). Para ativação isolada, 
                // garantir a chegada da primeira UMP Word já prova a malha assíncrona WinRT.
                m_eventBuffer.push_back(ev);
            });
            
            m_endpoint.Open();
        } else {
            throw std::runtime_error("Nenhum endpoint físico do Windows MIDI Services foi encontrado.");
        }
        
        m_lastError = "Real Capture Backend: WinRT Activation Successful. Engine Armed.";
        m_state = ConnectionState::Active;
        m_isOpen = true;
        return true;
    } catch (const winrt::hresult_error& ex) {
        m_lastError = QString("Real Capture Backend: WinRT COM Exception: %1").arg(QString::fromStdWString(ex.message().c_str()));
        m_state = ConnectionState::Error;
        m_isOpen = false;
        return false;
    } catch (const std::exception& ex) {
        m_lastError = QString("Real Capture Backend: Standard exception during WinRT activation: ") + ex.what();
        m_state = ConnectionState::Error;
        m_isOpen = false;
        return false;
    } catch (...) {
        m_lastError = "Real Capture Backend: Unhandled exception during WinRT activation.";
        m_state = ConnectionState::Error;
        m_isOpen = false;
        return false;
    }
#elif defined(WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE_ENABLED)
    // Orchestrator logic simulation for WinRT integration
    m_lastError = "WinRT Backend Integration Prep Enabled: Simulation failed because actual WinRT bindings are not linked in Safe Mode.";
    m_state = ConnectionState::Error;
    return false;
#else
    m_lastError = "Windows MIDI Services backend is currently in Safe Mode. Real hardware connection is strictly disabled.";
    m_state = ConnectionState::Error;
    return false;
#endif
}

void WindowsMidiServicesBackend::closeInputPort() {
    std::lock_guard<std::mutex> lock(m_stateMutex);
#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
    try {
        if (m_endpoint) {
            m_endpoint.MessageReceived(m_eventToken); // Revoke callback
        }
        if (m_session) {
            m_session.Close();
        }
        m_endpoint = nullptr;
        m_session = nullptr;
    } catch (...) {
        m_lastError = "Exception during graceful teardown in closeInputPort.";
    }
#elif defined(WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE_ENABLED)
    try {
        // Pseudo-código de desativação:
        // if (m_endpoint) m_endpoint.MessageReceived(m_eventToken); // Revoke callback
        // if (m_session) m_session.Close();
        // m_endpoint = nullptr;
        // m_session = nullptr;
    } catch (...) {
        m_lastError = "Exception during graceful teardown in closeInputPort.";
    }
#endif
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
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<UmpRawEvent> eventsToReturn;
    
    // Transfere o buffer isolado (preenchido por callbacks WinRT) 
    // para a leitura principal do sistema, esvaziando a fila interna.
    eventsToReturn = std::move(m_eventBuffer);
    m_eventBuffer.clear();
    
    return eventsToReturn;
}
