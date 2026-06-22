#include "MidiInputController.h"
#include "FakeUmpInputBackend.h"
#include "WindowsMidiServicesBackend.h"

MidiInputController::MidiInputController() : m_backend(std::make_unique<FakeUmpInputBackend>()), m_activeType(UmpBackendType::FakeUmp) {
}

MidiInputController::~MidiInputController() {
    closePort();
}

void MidiInputController::switchBackend(UmpBackendType newType) {
    if (m_activeType == newType && m_backend) return;
    
    closePort();
    
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
    return m_activeType;
}

QString MidiInputController::getActiveBackendName() const {
    if (!m_backend) return "None";
    return m_backend->backendName();
}

QStringList MidiInputController::getAvailablePorts() const {
    if (!m_backend) return QStringList();
    return m_backend->listInputPorts();
}

bool MidiInputController::openPort(int index) {
    if (!m_backend) return false;
    return m_backend->openInputPort(index);
}

void MidiInputController::closePort() {
    if (m_backend && m_backend->isOpen()) {
        m_backend->closeInputPort();
    }
}

bool MidiInputController::isPortOpen() const {
    if (!m_backend) return false;
    return m_backend->isOpen();
}

std::vector<UmpRawEvent> MidiInputController::pollNewEvents() {
    if (!m_backend || !m_backend->isOpen()) {
        return {};
    }
    return m_backend->pollUmpEvents();
}
