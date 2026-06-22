#include "MidiInputController.h"
#include "FakeUmpInputBackend.h"
#include "WindowsMidiServicesBackend.h"

MidiInputController::MidiInputController() : m_backend(std::make_unique<FakeUmpInputBackend>()), m_activeType(UmpBackendType::FakeUmp) {
}

MidiInputController::~MidiInputController() {
    closePort();
}

void MidiInputController::switchBackend(UmpBackendType newType) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_activeType == newType && m_backend) return;
    
    if (m_backend && m_backend->isOpen()) {
        m_backend->closeInputPort();
    }
    
    if (newType == UmpBackendType::WindowsMidiServices) {
#ifdef WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE_ENABLED
        m_backend = std::make_unique<WindowsMidiServicesBackend>();
#else
        m_backend = std::make_unique<FakeUmpInputBackend>();
        newType = UmpBackendType::FakeUmp;
#endif
    } else {
        m_backend = std::make_unique<FakeUmpInputBackend>();
    }
    
    m_activeType = newType;
}

UmpBackendType MidiInputController::getActiveBackendType() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeType;
}

QString MidiInputController::getActiveBackendName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_backend) return "None";
    return m_backend->backendName();
}

QStringList MidiInputController::getAvailablePorts() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_backend) return QStringList();
    return m_backend->listInputPorts();
}

bool MidiInputController::openPort(int index) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_backend) return false;
    return m_backend->openInputPort(index);
}

void MidiInputController::closePort() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_backend && m_backend->isOpen()) {
        m_backend->closeInputPort();
    }
}

bool MidiInputController::isPortOpen() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_backend) return false;
    return m_backend->isOpen();
}

std::vector<UmpRawEvent> MidiInputController::pollNewEvents() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_backend || !m_backend->isOpen()) {
        return {};
    }
    return m_backend->pollUmpEvents();
}
