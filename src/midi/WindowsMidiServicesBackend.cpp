#include "WindowsMidiServicesBackend.h"

WindowsMidiServicesBackend::WindowsMidiServicesBackend() : m_isOpen(false) {
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
    QStringList ports;
    
#ifdef USE_WINDOWS_MIDI_SERVICES
    // Futuro (v4.2.0): Listagem real de endpoints usando winrt::Windows::Devices::Midi2
    // Por enquanto, retorna vazio e documenta.
#endif

    return ports;
}

bool WindowsMidiServicesBackend::openInputPort(int portIndex) {
    (void)portIndex;
    std::lock_guard<std::mutex> lock(m_mutex);
    // Não conecta em nada ainda.
    m_isOpen = false; 
    return false;
}

void WindowsMidiServicesBackend::closeInputPort() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isOpen = false;
}

bool WindowsMidiServicesBackend::isOpen() const {
    return m_isOpen;
}

std::vector<UmpRawEvent> WindowsMidiServicesBackend::pollUmpEvents() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<UmpRawEvent> events;
    // Não envia UMP words. Mock estéril.
    return events;
}
