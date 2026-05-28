#include "MidiInputController.h"

MidiInputController::MidiInputController() : m_backend(nullptr) {
}

MidiInputController::~MidiInputController() {
    closePort();
}

void MidiInputController::setBackend(std::unique_ptr<IMidiInputBackend> backend) {
    closePort();
    m_backend = std::move(backend);
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

std::vector<MidiRawEvent> MidiInputController::pollNewEvents() {
    if (!m_backend || !m_backend->isOpen()) {
        return {};
    }
    return m_backend->pollEvents();
}
