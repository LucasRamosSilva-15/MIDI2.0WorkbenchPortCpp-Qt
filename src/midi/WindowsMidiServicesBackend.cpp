#include "WindowsMidiServicesBackend.h"

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
    QStringList endpoints;
    
#ifdef USE_WINDOWS_MIDI_SERVICES
    // Futuro (v4.x): Listagem real usando winrt::Windows::Devices::Midi2
    // Por enquanto, retorna vazio blindando o sistema.
#endif

    return endpoints;
}

bool WindowsMidiServicesBackend::openInputPort(int portIndex) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    
#if defined(WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE_ENABLED)
    try {
        // Pseudo-código de Integração WinRT Ativa:
        // winrt::init_apartment();
        // auto session = MidiSession::CreateSession(L"MidiUmpAnalyzerSession");
        // auto endpoint = MidiEndpointConnection::GetEndpoint(deviceId);
        // session.Connect(endpoint);
        // endpoint.MessageReceived([this](auto const& sender, auto const& args) {
        //     std::lock_guard<std::mutex> bufferLock(m_mutex);
        //     // parse bytes to words -> append to m_eventBuffer
        // });
        
        m_lastError = "Experimental Capture Backend: Simulation armed. Try/Catch architecture in place.";
        m_state = ConnectionState::Active;
        m_isOpen = true;
        return true;
    } catch (...) {
        m_lastError = "Experimental Capture Backend: Unhandled exception during WinRT COM activation.";
        m_state = ConnectionState::Error;
        m_isOpen = false;
        return false;
    }
#elif defined(WINDOWS_MIDI_SERVICES_BACKEND_INTEGRATION_PREP_ENABLED)
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
